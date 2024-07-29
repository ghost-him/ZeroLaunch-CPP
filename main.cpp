#include "ui/searchbar.h"

#include "ui/singleapplication.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include "controller/controller.h"
#include <QTimer>
#include "ui/settingwindow.h"
#include <QMessageBox>
#include "controller/utils.h"
#include "controller/controller.h"
#include "controller/windowhook.h"
#include <QJsonDocument>
#include <QStyleHints>

int main(int argc, char *argv[])
{
    SingleApplication app(argc, argv);

    if (app.isRunning()) {
        QMessageBox::warning(nullptr, "提示", "当前程序已在运行");
        return 0;
    }
    app.setWindowIcon(QIcon(":/icon/trayIcon.svg"));

    Controller& controller = Controller::getInstance();
    controller.init();

    // System Tray Icon Setup
    QSystemTrayIcon trayIcon;
    QMenu trayMenu;

    QAction exitAction("退出程序", &trayMenu);
    exitAction.setIcon(QIcon(":/icon/exit.svg"));
    QObject::connect(&exitAction, &QAction::triggered, [&](){
        WindowHook::getInstance().stop();
        app.quit();
    });

    QAction openSettingAction("打开设置", &trayMenu);
    openSettingAction.setIcon(QIcon(":/icon/setting.svg"));
    QObject::connect(&openSettingAction, &QAction::triggered, [](){
        SettingWindow& settingWindow = SettingWindow::getInstance();
        settingWindow.show();
    });

    QAction reloadSettingAction("刷新数据库", &trayMenu);
    reloadSettingAction.setIcon(QIcon(":/icon/refresh.svg"));
    QObject::connect(&reloadSettingAction, &QAction::triggered, [](){
        Controller& controller = Controller::getInstance();
        controller.init();
    });
    QAction showHelp("打开帮助页面", &trayMenu);
    showHelp.setIcon(QIcon(":/icon/help.svg"));

    QObject::connect(&showHelp, &QAction::triggered, [](){
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

    trayMenu.addAction(&openSettingAction);
    trayMenu.addAction(&reloadSettingAction);
    trayMenu.addAction(&showHelp);
    trayMenu.addAction(&exitAction);


    trayIcon.setContextMenu(&trayMenu);
    trayIcon.setIcon(QIcon(":/icon/trayIcon.svg"));
    trayIcon.setToolTip(getProgramVersion());
    trayIcon.show();
    return app.exec();
}
