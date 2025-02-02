#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include "notificationManager.h"
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <unordered_set>
#include <atomic>
#include <shared_mutex>

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

    std::unordered_set<uWS::WebSocket<false, true, UserData>*> activeConnections;
    std::shared_mutex connectionsMutex;

    void handleMessage(std::string message, uWS::WebSocket<false, true, UserData>* ws);
};

#endif // WEBSOCKETSERVER_H

