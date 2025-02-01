#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include "notificationManager.h"
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <atomic>

struct UserData {
	std::string sessionID;
};;

class WebSocketServer {
public:
	WebSocketServer();
	void run();
	void stop();

private:
	int port;
	std::atomic<bool> keepRunning;
	void handleMessage(std::string message, uWS::WebSocket<false, true, UserData>* ws);
};

#endif // WEBSOCKETSERVER_H

