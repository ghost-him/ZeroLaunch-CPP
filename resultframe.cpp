#include "resultframe.h"
#include "ui_resultframe.h"
#include "resultitem.h"
#include <QScreen>
#include <QListWidgetItem>
#include "controller.h"
#include "utils.h"
#include <QScrollBar>
#include <string>
#include <thread>
#include <algorithm>
#include "searchbar.h"

ResultFrame::ResultFrame(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ResultFrame)
{
    ui->setupUi(this);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool); // 不显示任务栏图标
    setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    // 禁止垂直和水平滚动
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 不显示垂直和水平滚动条
    ui->listWidget->verticalScrollBar()->setVisible(false);
    ui->listWidget->horizontalScrollBar()->setVisible(false);

    QList<QScreen *> list_screen =  QGuiApplication::screens();
    QRect rect = list_screen.at(0)->geometry();
    auto desktop_width = rect.width();
    auto desktop_height = rect.height();

    auto x = desktop_width / 2;
    auto y = desktop_height / 3 / 5;

    auto x_bias = desktop_width / 2 - x / 2;


    this->setGeometry(x_bias, desktop_height/ 4 + y + y / 10, x, y * 4);
    setAttribute(Qt::WA_TranslucentBackground);

    ui->frame->setStyleSheet("{background-color: white;"
                             "border: 2px solid gray;"
                             "border-radius: 10px;"
                             "selection-background-color: lightblue;"
                             "}");
    ui->listWidget->setStyleSheet("");
    updateInputText("");
}

ResultFrame::~ResultFrame()
{
    delete ui;
}

void ResultFrame::updateInputText(const QString &inputText)
{
    clearItem();
    if (inputText.isEmpty()) {
        QPixmap pixmap(":/icon/tips.svg");

        addItem(pixmap, "当前无结果");
        adjustSizeToFitItems();
        return ;
    }
    static Controller& controller = Controller::getInstance();
    auto& ret = controller.changedText(inputText);

    for (int i = 0; i < resultItemNumber && i < ret.size(); i ++) {
        auto& programItem = ret[i];
        const std::wstring& programName = programItem.programName;
        const std::wstring& programPath = programItem.programPath;

        const QPixmap& programIcon = getIconWithPath(programPath);
        QString str = QString::fromStdWString(programName);

        addItem(programIcon, str);
    }
    adjustSizeToFitItems();
    ui->listWidget->setCurrentRow(0);
}

void ResultFrame::clearItem()
{
    ui->listWidget->clear();
}

void ResultFrame::focusOutEvent(QFocusEvent *event) {
    qDebug() << "ResultFrame lost focus, hiding...";
    this->hide();
    QWidget::focusOutEvent(event);
}

void ResultFrame::initProgramIcon()
{
    /*
    // 开多个线程来获取所有的图标，来加快搜索
    int thread_number = std::thread::hardware_concurrency();

    Database& db = Database::getInstance();
    const auto& programs = db.getProgramsFile();
    int loadNumberPerThread = max((int)programs.size() / thread_number, 1);

    std::vector<std::thread> threads;
    threads.reserve(thread_number);

    for (int i = 0; i < thread_number; i++) {
        qDebug() << "已启动id: " << i;
        threads.emplace_back([this, i, loadNumberPerThread, &programs]() {
            int count = 0;
            for (int j = i * loadNumberPerThread; j < (i + 1) * loadNumberPerThread && j < programs.size(); j++) {
                const ProgramNode& program = programs[j];
                const std::wstring& programPath = program.programPath;
                getIconWithPath(programPath);
                count++;
            }
            qDebug() << "id:" << i << "已成功加载：" << count;
        });
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
*/
    Database& db = Database::getInstance();
    const auto& programs = db.getProgramsFile();
    int count = 0;
    for (int i = 0; i < programs.size(); i ++) {
        const ProgramNode& program = programs[i];
        const std::wstring& programPath = program.programPath;
        getIconWithPath(programPath);
        count ++;
    }
    qDebug() << "已成功加载：" << count ;
}

void ResultFrame::setResultItemNumber(int number)
{
    resultItemNumber = number;
}

void ResultFrame::clearIconCache()
{
    iconCache.clear();
}


void ResultFrame::addItem(const QPixmap &programIcon, const QString &programName)
{
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    ResultItem* content = new ResultItem(this);

    content->setProgramName(programName);
    content->setProgramIcon(programIcon);

    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, content);

    QSize hint = content->sizeHint();
    item->setSizeHint(QSize(width() - 20, hint.height()));
}

const QPixmap& ResultFrame::getIconWithPath(const std::wstring &path)
{
    std::unique_lock<std::mutex> guard(cacheLock);
    if (iconCache.contains(path)) {
        return iconCache[path];
    }
    guard.unlock();
    QString str = QString::fromStdWString(path);
    QPixmap icon = GetFileIcon(str);

    guard.lock();
    iconCache[path] = std::move(icon);
    return iconCache[path];
}

void ResultFrame::selectForwardItem()
{
    int currentRow = ui->listWidget->currentRow();
    if (currentRow < ui->listWidget->count() - 1) {
        ui->listWidget->setCurrentRow(currentRow + 1);
    }
}

void ResultFrame::selectBackwardItem()
{
    int currentRow = ui->listWidget->currentRow();
    if (currentRow > 0) {
        ui->listWidget->setCurrentRow(currentRow - 1);
    }

}

void ResultFrame::confirmCurrentItem()
{
    int currentRow = ui->listWidget->currentRow();
    Controller& control = Controller::getInstance();
    // 如果当前没有选中任何一个，则返回
    if (currentRow < 0)
        return;
    control.runProgramWithIndex(currentRow);
}

void ResultFrame::adjustSizeToFitItems()
{
    // Adjust size policy to ensure no extra space at the bottom
    ui->listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Adjust the size hint to fit the contents
    int totalHeight = 0;
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        totalHeight += ui->listWidget->sizeHintForRow(i);
    }
    ui->listWidget->setFixedHeight(totalHeight + 2 * ui->listWidget->frameWidth());



    totalHeight = 0;
    for (int i = 0; i < ui->listWidget->count(); i ++) {
        totalHeight +=  ui->listWidget->sizeHintForRow(i);
    }

    // 添加顶部和底部的边距
    totalHeight +=  ui->listWidget->frameWidth() * 2;

    // 设置新的高度
    setFixedHeight(totalHeight);
}



