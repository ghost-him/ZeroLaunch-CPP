#include "uiutils.h"
#include "../controller/windowhook.h"
#include "singleapplication.h"
#include "settingwindow.h"
#include "../controller/controller.h"
#include "../controller/uicontroller.h"
#include "searchbar.h"
#include <QMessageBox>
#include "singleapplication.h"

QAction* createExitAction(QObject *parent)
{
    QAction *exitAction = new QAction("退出程序", parent);
    exitAction->setIcon(QIcon(":/icon/exit.svg"));
    QObject::connect(exitAction, &QAction::triggered, [&](){
        WindowHook::getInstance().stop();
        qApp->quit();
    });
    return exitAction;
}

QAction *createOpenSettingAction(QObject *parent)
{
    QAction *openSettingAction = new QAction("打开设置", parent);
    openSettingAction->setIcon(QIcon(":/icon/setting.svg"));
    QObject::connect(openSettingAction, &QAction::triggered, [](){
        Controller::getInstance().showSettingWindow();
    });
    return openSettingAction;
}


// 创建刷新数据库的 QAction
QAction* createReloadSettingAction(QObject *parent)
{
    QAction *reloadSettingAction = new QAction("刷新数据库", parent);
    reloadSettingAction->setIcon(QIcon(":/icon/refresh.svg"));
    QObject::connect(reloadSettingAction, &QAction::triggered, [](){
        Controller& controller = Controller::getInstance();
        controller.init();
    });
    return reloadSettingAction;
}

// 创建打开帮助页面的 QAction
QAction* createShowHelpAction(QObject *parent)
{
    QAction *showHelp = new QAction("打开帮助页面", parent);
    showHelp->setIcon(QIcon(":/icon/help.svg"));
    QObject::connect(showHelp, &QAction::triggered, [](){
        SearchBar& bar = SearchBar::getInstance();
        QMessageBox::information(&bar, "使用方法",
                                 R"(
Alt + space：打开搜索栏
Enter：选择要启动的应用。
直接点击搜索结果栏中显示的程序也可以启动程序。
方向键上：选择上一个选项
方向键下：选择下一个选择
Ctrl + j：等于方向键下
Ctrl + k：等于方向键上
ESC：当搜索栏中有文字时，则清屏；没有文字时，则隐藏搜索栏。
)");
    });
    return showHelp;
}

QPalette* Color::palette = nullptr;

bool Color::isDarkMode()
{
    if (!palette) {
        palette = new QPalette(SingleApplication::palette());
    }


    static std::optional<bool> ret;
    if (ret.has_value()) {
        return ret.value();
    }
#ifdef Q_OS_WIN
    HKEY hKey;
    DWORD type;
    DWORD value;
    DWORD size = sizeof(value);

    // 打开注册表项
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        // 读取 AppsUseLightTheme 值
        if (RegQueryValueEx(hKey, TEXT("AppsUseLightTheme"), NULL, &type, reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS)
        {
            ret = static_cast<bool>(value == 0);
        }
        RegCloseKey(hKey);
    }
#endif
    return ret.value_or(false); // 如果无法确定，默认返回false（非深色模式）
}

QString Color::textColor()
{
    return isDarkMode() ? "#E0E0E0" : "black";
}

QString Color::selectedBackgroundColor()
{
    return  isDarkMode() ? "#3F3F46" : palette->color(QPalette::Highlight).name();
}

QString Color::selectedTextColor()
{
    return isDarkMode() ? "#FFFFFF" : palette->color(QPalette::HighlightedText).name();
}

QString Color::backgroundColor()
{
    return isDarkMode() ? "#2D2D30" : "white";
}

QString Color::borderColor()
{
    return isDarkMode() ? "#555555" : "gray";
}
