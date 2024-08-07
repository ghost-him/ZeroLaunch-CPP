#ifndef UIUTILS_H
#define UIUTILS_H

#include <QAction>
#include <QPalette>


// 获取Action
QAction* createExitAction(QObject *parent);

QAction* createOpenSettingAction(QObject* parent);

QAction* createReloadSettingAction(QObject *parent);

QAction* createShowHelpAction(QObject *parent);

class Color {
public:
    static bool isDarkMode();
    static QString textColor();
    static QString selectedBackgroundColor();
    static QString selectedTextColor();
    static QString backgroundColor();
    static QString borderColor();
private:
    static QPalette* palette;
};




#endif // UIUTILS_H
