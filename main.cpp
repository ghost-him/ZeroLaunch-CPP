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
    trayMenu.addAction(&openSettingAction);
    trayMenu.addAction(&reloadSettingAction);
    trayMenu.addAction(&exitAction);


    trayIcon.setContextMenu(&trayMenu);
    trayIcon.setIcon(QIcon(":/icon/trayIcon.svg"));
    trayIcon.setToolTip(getProgramVersion());
    trayIcon.show();

    //Database::getInstance().testCompareAlgorithm(L"stem");

    return app.exec();
}
