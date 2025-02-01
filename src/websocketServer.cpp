#include "websocketServer.h"
#include <uwebsockets/App.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <atomic>
#include <csignal>
#include <thread>

WebSocketServer::WebSocketServer() : port(9001), keepRunning(true) {
    std::cout << "WebSocketServer initialized on port " << port << std::endl;
}

void WebSocketServer::run() {
    uWS::App().ws<UserData>("/*", {
        .idleTimeout = 16,
        .open = [](auto* ws) {
            UserData* userData = ws->getUserData();
            userData->sessionID = "default-session"; // Initialize session ID
            std::cout << "WebSocket connection opened!" << std::endl;
        },
        .message = [this](auto* ws, std::string_view message, uWS::OpCode opCode) {
            if (opCode == uWS::OpCode::TEXT) {
                this->handleMessage(std::string(message), ws);
            }
 else if (opCode == uWS::OpCode::BINARY) {
  std::cout << "Binary message received!" << std::endl;
}
},
.close = [](auto* /*ws*/, int code, std::string_view message) {
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

    while (keepRunning.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "WebSocket server stopped gracefully." << std::endl;
}

void WebSocketServer::stop() {
    keepRunning.store(false);
}

void WebSocketServer::handleMessage(std::string message, uWS::WebSocket<false, true, UserData>* ws) {
    UserData* userData = ws->getUserData();
    if (!userData) {
        std::cerr << "Error: No user data associated with the WebSocket connection." << std::endl;
        return;
    }

    std::cout << "Handling message for session " << userData->sessionID << ": " << message << std::endl;

    try {
        auto json = nlohmann::json::parse(message);
        nlohmann::json response = {
            {"status", "success"},
            {"message", "Acknowledged"}
        };
        ws->send(response.dump(), uWS::OpCode::TEXT);
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Failed to parse message: " << e.what() << std::endl;
    }
}
