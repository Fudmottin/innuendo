// src/include/EventLoop.hpp
#pragma once

#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <chrono>

class EventLoop {
public:
    EventLoop()
        : io_context_(std::make_shared<boost::asio::io_context>()),
          work_guard_(boost::asio::make_work_guard(*io_context_))
    {}

    ~EventLoop() {
        stop();
    }

    // Run the loop (blocking)
    void run() {
        io_context_->run();
    }

    // Stop the loop
    void stop() {
        work_guard_.reset();
        io_context_->stop();
        for (auto& t : threads_) t.join();
        threads_.clear();
    }

    // Run loop in background threads
    void run_in_threads(std::size_t n_threads = 1) {
        for (std::size_t i = 0; i < n_threads; ++i) {
            threads_.emplace_back([ctx=io_context_]{ ctx->run(); });
        }
    }

    // Post a task to be executed asynchronously
    void post(std::function<void()> f) {
        boost::asio::post(*io_context_, std::move(f));
    }

    // Schedule a timer callback after delay_ms milliseconds
    void schedule_timer(int delay_ms, std::function<void()> f) {
        auto timer = std::make_shared<boost::asio::steady_timer>(*io_context_,
                          std::chrono::milliseconds(delay_ms));
        timer->async_wait([f, timer](const boost::system::error_code& ec){
            if (!ec) f();
        });
    }

    auto& get_io_context() { return *io_context_; }

private:
    std::shared_ptr<boost::asio::io_context> io_context_;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;
    std::vector<std::thread> threads_;
};

