#ifndef TERMINAL_UI_H
#define TERMINAL_UI_H

#include <iostream>
#include <string>
#include <iomanip>
#include "notificationManager.h"

class TerminalUI {
public:
    static void displayIntro() {
        std::cout << "=================================================================\n";
        std::cout << "   Thanks for using the Lightweight Notifier  \n";
        std::cout << "   Bridging Client Scripts with Windows Toast Notifications API\n";
        std::cout << "=================================================================\n\n";

        std::cout << "Server is listening on port 9001\n";
        std::cout << "To start, open a WebSocket connection at: ws://localhost:9001\n";
        std::cout << "Press Ctrl + C once to close all connections.\n";
        std::cout << "Press Ctrl + C again to exit the program.\n";
    }

    static void refreshScreen() {
        std::cout << "\033[2J\033[H"; 

        std::cout << "=========================================\n";
        std::cout << "   Lightweight Notifier - Active Sessions & Notifications\n";
        std::cout << "=========================================\n";
        std::cout << "Server is running on port 9001\n";
        std::cout << "WebSocket Endpoint: ws://localhost:9001\n";
        std::cout << "Press Ctrl + C once to close all connections.\n";
        std::cout << "Press Ctrl + C again to exit the program.\n";
        std::cout << "-----------------------------------------\n";

        std::set<std::string> activeSessions = NotificationManager::getInstance().getActiveSessions();
        std::cout << "\n[ACTIVE SESSIONS]: ";
        if (activeSessions.empty()) {
            std::cout << "No active sessions.\n";
        }
        else {
            for (const auto& sessionID : activeSessions) {
                std::cout << sessionID << " ";
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
                std::cout << "------------------------------------------------------------\n";
                std::cout << "| Notification ID: " << std::setw(10) << std::left << id << " |\n";
                std::cout << "| Title   : " << std::setw(30) << std::left << notif.first << " |\n";
                std::cout << "| Message : " << std::setw(30) << std::left << notif.second << " |\n";
                std::cout << "------------------------------------------------------------\n";
            }
        }
        std::cout << "\n";
    }
};

#endif // TERMINAL_UI_H
