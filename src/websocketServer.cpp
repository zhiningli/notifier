#include "websocketServer.h"
#include <uwebsockets/App.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <thread>

WebSocketServer::WebSocketServer(NotificationManager& manager)
    : port(9001), notificationManager(manager), keepRunning(true) {
}

WebSocketServer::~WebSocketServer() {
    stop();
    std::cout << "WebSocketServer destroyed." << std::endl;
}

void WebSocketServer::run() {
    uWS::App()
        .ws<UserData>("/*", {
            .idleTimeout = 960,
            .open = [this](uWS::WebSocket<false, true, UserData>* ws) {
                handleConnectionOpen(ws);
            },
            .message = [this](uWS::WebSocket<false, true, UserData>* ws, std::string_view message, uWS::OpCode opCode) {
                if (opCode == uWS::OpCode::TEXT) {
                    handleMessage(std::string(message), ws);
                }
                 else if (opCode == uWS::OpCode::BINARY) {
                  std::cout << "[INFO] Binary message received. Ignored." << std::endl;
                }
                },
            .close = [this](uWS::WebSocket<false, true, UserData>* ws, int code, std::string_view message) {
                handleConnectionClose(ws, code, message);
            }
        })
        .listen(port, [this](auto* token) {
        if (token) {
            std::cout << "Server listening on port " << port << std::endl;
        }
        else {
            throw std::runtime_error("Failed to listen on port " + std::to_string(port));
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
    for (auto& [sessionID, ws] : activeConnections) {
        if (ws) {
            ws->close();
        }
    }
    activeConnections.clear();
    std::cout << "All WebSocket connections closed." << std::endl;
}

std::string WebSocketServer::generateSessionID() {
    for (size_t i = 0; i < usedIDs.size(); ++i) {
        if (!usedIDs.test(i)) {
            usedIDs.set(i);
            return std::to_string(i);
        }
    }
    throw std::runtime_error("No available session IDs. Maximum limit of 32 reached.");
}

void WebSocketServer::freeSessionID(const std::string& sessionID) {
    try {
        int id = std::stoi(sessionID);
        if (id >= 0 && id < 32) {
            usedIDs.reset(id);
        }
        else {
            std::cerr << "[WARNING] Invalid session ID: " << sessionID << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception in freeSessionID: " << e.what() << std::endl;
    }
}

void WebSocketServer::handleConnectionOpen(uWS::WebSocket<false, true, UserData>* ws) {
    auto* userData = ws->getUserData();
    std::string sessionID = generateSessionID();
    userData->sessionID = sessionID;

    activeConnections[sessionID] = ws;
    notificationManager.addSession(sessionID);

    nlohmann::json response = { {"status", "success"}, {"sessionID", sessionID}, {"action", "session_assigned"}};
    ws->send(response.dump(), uWS::OpCode::TEXT);
}

void WebSocketServer::handleConnectionClose(uWS::WebSocket<false, true, UserData>* ws, int code, std::string_view message) {
    auto* userData = ws->getUserData();
    std::string sessionID = userData->sessionID;

    if (code != 1000) {
        std::cerr << "[Warning] Unexpected Websocket disconnection. Code: " << code
            << " Message: " << message << " SessionID: " << sessionID << std::endl;
    }

    activeConnections.erase(sessionID);
    freeSessionID(sessionID);
    notificationManager.removeSession(sessionID);
    

    std::cout << "[INFO] Connection closed. Session ID: " << sessionID << " Code: "<< code << " Message: " << message << std::endl;
}

void WebSocketServer::handleMessage(const std::string& message, uWS::WebSocket<false, true, UserData>* ws) {
    try {
        auto json = nlohmann::json::parse(message);

        if (!json.contains("sessionID") || !json.contains("action")) {
            throw std::runtime_error("[Error] Invalid message format: Missing sessionID or action");
        }

        std::string sessionID = json["sessionID"];
        std::string action = json["action"];
        auto* userData = ws->getUserData();

        if (userData->sessionID != sessionID) {
            throw std::runtime_error("Unauthorized session ID");
        }

        static const std::unordered_map<std::string, std::function<void(const nlohmann::json&)>> actionHandlers = {
            {"create", [this, sessionID, ws](const nlohmann::json& payload) {
                std::string notificationID = notificationManager.createNotification(sessionID, payload);
                nlohmann::json response = { {"status", "success"}, {"sessionID", sessionID}, {"payload", {{"action", "create"}, {"notificationID", notificationID}}} };
                ws->send(response.dump(), uWS::OpCode::TEXT);
            }},
            {"update", [this, sessionID, ws](const nlohmann::json& payload) {
                notificationManager.updateNotification(sessionID, payload["notificationID"], payload);
                nlohmann::json response = { {"status", "success"}, {"sessionID", sessionID}, {"payload", {{"action", "update"}, {"notificationID", payload["notificationID"]}}} };
                ws->send(response.dump(), uWS::OpCode::TEXT);
            }},
            {"delete", [this, sessionID, ws](const nlohmann::json& payload) {
                notificationManager.removeNotification(sessionID, payload["notificationID"]);
                nlohmann::json response = { {"status", "success"}, {"sessionID", sessionID}, {"payload", {{"action", "delete"}, {"notificationID", payload["notificationID"]}} } };
                ws->send(response.dump(), uWS::OpCode::TEXT);
            }},
            {"display", [this, sessionID, ws](const nlohmann::json& payload) {
                notificationManager.displayNotification(sessionID, payload["notificationID"]);
                nlohmann::json response = { {"status", "success"}, {"sessionID", sessionID}, {"payload", {{"action", "display"}, {"notificationID", payload["notificationID"]}}} };
                ws->send(response.dump(), uWS::OpCode::TEXT);
            }},
            {"displayAll", [this, sessionID, ws](const nlohmann::json&) {
                notificationManager.displayAllNotifications(sessionID);
                nlohmann::json response = { {"status", "success"}, {"sessionID", sessionID}, {"payload", {"action", "displayAll"}} };
                ws->send(response.dump(), uWS::OpCode::TEXT);
            }},
            {"ping", [this, sessionID, ws](const nlohmann::json& payload) {
                nlohmann::json response = {{"status", "success"}, {"sessionID", sessionID}, {"payload", {"action", "pong"}}};
                ws->send(response.dump(), uWS::OpCode::TEXT);
            }},
        };

        auto it = actionHandlers.find(action);
        if (it != actionHandlers.end()) {
            it->second(json["payload"]);
        }
        else {
            throw std::runtime_error("Unknown action: " + action);
        }

    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception in handleMessage: " << e.what() << std::endl;
        nlohmann::json errorResponse = { {"status", "error"}, {"message", e.what()} };
        ws->send(errorResponse.dump(), uWS::OpCode::TEXT);
    }
}
