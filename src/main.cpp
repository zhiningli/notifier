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
    // Step 1: Create a shortcut
    HRESULT shortcutResult = CreateAppShortcut();
    if (FAILED(shortcutResult)) {
        std::cerr << "Failed to create shortcut! HRESULT: " << shortcutResult << std::endl;
        return shortcutResult;
    }

    // Step 2: Set AppUserModelID
    setAppUserModelID();

    // Step 3: Send a test notification
    WindowsAPI::showNotification("Test Notification", "This is a test notification.");

    // Step 4: Keep the program running
    std::cout << "Program running. Press Ctrl + C to terminate." << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
