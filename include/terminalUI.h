#ifndef TERMINAL_UI_H
#define TERMINAL_UI_H

#include <iostream>
#include <string>
#include <iomanip>

class TerminalUI {
public:
	static void displayIntro() {
        std::cout << "=================================================================\n";
        std::cout << "   Thanks for using the Lightweight Notifer  \n";
        std::cout << "   Bridging Client Scripts with Windows Toast Notifications API\n ";
        std::cout << "=================================================================\n\n";

        std::cout << "Server is listening on port 9001\n";
        std::cout << "To start use it, raise a WebSocket connection to: ws://localhost:9001\n";
        std::cout << "\n\n To stop the programm";
        std::cout << "Press Ctrl + C once to close all connections.\n";
        std::cout << "Press Ctrl + C again to exit the program.\n";
	}

	static void displayAcknowledgement(const std::string& sessionID, std::string& clientID,  std::string& clientSource) {
        std::cout << "[ACK] Connection (sessionID: " << sessionID << " ) established \n";
        std::cout << "client ID: " << clientID << "source: " << clientSource<< "\n";
	}

    static void displayNotification(const std::string& title, const std::string& message, const std::string& source) {
        std::cout << "-----------------------------------------\n";
        std::cout << "| Title   : " << std::setw(30) << std::left << title << "|\n";
        std::cout << "| Message : " << std::setw(30) << std::left << message << "|\n";
        std::cout << "| Source  : " << std::setw(30) << std::left << source << "|\n";
        std::cout << "-----------------------------------------\n";
    }

    static void displayError(const std::string& errorMessage) {
        std::cerr << "[ERROR] " << errorMessage << "\n";
    }

    static void displayPersistentMessage() {
        // ANSI escape codes to move the cursor and clear the line
        std::cout << "\033[2J";          // Clear the entire screen
        std::cout << "\033[H";           // Move cursor to the top-left corner

        std::cout << "=========================================\n";
        std::cout << "   Program running: Lightweight Notifier \n";
        std::cout << "=========================================\n";
        std::cout << "Server is listening on port 9001\n";
        std::cout << "To use it, raise a WebSocket connection to:\n";
        std::cout << "    ws://localhost:9001\n";
        std::cout << "Press Ctrl + C once to close all connections.\n";
        std::cout << "Press Ctrl + C again to exit the program.\n";
        std::cout << "-----------------------------------------\n\n";
    }

    static void refreshScreen() {
        displayPersistentMessage();
    }
};

#endif // TERMINAL_UI_H