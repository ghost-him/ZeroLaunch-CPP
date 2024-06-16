#include "settingwindow.h"
#include "ui_settingwindow.h"
#include "controller.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QJsonObject>

SettingWindow::SettingWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool); // 不显示任务栏图标
}

SettingWindow::~SettingWindow()
{
    delete ui;
}

void SettingWindow::initWindow(const QJsonObject &config)
{

    ui->boxSearchStartMenu->setChecked(config["searchStartMenu"].toBool());
    ui->boxAutoStart->setChecked(config["isAutoStart"].toBool());
    ui->boxPreLoadResource->setChecked(config["preLoadResource"].toBool());
    ui->boxSearchProgramFile->setChecked(config["searchProgramFile"].toBool());
    ui->boxSearchRegistry->setChecked(config["searchRegistry"].toBool());
    ui->spResultItemNumber->setValue(config["resultItemNumber"].toInt());
    ui->LEPlaceholderText->setText(config["searchBarPlaceholderText"].toString());
}

void SettingWindow::on_btnConfirm_clicked()
{
    QJsonObject json;

    json["isAutoStart"] = ui->boxAutoStart->isChecked();
    json["searchStartMenu"] = ui->boxSearchStartMenu->isChecked();
    json["searchRegistry"] = ui->boxSearchRegistry->isChecked();
    json["searchProgramFile"] = ui->boxSearchProgramFile->isChecked();
    json["preLoadResource"] = ui->boxPreLoadResource->isChecked();
    json["resultItemNumber"] = ui->spResultItemNumber->value();
    json["searchBarPlaceholderText"] = ui->LEPlaceholderText->text();
    // 保存文件
    Controller::getInstance().saveConfigure(json);
    hide();
    Controller::getInstance().init();

}

void SettingWindow::on_btnCustomDir_clicked()
{
    QString filePath = QApplication::applicationDirPath() + "/customProgramDir.txt";

    // 使用QDesktopServices打开该文件
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(filePath))) {
        QMessageBox::warning(nullptr, "Error", "Cannot open file with default application.");
    }

}


void SettingWindow::on_btnBannedDir_clicked()
{
    QString filePath = QApplication::applicationDirPath() + "/bannedProgramDir.txt";

    // 使用QDesktopServices打开该文件
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(filePath))) {
        QMessageBox::warning(nullptr, "Error", "Cannot open file with default application.");
    }

}

