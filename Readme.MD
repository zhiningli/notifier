# Lightweight Notifier

## Still in development !!!

## Introduction
Lightweight Notifier is a simple and efficient tool designed to bridge client scripts with the Windows Toast Notifications API. It allows you to send and manage notifications through a WebSocket connection.

## How to Use

### Prerequisites
- Ensure you have CMake installed with Ninja generator support.
- Make sure your environment supports C++20.

### Building the Project
1. Clone the repository:

```bash
git clone <repository-url>
cd <repository-directory>
```

2. Create a build directory and navigate into it:
```bash   
mkdir build
cd build
```

3. Run CMake to generate the build files:
```bash
cmake -G "Ninja" ..
```

4. Build the project:
```bash
ninja
```

### Running the Server
1. Navigate to the build directory if you are not already there.
2. Run the executable:
```bash
./notifier
```

You should see the following output:
=================================================================
   Thanks for using the Lightweight Notifer  
Bridging Client Scripts with Windows Toast Notifications API
Server is listening on port 9001
To start use it, raise a WebSocket connection to: ws://localhost:9001
To stop the program:
Press Ctrl + C once to close all connections.
Press Ctrl + C again to exit the program.
=================================================================

### Creating a Connection
To create a connection, use a WebSocket client to connect to the server:
```javascript
ws://localhost:9001
```	

### Storing Notifications
Once connected, you can send notifications to the server. The server expects notifications in a specific format. Here is an example of how to send a notification:
```json
{
    "title": "Sample Notification",
    "message": "This is a test notification",
    "sourceID": "source123",
    "sessionID": "session123",
    "source": "TestSource",
    "expiryTime": "2023-12-31T23:59:59Z"
}
```

### Still in development
## Acknowledgements
- The server will acknowledge connections and display notifications using the `TerminalUI` class.
- Notifications are managed by the `NotificationManager` class.

## License
This project is licensed under the MIT License.

Thank you for using Lightweight Not


