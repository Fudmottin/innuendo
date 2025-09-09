// src/cli/main.cpp
#include "TorInstance.hpp"
#include "TorClient.hpp"

#include <ncursesw/ncurses.h>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <optional>
#include <csignal>

// --- RAII wrappers ---
struct NcursesSession {
    NcursesSession() {
        setlocale(LC_ALL, ""); // Unicode
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, true);
        curs_set(1);
    }
    ~NcursesSession() { endwin(); }
};

class Window {
    WINDOW* w_;
public:
    Window(int h, int w, int y, int x) : w_(newwin(h, w, y, x)) {}
    ~Window() { delwin(w_); }
    operator WINDOW*() const { return w_; }
    void resize(int h, int w, int y, int x) {
        wresize(w_, h, w);
        mvwin(w_, y, x);
        werase(w_);
        box(w_, 0, 0);
        wrefresh(w_);
    }
};

// --- globals ---
std::mutex q_mutex;
std::queue<std::string> incoming;
std::atomic<bool> running{true};

// --- network thread ---
void network_loop(TorClient& client) {
    try {
        while (running) {
            std::string msg = client.read_some(); // blocking
            {
                std::lock_guard<std::mutex> lock(q_mutex);
                incoming.push("peer: " + msg);
            }
        }
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(q_mutex);
        incoming.push(std::string("[network error] ") + e.what());
        running = false;
    }
}

// --- resize handler ---
std::atomic<bool> resized{false};
void handle_winch(int) { resized = true; }

int main() try {
    const std::string onion =
        "qyhw7f6jynynrcyffeploro2bnbssab3ed5few2nb3joj2sap2vinsid.onion";
    const unsigned short onion_port = 1440;

    TorInstance tor;
    tor.start();

    TorClient client(tor, onion, onion_port);
    client.connect();

    NcursesSession session;
    signal(SIGWINCH, handle_winch);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    Window msgwin(rows - 3, cols, 0, 0);
    Window inputwin(3, cols, rows - 3, 0);
    scrollok(msgwin, true);

    std::thread net(network_loop, std::ref(client));

    while (running) {
        // handle resize
        if (resized) {
            getmaxyx(stdscr, rows, cols);
            msgwin.resize(rows - 3, cols, 0, 0);
            inputwin.resize(3, cols, rows - 3, 0);
            resized = false;
        }

        // draw input box
        werase(inputwin);
        box(inputwin, 0, 0);
        wrefresh(inputwin);

        nodelay(inputwin, false); // blocking input
        char buf[256];
        mvwgetnstr(inputwin, 1, 1, buf, sizeof(buf) - 1);
        std::string line(buf);

        if (line == "/quit") {
            running = false;
            break;
        }

        if (!line.empty()) {
            wprintw(msgwin, "me: %s\n", line.c_str());
            wrefresh(msgwin);

            try {
                client.write_some(line + "\n");
            } catch (const std::exception& e) {
                wprintw(msgwin, "[send error] %s\n", e.what());
                wrefresh(msgwin);
                running = false;
            }
        }

        // display received messages
        {
            std::lock_guard<std::mutex> lock(q_mutex);
            while (!incoming.empty()) {
                wprintw(msgwin, "%s\n", incoming.front().c_str());
                incoming.pop();
            }
            wrefresh(msgwin);
        }
    }

    running = false;
    net.join();
    tor.stop();
    return 0;

} catch (const std::exception& e) {
    endwin();
    fprintf(stderr, "[FATAL] %s\n", e.what());
    return 1;
}

