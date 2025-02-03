# **Lightweight Notifier**
### ⚡ A minimal and efficient notification system for Windows 11+.

🚧 *Still under development!* This project is functional on Windows 11+, and I am currently testing cross-platform compatibility while improving support for multiple clients. If you are interested in using it, feel free to try it out or raise any issues—I'll do my best to address them! 🚀

---

## **Introduction**
Lightweight Notifier is a simple and efficient tool designed to bridge client scripts with the Windows Toast Notifications API. It allows you to send and manage notifications through a WebSocket endpoint.

💡 **Use Cases**
- Get instant alerts for **concert ticket availability** 🎟️
- Be notified when **stock prices drop** 📉
- Stay focused and get reminders when **you open YouTube while working** 🚀
- Connect any script to trigger notifications based on real-time events.

You can write client scripts based on your interests, send an update to the notifier, and a toast notification will pop up on your screen.


![Notifier Example](assets/notifier-creation-example.png)


## **How to Use**
### **Prerequisites**
- Ensure you have **CMake** installed with **Ninja generator support**.
- Make sure your environment supports **C++20**.

### **Building the Project**
You can directly use the pre-built `notifier.exe` file.

However, if you want to **customize functionality**, you can build the project using CMake:

```sh
mkdir build
cd build
cmake .. -G Ninja
ninja
```

I have deliberately **kept the features minimal** to ensure the application remains **lightweight** and efficient. However, you can extend it as needed.

---

## **Creating a Connection**
To establish a connection, use a **WebSocket client** to connect to the server:

```javascript
ws://localhost:9001
```

---

## **Sending Notifications**
Once connected, you can send notifications to the notifier. The server expects notifications in the following format:

```json
{
    "sessionID": "0",
    "action": "create",
    "payload": {
        "title": "You opened YouTube again!",
        "message": "You should do your 4YP instead of writing this."
    }
}
```

📜 For a complete list of supported formats and commands, check the [`assets/how-to.md`](assets/how-to.md) file.

---

## **Common Questions**
The Windows Toast Notification requires a start-menu shortcut. This is the reason why there is a `shortcut_util` class. This is weird but a necessary step. Relevant information can be found at:
[Windows Toast Notification Documentation](https://learn.microsoft.com/en-us/previous-versions/windows/apps/hh779727(v=win.10))

## **License**
This project is licensed under the **MIT License**.

---




