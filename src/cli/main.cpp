// src/cli/main.cpp

#include "TorInstance.hpp"
#include "TorClient.hpp"

#include <ncursesw/ncurses.h>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <csignal>
#include <locale>

// --- RAII for ncurses init/cleanup ---
struct NcursesSession {
    NcursesSession() {
        setlocale(LC_ALL, ""); // Unicode support
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(1);
    }
    ~NcursesSession() { endwin(); }
};

// --- globals ---
std::mutex q_mutex;
std::queue<std::string> incoming;
std::atomic<bool> running{true};
std::atomic<bool> resized{false};

// --- network thread ---
void network_loop(TorClient& client) {
    try {
        while (running) {
            std::string msg = client.read_some(); // blocking
            std::lock_guard<std::mutex> lock(q_mutex);
            incoming.push("peer: " + msg);
        }
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(q_mutex);
        incoming.push(std::string("[network error] ") + e.what());
        running = false;
    }
}

// --- resize handler ---
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

    WINDOW* msgwin = newwin(rows - 3, cols, 0, 0);
    WINDOW* inputwin = newwin(3, cols, rows - 3, 0);
    scrollok(msgwin, TRUE);

    nodelay(inputwin, TRUE);  // non-blocking input
    keypad(inputwin, TRUE);

    std::thread net(network_loop, std::ref(client));

    std::string input_buffer;

    while (running) {
        // --- handle resize ---
        if (resized) {
            getmaxyx(stdscr, rows, cols);
            wresize(msgwin, rows - 3, cols);
            mvwin(msgwin, 0, 0);
            wresize(inputwin, 3, cols);
            mvwin(inputwin, rows - 3, 0);
            werase(msgwin);
            werase(inputwin);
            box(inputwin, 0, 0);
            wrefresh(msgwin);
            wrefresh(inputwin);
            resized = false;
        }

        // --- read input (non-blocking) ---
        int ch = wgetch(inputwin);
        while (ch != ERR) {
            if (ch == '\n') {
                if (input_buffer == "/quit") {
                    running = false;
                    input_buffer.clear();
                    break; // exit inner while
                }
                if (!input_buffer.empty()) {
                    try { client.write_some(input_buffer + "\n"); }
                    catch (const std::exception& e) {
                        std::lock_guard<std::mutex> lock(q_mutex);
                        incoming.push(std::string("[send error] ") + e.what());
                        running = false;
                        break;
                    }
                    std::lock_guard<std::mutex> lock(q_mutex);
                    incoming.push("me: " + input_buffer);
                    input_buffer.clear();
                }
            } else if (ch == KEY_BACKSPACE || ch == 127) {
                if (!input_buffer.empty()) input_buffer.pop_back();
            } else if (isprint(ch)) {
                input_buffer.push_back(static_cast<char>(ch));
            }
            ch = wgetch(inputwin);
        }

        // --- display incoming messages ---
        {
            std::lock_guard<std::mutex> lock(q_mutex);
            while (!incoming.empty()) {
                wprintw(msgwin, "%s\n", incoming.front().c_str());
                incoming.pop();
            }
            wrefresh(msgwin);
        }

        // --- redraw input buffer and cursor ---
        werase(inputwin);
        box(inputwin, 0, 0);
        mvwprintw(inputwin, 1, 1, "%s", input_buffer.c_str());
        wmove(inputwin, 1, 1 + input_buffer.size()); // cursor in input window
        wrefresh(inputwin);

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    running = false;
    net.join();
    delwin(msgwin);
    delwin(inputwin);
    tor.stop();

    return 0;

} catch (const std::exception& e) {
    endwin();
    fprintf(stderr, "[FATAL] %s\n", e.what());
    return 1;
}

