#include "windows_api.h"
#include "shortcut_util.h"
#include "notification.h"
#include "notificationManager.h"
#include <iostream>
#include <csignal>
#include <thread>
#include <shobjidl.h>

#include <mutex>

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

int main() {
    // Set up signal handling early.
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    //Step 1: Initialize Notification Manager (Singleton)
    NotificationManager& manager = NotificationManager::getInstance();
    //Step 2: Create a shortcut
    HRESULT shortcutResult = CreateAppShortcut();
    if (FAILED(shortcutResult)) {
        std::cerr << "Warning: Failed to create shortcut! Notifications might not work properly. HRESULT: " << shortcutResult << std::endl;
    }

    //Step 3: Set AppUserModelID
    setAppUserModelID();

    //Step 4: Create and Add Test Notifications
    auto expiryTime = std::chrono::system_clock::now() + std::chrono::hours(1);
    Notification notification1("Notification 1", "This is the first notification", "source1", "session1", Notification::SourceEnum::Cpp, expiryTime);
    std::cout << "Notification 1 is created " << std::endl;
    std::cout << "Notification 1 is expired? " << notification1.isExpired() << std::endl;
    auto anotherExpiryTime = std::chrono::system_clock::now() + std::chrono::hours(1);
    Notification notification2("Notification 2", "This is the second notification", "source2", "session2", Notification::SourceEnum::Cpp, anotherExpiryTime);
    std::cout << "Notification 2 is created " << std::endl;
    std::cout << "Notification 2 is expired? " << notification2.isExpired() << std::endl;
    manager.addNotification(notification1);
    std::cout << "Adding notificaton 1 to the manager" << std::endl;
    manager.addNotification(notification2);
    std::cout << "Notifications added to the manager" << std::endl;
    //Step 5: Display Latest Notification
    manager.displayAllNotifications();
	std::cout << "Displaying all notifications" << std::endl;

    //Step 6: Handle Graceful Exit

    std::cout << "\nProgram running. Press Ctrl + C to terminate.\n" << std::endl;

    // Instead of an infinite loop, we check for keepRunning flag
    while (keepRunning) {
          std::this_thread::sleep_for(std::chrono::seconds(10));
    }

  // Now in the main thread, perform cleanup:
    WindowsAPI::terminateSession();
    std::cout << "Program exited gracefully." << std::endl;

    return 0;
}
