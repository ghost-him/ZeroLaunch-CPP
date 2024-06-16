#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QWidget>

namespace Ui {
class SettingWindow;
}

class SettingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingWindow(QWidget *parent = nullptr);
    ~SettingWindow();

    void initWindow(const QJsonObject& config);

private slots:
    void on_btnConfirm_clicked();

    void on_btnCustomDir_clicked();

    void on_btnBannedDir_clicked();

private:
    Ui::SettingWindow *ui;
};

#endif // SETTINGWINDOW_H
