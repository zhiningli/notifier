#ifndef TERMINAL_UI_H
#define TERMINAL_UI_H

#include <iostream>
#include <string>
#include <iomanip>
#include "notificationManager.h"

class TerminalUI {
public:
    static void displayIntro() {
        std::cout << std::string(80, '=') << "\n";
        std::cout << "              Thanks for using the Lightweight Notifier  \n";
        std::cout << "       Bridging Client Scripts with Windows Toast Notifications API\n";
        std::cout << std::string(80, '=') << "\n\n";

        std::cout << "Server is listening on port 9001\n";
        std::cout << "To start, open a WebSocket connection at: ws://localhost:9001\n";
        std::cout << "Press Ctrl + C twice to close all connections.\n";
    }

    static void refreshScreen() {
        std::cout << "\033[2J\033[H"; 

        std::cout << std::string(80, '=') << "\n";
        std::cout << "        Lightweight Notifier - Active Sessions & Notifications\n";
        std::cout << std::string(80, '=') << "\n";
        std::cout << "Server is running on port 9001\n";
        std::cout << "WebSocket Endpoint: ws://localhost:9001\n";
        std::cout << "Press Ctrl + C twice to exit the program.\n";
        std::cout << "-----------------------------------------\n";

        std::set<std::string> activeSessions = NotificationManager::getInstance().getActiveSessions();
        std::cout << "\n[ACTIVE SESSIONS]: ";
        if (activeSessions.empty()) {
            std::cout << "No active sessions.\n";
        }
        else {
            for (const auto& sessionID : activeSessions) {
                std::cout << "\n session " << sessionID << " ";
            }
            std::cout << "\n";
        }

        std::cout << "-----------------------------------------\n\n";

        auto activeNotifications = NotificationManager::getInstance().getActiveNotifications();
        if (activeNotifications.empty()) {
            std::cout << "[INFO] No active notifications.\n";
        }
        else {
            std::cout << "Active Notifications:\n";
            for (const auto& [id, notif] : activeNotifications) {
                const int totalWidth = 80;         
                const int labelWidth = 19;         
                const int contentWidth = totalWidth - labelWidth - 3;  

                std::cout << std::string(totalWidth, '-') << "\n";
                std::cout << "| Notification ID : " << std::setw(contentWidth) << std::right << id << " |\n";
                std::cout << "| Title           : " << std::setw(contentWidth) << std::right << notif.first << " |\n";
                std::cout << "| Message         : " << std::setw(contentWidth) << std::right << notif.second << " |\n";
                std::cout << std::string(totalWidth, '-') << "\n";
            }
        }
        std::cout << "\n";
    }
};

#endif // TERMINAL_UI_H
