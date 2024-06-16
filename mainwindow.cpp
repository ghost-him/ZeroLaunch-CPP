#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool); // 不显示任务栏图标
    setFocusPolicy(Qt::StrongFocus);
    // 安装事件过滤器
    qApp->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::focusOutEvent(QFocusEvent *event) {
    qDebug() << "Window lost focus, hiding...";
    this->hide();
    QWidget::focusOutEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (!this->geometry().contains(mouseEvent->globalPosition().toPoint())) {
            qDebug() << "Mouse clicked outside the window, hiding...";
            this->hide();
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
