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
#include "ui/uiutils.h"

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

    QAction* exitAction = createExitAction(&trayMenu);
    QAction* openSettingAction = createOpenSettingAction(&trayMenu);
    QAction* reloadSettingAction = createReloadSettingAction(&trayMenu);
    QAction* showHelpAction = createShowHelpAction(&trayMenu);

    trayMenu.addAction(openSettingAction);
    trayMenu.addAction(reloadSettingAction);
    trayMenu.addAction(showHelpAction);
    trayMenu.addAction(exitAction);


    trayIcon.setContextMenu(&trayMenu);
    trayIcon.setIcon(QIcon(":/icon/trayIcon.svg"));
    trayIcon.setToolTip(getProgramVersion());
    trayIcon.show();
    return app.exec();
}
