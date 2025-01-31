#include "windows_api.h"
#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#include <wrl.h>
#include <windows.ui.notifications.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.UI.Notifications.h>


void WindowsAPI::showNotification(const std::string& title, const std::string& message)
{
	using namespace winrt::Windows::UI::Notifications;
	using namespace winrt::Windows::Data::Xml::Dom;


	// Initialise COM
	std::cout << "Initializing COM..." << std::endl;
	winrt::init_apartment();
	std::cout << "COM initialized successfully." << std::endl;

	// Create XML for toast notification
	std::wstring toastXml =
		L"<toast>"
		L"<visual>"
		L"<binding template = 'ToastGeneric'>"
		L"<text>" + std::wstring(title.begin(), title.end()) + L"</text>"
		L"<text>" + std::wstring(message.begin(), message.end()) + L"</text>"
		L"</binding>"
		L"</visual>"
		L"</toast>";

	std::wcout << L"Toast XML: " << toastXml << std::endl;

	XmlDocument xmlDoc;
	xmlDoc.LoadXml(toastXml);
	std::cout << "Toast XML loaded into XmlDocument." << std::endl;
	// Create a toast notification

	ToastNotification toast(xmlDoc);
	std::cout << "ToastNotification created." << std::endl;


	// Show the toast notification
	std::cout << "About to show toast notification..." << std::endl;

	try {
		auto notifier = ToastNotificationManager::CreateToastNotifier();
		if (!notifier) {
			throw std::runtime_error("CreateToastNotifier returned a null notifier.");
		}
		notifier.Show(toast);
		std::cout << "Toast notification displayed successfully." << std::endl;
	}
	catch (const winrt::hresult_error& e) {
		std::wcerr << L"Error in Show: " << e.message().c_str() << std::endl;
		std::wcerr << L"HRESULT: " << std::hex << e.code() << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "Error in Show: " << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Unknown error in Show." << std::endl;
	}

}

void WindowsAPI::playSound(const std::string& soundFile) {
	PlaySound(
		soundFile.c_str(),
		NULL,
		SND_FILENAME | SND_ASYNC
	);
}

void WindowsAPI::cleanup() {
	winrt::uninit_apartment();
	std::cout << "WindowsAPI cleanup completed." << std::endl;
}

void WindowsAPI::terminateSession() {
	stopSound();
	cleanup();
	std::cout << "Session terminated gracefully." << std::endl;
	exit(0);
}

void WindowsAPI::stopSound() {
	PlaySound(NULL, NULL, 0);  // Stop sound playback
	std::cout << "Sound playback stopped." << std::endl;
}