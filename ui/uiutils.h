#ifndef UIUTILS_H
#define UIUTILS_H

#include <QAction>


// 获取Action
QAction* createExitAction(QObject *parent);

QAction* createOpenSettingAction(QObject* parent);

QAction* createReloadSettingAction(QObject *parent);

QAction* createShowHelpAction(QObject *parent);


#endif // UIUTILS_H
