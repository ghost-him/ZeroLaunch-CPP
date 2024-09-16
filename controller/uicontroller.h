#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include <QJsonObject>
#include "../ui/settingwindow.h"
#include "windowhook.h"

class QHotkey;

class UIController
{

public:
    UIController();

    void initUI(const SettingWindowConfigure& configure);

    void showSearchBar();
    void hideSearchBar();

    void preLoadProgramIcon();
    void updateResultFrame(bool isEmptyText);

    void initProgramIcon();

    void clearIconCache();

    void showSettingWindow();
    void hideSettingWindow();

    void refreshIndexedApp();

    const QPixmap& getIcon(const std::wstring& iconPath, bool isUWPApp);

    ~UIController() = default;
private:
    std::shared_ptr<QHotkey> hotkey;
    QPixmap addMarginToPixmap(const QPixmap &originalPixmap, int margin);


    const QPixmap& getIconWithEXE(const std::wstring& path);
    const QPixmap& getIconWithABPath(const std::wstring& path);

    int resultItemNumber;

    QString resultFrameEmptyText;

    std::unordered_map<std::wstring, QPixmap> iconCache;
};

#endif // UICONTROLLER_H
