#include "ui/searchbar.h"

#include "ui/singleapplication.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include "controller/controller.h"
#include <QTimer>
#include "ui/settingwindow.h"
#include <QMessageBox>
#include "controller/utils.h"

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
    QObject::connect(&exitAction, &QAction::triggered, &app, &SingleApplication::quit);

    QAction openSettingAction("打开设置", &trayMenu);
    openSettingAction.setIcon(QIcon(":/icon/setting.svg"));
    QObject::connect(&openSettingAction, &QAction::triggered, [](){
        SettingWindow& settingWindow = SettingWindow::getInstance();
        settingWindow.show();
    });

    trayMenu.addAction(&exitAction);
    trayMenu.addAction(&openSettingAction);

    trayIcon.setContextMenu(&trayMenu);
    trayIcon.setIcon(QIcon(":/icon/trayIcon.svg"));
    trayIcon.setToolTip(getProgramVersion());
    trayIcon.show();

    //Database::getInstance().testCompareAlgorithm(L"stem");

    return app.exec();
}
