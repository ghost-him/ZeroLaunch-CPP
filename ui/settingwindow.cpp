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
#include "../controller/utils.h"


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
    ui->tableIndexedApp->setColumnCount(5);
    ui->tableIndexedApp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 设置水平滚动模式为像素级
    ui->tableIndexedApp->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    // 设置垂直滚动模式为像素级
    ui->tableIndexedApp->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    ui->tableIndexedApp->horizontalScrollBar()->setSingleStep(10);
    ui->tableIndexedApp->verticalScrollBar()->setSingleStep(10);
    QStringList indexedAppHeaders;
    indexedAppHeaders << "程序名" << "是否为UWP应用" << "固定偏移值" << "打开总次数" << "程序路径";
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
    ui->spReloadTime->setValue(config.autoReloadTime);
    ui->boxEnableStatistics->setChecked(config.isEnableStatictics);
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

void SettingWindow::addIndexedAppItem(QString programName, bool isUWPApp, int stableBias, int totalOpenCount, QString programPath)
{
    QStringList data {programName,
                     isUWPApp? "true": "false",
                     QString::number(stableBias),
                     QString::number(totalOpenCount),
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

void SettingWindow::clearKeyFilterTable()
{
    on_btnResetKeyTable_clicked();
}

void SettingWindow::addkeyFilterItem(const QString &programName, double stableBias, const QString &note)
{
    // 获取当前的行数
    int rowCount = keyFilterItemModel->rowCount();

    // 插入新的一行
    keyFilterItemModel->insertRow(rowCount);

    // 设置每一列的数据
    QModelIndex index;
    index = keyFilterItemModel->index(rowCount, 0); // 第一列
    keyFilterItemModel->setData(index, programName);

    index = keyFilterItemModel->index(rowCount, 1); // 第二列
    keyFilterItemModel->setData(index, stableBias);

    index = keyFilterItemModel->index(rowCount, 2); // 第三列
    keyFilterItemModel->setData(index, note);
}

void SettingWindow::clearSearchList()
{
    on_btnResetSearchList_clicked();
}

void SettingWindow::addSearchListItem(const QString &searchPath)
{
    on_btnAddSearchItem_clicked();
    ui->searchList->item(ui->searchList->count() - 1)->setText(searchPath);
}

void SettingWindow::clearBannedList()
{
    on_btnResetBannedList_clicked();
}

void SettingWindow::addBannedListItem(const QString &bannedPath)
{
    on_btnAddBannedItem_clicked();
    ui->bannedList->item(ui->bannedList->count() - 1)->setText(bannedPath);
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
    configure.autoReloadTime = ui->spReloadTime->value();
    configure.isEnableStatictics = ui->boxEnableStatistics->isChecked();

    for (int i = 0; i < ui->bannedList->count(); i ++) {
        QListWidgetItem* item = ui->bannedList->item(i);
        configure.bannedPaths.push_back(item->text());
    }

    for (int i = 0; i < ui->searchList->count(); i ++) {
        QListWidgetItem* item = ui->searchList->item(i);
        configure.searchPaths.push_back(item->text());
    }

    int rowCount = keyFilterItemModel->rowCount();
    for (int i = 0; i < rowCount; i ++ ) {
        SettingWindowConfigure::KeyFilter keyFilter;

        auto item = keyFilterItemModel->item(i, 0);
        keyFilter.key = item ? item->text() : "";

        item = keyFilterItemModel->item(i, 1);
        keyFilter.stableBias = item ? item->text().toDouble() : 0.0;

        item = keyFilterItemModel->item(i, 2);
        keyFilter.note = item ? item->text() : "";

        configure.keyFilters.push_back(std::move(keyFilter));
    }

    hide();
    // 保存文件
    emit confirmSetting(configure);
}

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


void SettingWindow::on_btnResetSearchList_clicked()
{
    ui->searchList->clear();
}


void SettingWindow::on_btnAddSearchItem_clicked()
{
    QListWidgetItem *item = new QListWidgetItem(getDefaultItemPlaceHolder());
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->searchList->addItem(item);
}


void SettingWindow::on_btnDelSearchItem_clicked()
{
    // 获取选中的项
    QListWidgetItem *selectedItem = ui->searchList->currentItem();
    if (selectedItem) {
        // 删除选中的项
        delete ui->searchList->takeItem(ui->searchList->row(selectedItem));
    }
}


void SettingWindow::on_btnResetBannedList_clicked()
{
    ui->bannedList->clear();
}


void SettingWindow::on_btnAddBannedItem_clicked()
{
    QListWidgetItem *item = new QListWidgetItem(getDefaultItemPlaceHolder());
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->bannedList->addItem(item);
}


void SettingWindow::on_btnDelBannedItem_clicked()
{
    // 获取选中的项
    QListWidgetItem *selectedItem = ui->bannedList->currentItem();
    if (selectedItem) {
        // 删除选中的项
        delete ui->bannedList->takeItem(ui->bannedList->row(selectedItem));
    }
}


void SettingWindow::on_btnIgnoreUnstallApp_clicked()
{
    addkeyFilterItem("卸载", -5000, "忽略卸载程序");
    addkeyFilterItem("uninstall", -5000, "忽略卸载程序");
}


void SettingWindow::on_btnIgnoreHelpApp_clicked()
{
    addkeyFilterItem("帮助", -5000, "忽略帮助文件");
    addkeyFilterItem("help", -5000, "忽略帮助文件");
}

