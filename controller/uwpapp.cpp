#include "uwpapp.h"

#include <windows.h>
#include <Shobjidl.h>
#include <atlbase.h>
#include <propvarutil.h>

#include <QDebug>
#include <QFile>
#include <QDir>

UWPAppManager::UWPAppManager() {}

DEFINE_GUID(BHID_EnumItems, 0x94f60519, 0x2850, 0x4924, 0xaa, 0x5a, 0xd1, 0x5e, 0x84, 0x86, 0x80, 0x39);
DEFINE_GUID(BHID_PropertyStore, 0x0384e1a4, 0x1523, 0x439c, 0xa4, 0xc8, 0xab, 0x91, 0x10, 0x52, 0xf5, 0x86);

std::vector<UWP> UWPAppManager::getUWPApp()
{

    qDebug() << "UWPApp::getCatalog, function entry";

    std::vector<UWP> ret;

    CoInitialize(NULL);

    do {
        CComPtr<IShellItem> appFolder;
        if (FAILED(SHCreateItemFromParsingName(L"shell:AppsFolder",
                                               nullptr,
                                               IID_PPV_ARGS(&appFolder)))) {
            qWarning() << "UWPApp::getCatalog, fail to open shell:AppsFolder";
            break;
        }

        //qDebug() << "UWPApp::getCatalog, succeed to open shell::AppsFolder";

        CComPtr<IEnumShellItems> enumShellItems;
        if (FAILED(appFolder->BindToHandler(nullptr,
                                            BHID_EnumItems,
                                            IID_PPV_ARGS(&enumShellItems)))) {
            qWarning() << "UWPApp::getCatalog, fail to bind to handler";
            break;
        }

        //qDebug() << "UWPApp::getCatalog, succeed to bind to handler";

        PROPERTYKEY pkLauncherAppState;
        PSGetPropertyKeyFromName(L"System.Launcher.AppState", &pkLauncherAppState);
        PROPERTYKEY pkSmallLogoPath;
        PSGetPropertyKeyFromName(L"System.Tile.SmallLogoPath", &pkSmallLogoPath);
        PROPERTYKEY pkAppUserModelID;
        PSGetPropertyKeyFromName(L"System.AppUserModel.ID", &pkAppUserModelID);
        PROPERTYKEY pkInstallPath;
        PSGetPropertyKeyFromName(L"System.AppUserModel.PackageInstallPath", &pkInstallPath);

        const size_t pvslen = 512;
        CComHeapPtr<wchar_t> pvs;
        pvs.Allocate(pvslen);

        //qDebug() << "UWPApp::getCatalog, begin while loop";
        IShellItem* shellItemNext = nullptr;
        while (enumShellItems->Next(1, &shellItemNext, nullptr) == S_OK) {
            CComPtr<IShellItem> shellItem = shellItemNext;

            CComPtr<IPropertyStore> propertyStore;
            if (FAILED(shellItem->BindToHandler(NULL,
                                                BHID_PropertyStore,
                                                IID_PPV_ARGS(&propertyStore)))) {
                continue;
            }

            PROPVARIANT pv;
            PropVariantInit(&pv);

            // UWP app always has valid "Launcher.AppState"
            if (FAILED(propertyStore->GetValue(pkLauncherAppState, &pv))) {
                continue;
            }
            else {
                memset(pvs, 0, sizeof(wchar_t) * pvslen);
                PropVariantToString(pv, pvs, pvslen);
                if (std::wcslen(pvs) == 0) {
                    continue;
                }
            }

            QString shortName;
            QString fullPath;
            QString installPath;
            QString iconPath;

            CComHeapPtr<wchar_t> name;
            if (SUCCEEDED(shellItem->GetDisplayName(SIGDN_NORMALDISPLAY, &name))) {
                shortName = QString::fromWCharArray(name);
                //qDebug() << "name: " << shortName;
            }

            PropVariantClear(&pv);
            if (SUCCEEDED(propertyStore->GetValue(pkAppUserModelID, &pv))) {
                memset(pvs, 0, sizeof(wchar_t) * pvslen);
                PropVariantToString(pv, pvs, pvslen);
                fullPath = QString::fromWCharArray(static_cast<wchar_t*>(pvs));
                //qDebug() << " id: " << fullPath;
            }

            PropVariantClear(&pv);
            if (SUCCEEDED(propertyStore->GetValue(pkInstallPath, &pv))) {
                memset(pvs, 0, sizeof(wchar_t) * pvslen);
                PropVariantToString(pv, pvs, pvslen);
                installPath = QString::fromWCharArray(pvs);
                //qDebug() << " install: " << installPath;
            }

            PropVariantClear(&pv);
            if (SUCCEEDED(propertyStore->GetValue(pkSmallLogoPath, &pv))) {
                memset(pvs, 0, sizeof(wchar_t) * pvslen);
                PropVariantToString(pv, pvs, pvslen);
                iconPath = QString::fromWCharArray(pvs);
                //qDebug() << " logo: " << iconPath;
                iconPath = installPath + QDir::separator() + iconPath;
                iconPath = validateIconPath(iconPath);
                //qDebug() << " logo(validate): " << iconPath;
            }

            UWP uwp{shortName.toStdWString(), fullPath.toStdWString(), iconPath.toStdWString()};
            ret.push_back(uwp);

        }

        //qDebug() << "UWPApp::getCatalog, end while loop";

    } while (0);

    CoUninitialize();

    return ret;
}

void UWPAppManager::lunchUWPApp(const std::wstring appID)
{
    // Specify the appropriate COM threading model

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    IApplicationActivationManager* pAAM = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ApplicationActivationManager,
                                  nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&pAAM));
    if (FAILED(hr)) {
        qWarning() << "UWPApp::launchItem, fail to create CoCreateInstance, HR is" << hr;
        return;
    }

    DWORD pid = 0;
    hr = pAAM->ActivateApplication(appID.c_str(), L"", AO_NONE, &pid);
    if (FAILED(hr)) {
        qWarning() << "UWPApp::launchItem, Error in ActivateApplication call & HR is " << hr;
        return;
    }

    if (hr == 0) {
        qDebug() << "UWPApp::launchItem, Activated " << appID << " with pid " << pid;
    }

    CoUninitialize();
}

QString UWPAppManager::validateIconPath(const QString &iconPath)
{
    static const char* scales[] = {
        ".scale-200.",
        ".scale-100.",
        ".scale-300.",
        ".scale-400.",
        ".targetsize-48.",
        ".targetsize-16.",
        ".targetsize-24.",
        ".targetsize-256."
    };

    QString iconPathBase = iconPath.section('.', 0, -2);
    QString iconPathExt = iconPath.section('.', -1);

    for (std::size_t i = 0; i < sizeof(scales)/sizeof(scales[0]); ++i) {
        QString path = iconPathBase + scales[i] + iconPathExt;
        if (QFile::exists(path)) {
            return path;
        }
    }

    // icon not matched, list all files and find the shortest one
    QString iconDir = iconPath.section('\\', 0, -2);
    QString iconPrefix = iconPath.section('\\', -1).section('.', 0, -2);

    QDir dir(iconDir);
    QStringList listIconPath = dir.entryList(QStringList() << "*.png");

    QString result;
    for (const auto& path : listIconPath) {
        // find the shortest one with icon prefix
        if (path.startsWith(iconPrefix)
            && (result.isEmpty()
                || result.length() > path.length())) {
            result = path;
        }
    }

    if (!result.isEmpty()) {
        return iconDir + QDir::separator() + result;
    }

    return "";
}
