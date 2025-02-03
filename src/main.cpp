#include "websocketServer.h"
#include "notificationManager.h"
#include "terminalUI.h"
#include <iostream>
#include <thread>
#include <csignal>
#include <future>

std::atomic<bool> keepRunning(true);
std::atomic<bool> terminateProgramme(false);

static void signalHandler(int signal) {
    if (terminateProgramme.load()) {
        std::cout << "\n[INFO] Signal received: " << signal << ". Exiting program..." << std::endl;
        keepRunning.store(false);
    }
    else {
        std::cout << "\n[INFO] Signal received: " << signal << ". Closing all connections..." << std::endl;
        terminateProgramme.store(true);
    }
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    NotificationManager& manager = NotificationManager::getInstance();
    WebSocketServer server(manager);

    std::thread serverThread([&server]() {
        try {
            server.run();
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] WebSocket server encountered an exception: " << e.what() << std::endl;
            keepRunning.store(false);
        }
        });

    // Ensure server starts before main loop
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    TerminalUI::displayIntro();

    while (keepRunning.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(10));

        if (terminateProgramme.load()) {
            std::cout << "[INFO] Stopping WebSocket server..." << std::endl;

            // Use async with timeout to stop server safely
            auto stopFuture = std::async(std::launch::async, [&server]() { server.stop(); });

            if (stopFuture.wait_for(std::chrono::seconds(5)) == std::future_status::timeout) {
                std::cerr << "[ERROR] Server stop() is taking too long. Forcing exit." << std::endl;
            }

            keepRunning.store(false);
        }
    }

    if (serverThread.joinable()) {
        serverThread.join();
    }

    std::cout << "[INFO] Program exited gracefully." << std::endl;
    return 0;
}

