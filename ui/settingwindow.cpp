#include "settingwindow.h"
#include "ui_settingwindow.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QJsonObject>
#include "../controller/utils.h"
#include <QCloseEvent>
#include <QItemDelegate>
#include <QTableView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QScrollBar>


SettingWindow::SettingWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("设置");
    // 设置窗口无法更改大小
    this->setFixedSize(this->size());

    // 初始化4个标签页的名字
    auto& tabWidget = ui->tabWidget;

    tabWidget->setTabText(0, "程序设置");
    tabWidget->setTabText(1, "自定义搜索路径");
    tabWidget->setTabText(2, "关键字过滤设置");
    tabWidget->setTabText(3, "查看当前索引的所有的程序");

    // 初始化关键字过滤器的表格

    QStringList keyFilterHeaders;
    keyFilterHeaders << "关键字" << "偏移值" << "备注";
    //ui->tableKeyFilter->setHorizontalHeaderLabels(keyFilterHeaders);
    keyFilterItemModel = new QStandardItemModel(0, 3, this);
    keyFilterSelectModel = new QItemSelectionModel(keyFilterItemModel, this);

    ui->tableKeyFilter->setModel(keyFilterItemModel);
    ui->tableKeyFilter->setSelectionModel(keyFilterSelectModel);
    ui->tableKeyFilter->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableKeyFilter->setSelectionBehavior(QAbstractItemView::SelectItems);

    spinBoxDelegate = new SpinBoxDelegate(this);
    ui->tableKeyFilter->setItemDelegateForColumn(1, spinBoxDelegate);

    NoToolTipFilter *filter = new NoToolTipFilter(this);
    ui->tableKeyFilter->viewport()->installEventFilter(filter);

    keyFilterItemModel->setHorizontalHeaderLabels(keyFilterHeaders);
    ui->tableKeyFilter->horizontalHeader()->setStretchLastSection(true);

    ui->tableKeyFilter->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableKeyFilter->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableKeyFilter->horizontalScrollBar()->setSingleStep(10);
    ui->tableKeyFilter->verticalScrollBar()->setSingleStep(10);

    // 初始化
    ui->tableIndexedApp->setColumnCount(4);
    ui->tableIndexedApp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 设置水平滚动模式为像素级
    ui->tableIndexedApp->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    // 设置垂直滚动模式为像素级
    ui->tableIndexedApp->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    ui->tableIndexedApp->horizontalScrollBar()->setSingleStep(10);
    ui->tableIndexedApp->verticalScrollBar()->setSingleStep(10);
    QStringList indexedAppHeaders;
    indexedAppHeaders << "程序名" << "是否为UWP应用" << "固定偏移值" << "程序路径";
    ui->tableIndexedApp->setHorizontalHeaderLabels(indexedAppHeaders);

}

SettingWindow::~SettingWindow()
{
    delete ui;
}

void SettingWindow::initWindow(const SettingWindowConfigure &config)
{

    ui->boxSearchStartMenu->setChecked(config.isSearchStartMenu);
    ui->boxAutoStart->setChecked(config.isAutoStart);
    ui->boxPreLoadResource->setChecked(config.isPreLoadResource);
    ui->boxSearchProgramFile->setChecked(config.isSearchProgramFile);
    ui->boxSearchRegistry->setChecked(config.isSearchRegistry);
    ui->spResultItemNumber->setValue(config.resultItemNumber);
    ui->LEPlaceholderText->setText(config.searchBarPlaceholderText);
    ui->LEEmptyText->setText(config.resultFrameEmptyText);
    ui->boxSearchUWP->setChecked(config.isSearchUWP);
    ui->boxIgnoreUninstall->setChecked(config.isIgnoreUninstallApp);
}

void SettingWindow::show()
{
    QWidget::show();
    raise();
    activateWindow();
    setFocus();
}

void SettingWindow::clearIndexedAppTable()
{
    ui->tableIndexedApp->clearContents();
    nextRow = 0;
}

