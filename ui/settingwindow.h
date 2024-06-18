#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QWidget>
#include "../controller/utils.h"

namespace Ui {
class SettingWindow;
}

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

signals:
    void confirmSetting(SettingWindowConfigure configure);

private slots:
    void on_btnConfirm_clicked();

    void on_btnCustomDir_clicked();

    void on_btnBannedDir_clicked();

private:
    explicit SettingWindow(QWidget *parent = nullptr);
    ~SettingWindow();


    Ui::SettingWindow *ui;
};

#endif // SETTINGWINDOW_H
