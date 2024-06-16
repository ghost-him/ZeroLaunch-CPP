#include "searchbar.h"

#include "singleapplication.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include "keyboardhook.h"
#include "controller.h"
#include <QTimer>
#include "settingwindow.h"

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

    SearchBar& searchBar = SearchBar::getInstance();
    searchBar.show();

    KeyboardHook kh;
    app.setProperty("KeyboardHook", QVariant::fromValue((void*)&kh));

    QObject::connect(&kh, &KeyboardHook::keyPressed, [](int keyCode) {
        qDebug() << "Key pressed:" << keyCode;
    });

    QObject::connect(&kh, &KeyboardHook::altSpacePressed, [&]() {
        searchBar.show();
    });

    // System Tray Icon Setup
    QSystemTrayIcon trayIcon;
    QMenu trayMenu;

    QAction exitAction("退出程序", &trayMenu);
    exitAction.setIcon(QIcon(":/icon/exit.svg"));
    QObject::connect(&exitAction, &QAction::triggered, &app, &SingleApplication::quit);

    QAction openSettingAction("打开设置", &trayMenu);
    openSettingAction.setIcon(QIcon(":/icon/setting.svg"));
    QObject::connect(&openSettingAction, &QAction::triggered, &searchBar.getSettingWindow(), &SettingWindow::show);

    trayMenu.addAction(&exitAction);
    trayMenu.addAction(&openSettingAction);

    trayIcon.setContextMenu(&trayMenu);
    trayIcon.setIcon(QIcon(":/icon/trayIcon.svg"));
    trayIcon.setToolTip("QuickLaunch beta 1");
    trayIcon.show();

    // Database::getInstance().testCompareAlgorithm(L"steam1");

    return app.exec();
}
