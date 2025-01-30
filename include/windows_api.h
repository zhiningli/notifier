// This file provides an interface for Window's specific functionality
// This file includes a method for
//    1. Display a notification as a toast notification
//    2. Play a sound using Window's Multimedia API

#ifndef WINDOWS_API_H
#define WINDOWS_API_H

#include<string>

class WindowsAPI {
    public:
        static void showNotification(const std::string &title, const std::string &message);
        static void playSound(const std::string &soundFile);  
    private:
};


#endif // WINDOWS_API_H


