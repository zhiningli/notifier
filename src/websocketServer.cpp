#include "websocketServer.h"
#include <uwebsockets/App.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <atomic>
#include <csignal>
#include <thread>

WebSocketServer::WebSocketServer(NotificationManager& manager)
    : port(9001), notificationManager(manager), keepRunning(true) {
    std::cout << "WebSocketServer initialized on port " << port << std::endl;
}

WebSocketServer::~WebSocketServer() {
    stop();
    std::cout << "WebSocketServer destroyed." << std::endl;
}

void WebSocketServer::run() {
    uWS::App().ws<UserData>("/*", {
        .idleTimeout = 16,
        .open = [this](auto* ws) {
            (void)ws; // Suppress unused parameter warning
            {
                std::unique_lock lock(connectionsMutex);
                activeConnections.insert(ws);
            }
            std::cout << "WebSocket connection opened!" << std::endl;
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
                (void)ws; // Suppress unused parameter warning
                {
                    std::unique_lock lock(connectionsMutex);
					activeConnections.erase(ws);
                }
                std::cout << "WebSocket connection closed with code " << code
                          << ". Message: " << message << std::endl;
            }
        }).listen(port, [this](auto* token) {
            if (token) {
                std::cout << "Server is listening on port " << port << std::endl;
            }
            else {
                std::cerr << "Failed to listen on port " << port << ". Exiting..." << std::endl;
                std::exit(EXIT_FAILURE);
            }
            }).run();

    while (keepRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "WebSocket server stopped gracefully." << std::endl;
}

void WebSocketServer::stop() {
    keepRunning = false;
    closeAllConnections();
	std::cout << "Websocket server stopped." << std::endl;
}


void WebSocketServer::closeAllConnections() {
    std::shared_lock<std::shared_mutex> lock{ connectionsMutex };
    for (auto* ws : activeConnections) {
        ws->close();
    }
}

void WebSocketServer::handleMessage(std::string message, uWS::WebSocket<false, true, UserData>* ws) {
    try {
        auto json = nlohmann::json::parse(message);

        std::string title = json["title"];
        std::string msg = json["message"];
        std::string source = json["source"];
        std::string session = json["session"];
        auto expiryTime = std::chrono::system_clock::from_time_t(json["expiry"]);

        Notification notification(title, msg, source, session, Notification::SourceEnum::Cpp, expiryTime);
        notificationManager.addNotification(notification);
        std::cout << "Notification added by WebSocketServer: " << title << std::endl;
		notificationManager.displayAllNotifications();
        nlohmann::json response = { {"status", "success"}, {"message", "Notification created"} };
        ws->send(response.dump(), uWS::OpCode::TEXT);
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "Error parsing or processing JSON: " << e.what() << std::endl;

        nlohmann::json errorResponse = { {"status", "error"}, {"message", "Invalid JSON format"} };
        ws->send(errorResponse.dump(), uWS::OpCode::TEXT);
    }
}
