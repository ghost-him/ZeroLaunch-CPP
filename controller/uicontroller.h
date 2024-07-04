#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include <QJsonObject>
#include "../ui/settingwindow.h"
#include "windowhook.h"

class UIController
{
public:
    UIController();

    void initUI(const SettingWindowConfigure& configure);

    void show();
    void hide();

    void preLoadProgramIcon();

    void updateResultFrame(bool isEmptyText);

    void initProgramIcon();

    void clearIconCache();

    const QPixmap& getIcon(const std::wstring& iconPath, bool isUWPApp);

    ~UIController() = default;
private:

    const QPixmap& getIconWithEXE(const std::wstring& path);
    const QPixmap& getIconWithABPath(const std::wstring& path);

    int resultItemNumber;

    QString resultFrameEmptyText;

    std::unordered_map<std::wstring, QPixmap> iconCache;
};

#endif // UICONTROLLER_H
