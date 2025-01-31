#include "shortcut_util.h"
#include <windows.h>
#include <shlobj.h>      // For SHGetFolderPathW
#include <propsys.h>     // For InitPropVariantFromString
#include <propkey.h>     // For PKEY_AppUserModel_ID
#include <propvarutil.h>
#include <wrl/client.h>  // For ComPtr
#include <iostream>

using namespace Microsoft::WRL;  // For ComPtr

HRESULT CreateAppShortcut() {
    // Initialize COM
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM. HRESULT: " << hr << std::endl;
        return hr;
    }

    wchar_t shortcutPath[MAX_PATH];
    wchar_t exePath[MAX_PATH];

    // Get the user's Start Menu Programs folder
    hr = SHGetFolderPathW(NULL, CSIDL_PROGRAMS, NULL, 0, shortcutPath);
    if (FAILED(hr)) {
        std::cerr << "Failed to get Start Menu path. HRESULT: " << hr << std::endl;
        CoUninitialize();
        return hr;
    }

    // Append the shortcut filename
    wcscat_s(shortcutPath, MAX_PATH, L"\\NotifierApp.lnk");

    // Get the full path to the executable
    GetModuleFileNameW(NULL, exePath, MAX_PATH);

    // Create the shortcut
    ComPtr<IShellLinkW> shellLink;
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
    if (FAILED(hr)) {
        std::cerr << "Failed to create shell link. HRESULT: " << hr << std::endl;
        CoUninitialize();
        return hr;
    }

    shellLink->SetPath(exePath);
    shellLink->SetArguments(L"");

    // Set AppUserModelID
    ComPtr<IPropertyStore> propertyStore;
    hr = shellLink.As(&propertyStore);
    if (FAILED(hr)) {
        std::cerr << "Failed to get property store. HRESULT: " << hr << std::endl;
        CoUninitialize();
        return hr;
    }

    PROPVARIANT appIdPropVar;
    InitPropVariantFromString(L"com.example.notifier", &appIdPropVar);
    propertyStore->SetValue(PKEY_AppUserModel_ID, appIdPropVar);
    propertyStore->Commit();
    PropVariantClear(&appIdPropVar);

    // Save the shortcut
    ComPtr<IPersistFile> persistFile;
    hr = shellLink.As(&persistFile);
    if (FAILED(hr)) {
        std::cerr << "Failed to save shortcut file. HRESULT: " << hr << std::endl;
        CoUninitialize();
        return hr;
    }

    hr = persistFile->Save(shortcutPath, TRUE);
    if (SUCCEEDED(hr)) {
        std::wcout << L"Shortcut created successfully: " << shortcutPath << std::endl;
    }
    else {
        std::cerr << "Failed to save shortcut file. HRESULT: " << hr << std::endl;
    }

    CoUninitialize();
    return hr;
}

