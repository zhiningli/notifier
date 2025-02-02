#include "websocketServer.h"
#include "notificationManager.h"
#include "terminalUI.h"
#include <iostream>
#include <thread>
#include <csignal>

std::atomic<bool> keepRunning(true);
std::atomic<bool> terminateProgramme(false);

void signalHandler(int signal) {
    if (terminateProgramme.load()) {
        std::cout << "\nSignal received: " << signal << ". Exiting program..." << std::endl;
        keepRunning.store(false);
    } else {
        std::cout << "\nSignal received: " << signal << ". Closing all connections..." << std::endl;
        terminateProgramme.store(true);
    }

}

int main() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

	NotificationManager& manager = NotificationManager::getInstance();
    WebSocketServer server(manager);

    // Run the server in a separate thread
    std::thread serverThread([&server]() { server.run(); });

	// Display the intro message
	TerminalUI::displayIntro();

    while (keepRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    // Stop the server and clean up
    server.stop();
    serverThread.join();

    std::cout << "Program exited gracefully." << std::endl;

    return 0;
}
