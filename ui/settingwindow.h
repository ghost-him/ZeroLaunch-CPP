#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QWidget>
#include "../controller/utils.h"


namespace Ui {
class SettingWindow;
}

class QStandardItemModel;
class QItemSelectionModel;

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

    void addIndexedAppItem(QString programName, bool isUWPApp, int staticBias, QString programPath);

    void adjustIndexedAppTable();

protected:
    void closeEvent(QCloseEvent *event) override;


signals:
    void confirmSetting(SettingWindowConfigure configure);

    void sg_refreshIndexedApp();

private slots:
    void on_btnConfirm_clicked();

    void on_pushButton_clicked();

private:
    explicit SettingWindow(QWidget *parent = nullptr);
    ~SettingWindow();


    Ui::SettingWindow *ui;

    // 维护tableview
    QStandardItemModel* keyFilterItemModel;
    QItemSelectionModel* keyFileterSelectModel;

    int nextRow {0};


};

#endif // SETTINGWINDOW_H
