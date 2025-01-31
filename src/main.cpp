#include "windows_api.h"
#include "shortcut_util.h" 
#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include <shobjidl.h> // For SetCurrentProcessExplicitAppUserModelID

void signalHandler(int signal) {
    std::cout << "Signal received: " << signal << ". Clearing up resources..." << std::endl;
    WindowsAPI::terminateSession();
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

int main() {
    // ✅ Step 1: Create a shortcut in Start Menu (required for notifications)
    HRESULT shortcutResult = CreateAppShortcut();
    if (FAILED(shortcutResult)) {
        std::cerr << "Failed to create shortcut! HRESULT: " << shortcutResult << std::endl;
    }

    // ✅ Step 2: Set AppUserModelID
    setAppUserModelID();

    // ✅ Step 3: Register signal handler
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // ✅ Step 4: Send Test Notification
    WindowsAPI::showNotification("This is a test notification", "Hello, how are you");

    std::cout << "\n\n\n\n\Program running. Press Ctrl + C to terminate";
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
