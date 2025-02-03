#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include "notificationManager.h"
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <atomic>

struct UserData {
    std::string sessionID;
};

class WebSocketServer {
public:
    WebSocketServer(NotificationManager& manager);
    ~WebSocketServer();
    void run();
    void stop();
    void closeAllConnections();

private:
    int port;
    std::atomic<bool> keepRunning;
    NotificationManager& notificationManager;

    std::unordered_map<std::string, uWS::WebSocket<false, true, UserData>*> activeConnections;

    std::bitset<32> usedIDs;

    std::string generateSessionID();
    void freeSessionID(const std::string& sessionID);
    void handleMessage(std::string message, uWS::WebSocket<false, true, UserData>* ws);
};

#endif // WEBSOCKETSERVER_H
