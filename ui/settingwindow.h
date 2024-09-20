#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QWidget>
#include "../controller/utils.h"
#include "spinboxdelegate.h"
#include <QEvent>

namespace Ui {
class SettingWindow;
class NoToolTipFilter;
}

class QTimer;
class QStandardItemModel;
class QItemSelectionModel;

class NoToolTipFilter : public QObject
{
public:
    explicit NoToolTipFilter(QObject* parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

};

class SettingWindow : public QWidget
{
    Q_OBJECT

public:
    // 删除拷贝构造函数和拷贝赋值操作符
    SettingWindow(SettingWindow const&) = delete;
    SettingWindow& operator=(SettingWindow const&) = delete;
    // 提供一个全局唯一的接口
    static SettingWindow& getInstance() {
        static SettingWindow instance;
        return instance;
    }

    void initWindow(const SettingWindowConfigure& config);

    void show();

    void clearIndexedAppTable();
    // 向已索引的app展示表中添加一项
    void addIndexedAppItem(QString programName, bool isUWPApp, int stableBias, int totalOpenCount, QString programPath);

    void adjustIndexedAppTable();


    void clearKeyFilterTable();
    void addkeyFilterItem(const QString& programName, double stableBias, const QString& note);

    void clearSearchList();
    void addSearchListItem(const QString& searchPath);

    void clearBannedList();
    void addBannedListItem(const QString& bannedPath);


protected:
    void closeEvent(QCloseEvent *event) override;


signals:
    void confirmSetting(SettingWindowConfigure configure);

    void sg_refreshIndexedApp();

private slots:
    void on_btnConfirm_clicked();

    void on_pushButton_clicked();

    void on_btnAddKeyItem_clicked();

    void on_btnDelKeyItem_clicked();

    void on_btnResetKeyTable_clicked();

    void on_btnResetSearchList_clicked();

    void on_btnAddSearchItem_clicked();

    void on_btnDelSearchItem_clicked();

    void on_btnResetBannedList_clicked();

    void on_btnAddBannedItem_clicked();

    void on_btnDelBannedItem_clicked();

    void on_btnIgnoreUnstallApp_clicked();

    void on_btnIgnoreHelpApp_clicked();

private:
    explicit SettingWindow(QWidget *parent = nullptr);
    ~SettingWindow();

    void startAutoReloadTime(QTime millisecond);


    Ui::SettingWindow *ui;

    // 维护tableview
    QStandardItemModel* keyFilterItemModel;
    QItemSelectionModel* keyFilterSelectModel;
    SpinBoxDelegate* spinBoxDelegate;
    int nextRow {0};

};

#endif // SETTINGWINDOW_H
