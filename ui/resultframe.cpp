#include "resultframe.h"
#include "ui_resultframe.h"
#include "resultitem.h"
#include <QScreen>
#include <QListWidgetItem>
#include "../controller/controller.h"
#include "../controller/utils.h"
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QSize>
#include "uiutils.h"

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

    setAttribute(Qt::WA_TranslucentBackground);

    this->setGeometry(x_bias, desktop_height/ 4 + y + y / 10, x, y * 4);
    this->setFixedSize({x, y});

    this->itemHeight = y;
    this->itemWidth = x;

    qDebug() << "resultframe: " << x << " " << y;

    QPalette palette = QApplication::palette();
    this->setStyleSheet(
        QString(
            "QFrame {"
            "   border-radius: 10px;"         // 设置 QFrame 的圆角边框
            "   background-color: transparent;" // 设置背景色为透明
            "}"
            "QListWidget {"
            "   border-radius: 10px;"         // 设置 QListWidget 的圆角边框
            "   background-color: %1;" // 设置背景色为透明
            "   border: 1px solid %2;"
            "}"
            "QListWidget::item:selected {"
            "   background-color: %3;"        // 设置选中项的背景颜色
            "   color: %4;"                   // 设置选中项的文字颜色
            "   border-radius: 10px;"
            "   font-weight: bold;"           // 设置选中项的文字加粗
            "}"
            "QListWidget::item {"
            "   padding-left: 4px;"          // 设置左侧填充，使标志更宽
            "   padding-right: 4px;"         // 设置右侧填充，使标志更宽
            "   border-radius: 10px;"
            "   color: %5;"               // 设置普通项的文字颜色
            "   background-color: %1;"    // 设置普通项的背景颜色
            "}"
            ).arg(
                Color::backgroundColor(),
                Color::borderColor(),
                Color::selectedBackgroundColor(),
                Color::selectedTextColor(),
                Color::textColor()
                ));
}

void ResultFrame::setCurrentItemIndex(int row)
{
    ui->listWidget->setCurrentRow(0);
}

int ResultFrame::getCurrentItemIndex()
{
    return ui->listWidget->currentRow();
}

ResultFrame::~ResultFrame()
{
    delete ui;
}

void ResultFrame::clearItem()
{
    ui->listWidget->clear();
}

void ResultFrame::show()
{
    this->setWindowOpacity(1.0); // 重置不透明度
    QWidget::show();
    raise();
}

void ResultFrame::hide()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(50); // 动画持续时间
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);

    connect(animation, &QPropertyAnimation::finished, this, &QWidget::hide);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}


void ResultFrame::addItem(const QPixmap &programIcon, const QString &programName)
{
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    ResultItem* content = new ResultItem(this);

    content->setFixedSize({itemWidth, itemHeight});

    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, content);

    QSize hint = content->sizeHint();
    item->setSizeHint({width() - 20, hint.height()});


    content->setProgramName(programName);
    content->setProgramIcon(programIcon);


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

void ResultFrame::on_listWidget_itemClicked(QListWidgetItem *item)
{
    emit sg_launchSelectedProgram();
}
