#include "websocketServer.h"
#include "notificationManager.h"
#include <iostream>
#include <thread>
#include <csignal>

std::atomic<bool> keepRunning(true);

void signalHandler(int signal) {
    std::cout << "Signal received: " << signal << ". Shutting down..." << std::endl;
    keepRunning.store(false);
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

	NotificationManager& manager = NotificationManager::getInstance();
    WebSocketServer server(manager);

    // Run the server in a separate thread
    std::thread serverThread([&server]() { server.run(); });

    std::cout << "Program running. Press Ctrl + C to terminate.\n";

    while (keepRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    // Stop the server and clean up
    server.stop();
    serverThread.join();

    std::cout << "Program exited gracefully." << std::endl;

    return 0;
}
