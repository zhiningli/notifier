#include "windows_api.h"
#include "shortcut_util.h"
#include "notification.h"
#include "notificationManager.h"
#include "websocketServer.h"
#include <iostream>
#include <csignal>
#include <thread>
#include <shobjidl.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <atomic>

std::atomic<bool> keepRunning(true);

void signalHandler(int signal) {
    std::cout << "Signal received: " << signal << ". Requesting shutdown..." << std::endl;
    keepRunning.store(false);  // Safely update atomic variable
}

void setAppUserModelID() {
    const wchar_t appID[] = L"com.example.notifier";
    HRESULT hr = SetCurrentProcessExplicitAppUserModelID(appID);

    if (SUCCEEDED(hr)) {
        std::cout << "App User Model ID set successfully." << std::endl;
    }
    else {
        std::cerr << "Failed to set App User Model ID. Error code: " << hr << std::endl;
    }
}

void startWebSocketServer() {
    WebSocketServer server;
    server.run();
}

int main() {
    // Set up signal handling early.
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Step 1: Initialize Notification Manager (Singleton)
    NotificationManager& manager = NotificationManager::getInstance();

    // Step 2: Create a shortcut
    HRESULT shortcutResult = CreateAppShortcut();
    if (FAILED(shortcutResult)) {
        std::cerr << "Warning: Failed to create shortcut! Notifications might not work properly. HRESULT: " << shortcutResult << std::endl;
    }

    // Step 3: Set AppUserModelID
    setAppUserModelID();

    // Step 4: Start the WebSocket server in a separate thread
    std::thread serverThread(startWebSocketServer);

    // Step 5: Simulate JSON forwarding
    std::cout << "Simulating JSON forwarding to the server..." << std::endl;
    nlohmann::json simulatedJson = {
        {"title", "Notification from Server"},
        {"message", "This notification was forwarded via WebSocket."},
        {"source", "server"},
        {"session", "session123"},
        {"expiry", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() + std::chrono::hours(1))}
    };

    // Mock sending JSON to server (this would be sent by a WebSocket client)
    std::cout << "Simulated JSON: " << simulatedJson.dump(4) << std::endl;

    // Step 6: Graceful shutdown
    std::cout << "\nProgram running. Press Ctrl + C to terminate.\n" << std::endl;

    while (keepRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    // Stop the server and join the thread
    serverThread.join();

    WindowsAPI::terminateSession();
    std::cout << "Program exited gracefully." << std::endl;

    return 0;
}
