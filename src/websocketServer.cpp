#include "websocketServer.h"
#include "terminalUI.h"
#include <uwebsockets/App.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <atomic>
#include <csignal>
#include <thread>

WebSocketServer::WebSocketServer(NotificationManager& manager)
    : port(9001), notificationManager(manager), keepRunning(true) {
}

WebSocketServer::~WebSocketServer() {
    stop();
    std::cout << "WebSocketServer destroyed." << std::endl;
}

void WebSocketServer::run() {
    uWS::App().ws<UserData>("/*", {
        .idleTimeout = 16,
        .open = [this](auto* ws) {
            auto userData = ws->getUserData();
            std::string sessionID = generateSessionID();
            userData->sessionID = sessionID;

            {
                std::unique_lock lock(connectionsMutex);
                activeConnections[sessionID] = ws;
            }
            TerminalUI::displayAcknowledgement(sessionID);
            nlohmann::json response = { {"status", "success"}, {"sessionID", sessionID} };
            ws->send(response.dump(), uWS::OpCode::TEXT);
        },
        .message = [this](auto* ws, std::string_view message, uWS::OpCode opCode) {
            if (opCode == uWS::OpCode::TEXT) {
                std::cout << "Text message received: " << message << std::endl;
                this->handleMessage(std::string(message), ws);
            }
             else if (opCode == uWS::OpCode::BINARY) {
              std::cout << "Binary message received!" << std::endl;
            }
        },
        .close = [this](auto* ws, int code, std::string_view message) {
			UserData* userData = ws->getUserData();
            {
                std::unique_lock lock(connectionsMutex);
				freeSessionID(userData->sessionID);
				activeConnections.erase(userData->sessionID);
            }
			TerminalUI::displayTerminationAcknowledgement(
                userData->sessionID, 
                code, 
                message);
        }
        })
    .listen(port, [this](auto* token) {
        if (token) {
            std::cout << "Server listening on port " << port << std::endl;
        }
        else {
            std::cerr << "Failed to listen on port " << port << ". Exiting..." << std::endl;
            std::exit(EXIT_FAILURE);
        }
            })
    .run();

    while (keepRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "WebSocket server stopped gracefully." << std::endl;
}

void WebSocketServer::stop() {
    keepRunning = false;
    closeAllConnections();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "Websocket server stopped." << std::endl;
}


void WebSocketServer::closeAllConnections() {
    std::unique_lock<std::shared_mutex> lock{ connectionsMutex };


    for (auto& [sessionID, ws] : activeConnections) {
        if (ws) {
            std::cout << "[INFO] Closing WebSocket session: " << sessionID << std::endl;
            ws->close();
			freeSessionID(sessionID);
        }
    }

    activeConnections.clear();
    std::cout << "[INFO] All WebSocket connections closed." << std::endl;
}



std::string WebSocketServer::generateSessionID() {
    std::unique_lock lock(idMutex);
    for (size_t i = 0; i < usedIDs.size(); ++i) {
        if (!usedIDs.test(i)) {
            usedIDs.set(i);
            return std::to_string(i);
        }
    }
    
	throw std::runtime_error("No available session IDs. Currently limited to 32. ");
    return "";
}

void WebSocketServer::freeSessionID(const std::string& sessionID) {
    std::unique_lock lock(idMutex);
    try {
        int id = std::stoi(sessionID);
        if (id >= 0 && id < 32 && usedIDs.test(id)) { 
            usedIDs.reset(id);
        }
        else {
            std::cerr << "Invalid sessionID: " << sessionID << " (Out of range)" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error in freeSessionID: " << e.what() << std::endl;
    }
}



void WebSocketServer::handleMessage(std::string message, uWS::WebSocket<false, true, UserData>* ws) {
    try {
        std::shared_lock lock(connectionsMutex);
        auto json = nlohmann::json::parse(message);

        UserData* userData = ws->getUserData();

        std::string title = json["title"];
        std::string msg = json["message"];
        std::string source = json["source"];
		std::string sessionID = json["sessionID"];
        auto expiryTime = std::chrono::system_clock::from_time_t(json["expiry"]);

        Notification notification(title, msg, source, sessionID, Notification::SourceEnum::Cpp, expiryTime);
        notificationManager.addNotification(notification);
        std::cout << "Notification added by WebSocketServer: " << title << " for sessionID: " << sessionID << std::endl;

        notificationManager.displayAllNotifications();

        nlohmann::json response = {
            {"status", "success"},
            {"message", "Notification created"},
            {"sessionID", sessionID}
        };
        ws->send(response.dump(), uWS::OpCode::TEXT);
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "Error parsing or processing JSON: " << e.what() << std::endl;

        nlohmann::json errorResponse = { 
                                        {"status", "error"}, 
                                        {"message", "Invalid JSON format"} };
        ws->send(errorResponse.dump(), uWS::OpCode::TEXT);
    }
}

