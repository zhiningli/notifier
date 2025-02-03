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
            activeConnections[sessionID] = ws;
            notificationManager.addSession(sessionID);
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
			freeSessionID(userData->sessionID);
			activeConnections.erase(userData->sessionID);
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
        auto json = nlohmann::json::parse(message);
        std::string sessionID = json["sessionID"];
        std::string action = json["action"];
        auto* userData = ws->getUserData();

        // Verify sessionID matches the WebSocket's sessionID
        if (userData->sessionID != sessionID) {
            throw std::runtime_error("Invalid sessionID");
        }

        // Dispatch table for actions
        static const std::unordered_map<std::string, std::function<void(const nlohmann::json&, uWS::WebSocket<false, true, UserData>*)>> actionHandlers = {
            {"create", [this, sessionID](const nlohmann::json& payload, uWS::WebSocket<false, true, UserData>* ws) {
                notificationManager.createNotification(sessionID, payload);
                nlohmann::json response = {{"status", "success"}, {"action", "create"}};
                ws->send(response.dump(), uWS::OpCode::TEXT);
            }},
            {"update", [this, sessionID](const nlohmann::json& payload, uWS::WebSocket<false, true, UserData>* ws) {
                std::string notificationID = payload["notificationID"];
                notificationManager.updateNotification(sessionID, notificationID, payload);
                nlohmann::json response = {{"status", "success"}, {"action", "update"}};
                ws->send(response.dump(), uWS::OpCode::TEXT);
            }},
            {"delete", [this, sessionID](const nlohmann::json& payload, uWS::WebSocket<false, true, UserData>* ws) {
                std::string notificationID = payload["notificationID"];
                notificationManager.removeNotification(sessionID, notificationID);
                nlohmann::json response = {{"status", "success"}, {"action", "delete"}};
                ws->send(response.dump(), uWS::OpCode::TEXT);
            }},
            {"display", [this, sessionID](const nlohmann::json& payload, uWS::WebSocket<false, true, UserData>* ws) {
                std::string notificationID = payload["notificationID"];
                notificationManager.displayNotification(sessionID, notificationID);

                nlohmann::json response = {{"status", "success"}, {"action", "display"}};
                ws->send(response.dump(), uWS::OpCode::TEXT);
            }},
            {"displayAll", [this, sessionID](const nlohmann::json&, uWS::WebSocket<false, true, UserData>* ws) {
                notificationManager.displayAllNotifications(sessionID);

                nlohmann::json response = {{"status", "success"}, {"action", "displayAll"}};
                ws->send(response.dump(), uWS::OpCode::TEXT);
            }}
        };

        auto it = actionHandlers.find(action);
        if (it != actionHandlers.end()) {
            it->second(json["payload"], ws);
        }
        else {
            throw std::runtime_error("Invalid action: " + action);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error handling message: " << e.what() << std::endl;

        nlohmann::json errorResponse = { {"status", "error"}, {"message", e.what()} };
        ws->send(errorResponse.dump(), uWS::OpCode::TEXT);
    }
}

