#include "windows_api.h"
#include <windows.h>
#include <mmsystem.h>
#include <wrl.h>
#include <windows.ui.notifications.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.UI.Notifications.h>

#pragma comment(lib, "winmm.lib")

void WindowsAPI::showNotification(const std::string &title, const std::string &message){
    using namespace winrt::Windows::UI::Notifications;
    using namespace winrt::Windows::Data::Xml::Dom;

    winrt::init_apartment();

    std::wstring toastXml = 
        L"<toast>"
        L"<visual>"
        L"<binding template='ToastGeneric'>"
        L"<text>" + 
}

void WindowsAPI::playSound(const std::string &soundFile){
    PlaySound(
        soundFile.c_str(),
        NULL,
        SND_FILENAME | SND_ASYNC
    );
}