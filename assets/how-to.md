## **Introduction**

This notifier is built to handle connection from multiple sources, you can use your python or javascript to call the notifier.

### API for calling

**Establish connection**

Upon connection, the notifier will allocate you a sessionID, you can either found it in the return json or on the terminal UI
``` reponse
{
    "status": "success",
    "sessionID": [sessionID]
}
{"status", "success"}, {"sessionID", sessionID}, {"action", "session_assigned"}

```

**Creating a notification**

```javascript
{
    "sessionID": "0",
    "action": "create",
    "payload": {
        "title": "You opened YouTube again!",
        "message": "You should do your 4YP instead of writing this."
    }
}
```

If successful, you shall see both a windows toast notification as well as it displaying in the terminal UI Notifer

``` response 
{
    "status": "success",
    "sessionID": sessionID,
    "payload": {
        "action": "create",
        "notificationID": notificationID
    }
}
```


** Updating a notification**
```javascript
{
    "action": "update",
    "sessionID": [sessionID],
    "payload": {
        "notificationID": notification_id,
        "message": new_message,
        "timestamp": int(time.time())
    }
}

```

** Deleting a notification **
```javascript
{
    "action": "delete",
    "sessionID": notif_data["session_id"],
    "payload": {
        "notificationID": notification_id
    }
}
```

** Display and display all notification
```javascript
{
    "action": "display",
    "sessionID": sessionID
}
```

** Ping

Websocket by default terminates after 960 seconds of idle time. So ping the notifer occasionally to keep the connection open

```javascript
{"action": "ping", "sessionID": sessionID}
```

Notifier will pong you back.