void SettingWindow::addIndexedAppItem(QString programName, bool isUWPApp, int staticBias, QString programPath)
{
    QStringList data {programName,
                     isUWPApp? "true": "false",
                     QString::number(staticBias),
                     programPath};

    if (nextRow == ui->tableIndexedApp->rowCount()) {
        ui->tableIndexedApp->insertRow(nextRow);
    }

    for (int col {0}; col < data.size(); col ++) {
        QTableWidgetItem* item = new QTableWidgetItem(data.at(col));
        item->setToolTip(data.at(col));
        ui->tableIndexedApp->setItem(nextRow, col, item);
    }
    nextRow ++;
}

void SettingWindow::adjustIndexedAppTable()
{
    ui->tableIndexedApp->sortItems(0, Qt::AscendingOrder);
    ui->tableIndexedApp->resizeColumnsToContents();
}

void SettingWindow::closeEvent(QCloseEvent *event)
{
    this->hide();
    event->ignore();  // 忽略关闭事件
}

void SettingWindow::on_btnConfirm_clicked()
{
    SettingWindowConfigure configure;

    configure.isAutoStart = ui->boxAutoStart->isChecked();
    configure.isSearchStartMenu = ui->boxSearchStartMenu->isChecked();
    configure.isSearchRegistry = ui->boxSearchRegistry->isChecked();
    configure.isSearchProgramFile = ui->boxSearchProgramFile->isChecked();
    configure.isPreLoadResource = ui->boxPreLoadResource->isChecked();
    configure.resultItemNumber = ui->spResultItemNumber->value();
    configure.searchBarPlaceholderText = ui->LEPlaceholderText->text();
    configure.resultFrameEmptyText = ui->LEEmptyText->text();
    configure.isSearchUWP = ui->boxSearchUWP->isChecked();
    configure.isIgnoreUninstallApp = ui->boxIgnoreUninstall->isChecked();
    // 保存文件
    emit confirmSetting(configure);
    hide();
}
/*
void SettingWindow::on_btnCustomDir_clicked()
{
    QString filePath = getCustomDirectoryPath();

    // 使用QDesktopServices打开该文件
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(filePath))) {
        QMessageBox::warning(nullptr, "Error", "Cannot open file with default application.");
    }

}


void SettingWindow::on_btnBannedDir_clicked()
{
    QString filePath = getBannedDirectoryPath();

    // 使用QDesktopServices打开该文件
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(filePath))) {
        QMessageBox::warning(nullptr, "Error", "Cannot open file with default application.");
    }

}
*/

void SettingWindow::on_pushButton_clicked()
{
    // 更新当前的显示的内容
    emit sg_refreshIndexedApp();
}


void SettingWindow::on_btnAddKeyItem_clicked()
{
    // 添加一行
    int row = keyFilterItemModel->rowCount();
    keyFilterItemModel->insertRow(row);
}


void SettingWindow::on_btnDelKeyItem_clicked()
{
    if (keyFilterSelectModel->hasSelection()) {
        int row = keyFilterSelectModel->currentIndex().row();
        keyFilterItemModel->removeRow(row);
    }
}


NoToolTipFilter::NoToolTipFilter(QObject *parent)
    : QObject(parent)
{

}

bool NoToolTipFilter::eventFilter(QObject *watched, QEvent *event)
{
    {
        if (event->type() == QEvent::ToolTip) {
            return true; // 阻止事件继续传播
        }
        return QObject::eventFilter(watched, event);
    }
}

void SettingWindow::on_btnResetKeyTable_clicked()
{
    // 保存当前的表头标签
    QStringList horizontalHeaders;
    for (int i = 0; i < keyFilterItemModel->columnCount(); ++i) {
        horizontalHeaders << keyFilterItemModel->horizontalHeaderItem(i)->text();
    }

    keyFilterItemModel->clear();
    keyFilterItemModel->setHorizontalHeaderLabels(horizontalHeaders);
}

