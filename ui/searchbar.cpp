#include "searchbar.h"
#include <QScreen>
#include <windows.h>
#include <QTimer>
#include <QApplication>
#include <QMenu>
#include <QPropertyAnimation>
#include <QPalette>
#include <QAction>
#include "uiutils.h"

HWND searchBar_hwnd;

SearchBar::SearchBar() {
    searchBar_hwnd = (HWND)winId();
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool); // 不显示任务栏图标
    setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    setWindowFlags(windowFlags() & ~Qt::WindowDoesNotAcceptFocus);

    setFocusPolicy(Qt::StrongFocus);

    QList<QScreen *> list_screen =  QGuiApplication::screens();
    QRect rect = list_screen.at(0)->geometry();
    auto desktop_width = rect.width();
    auto desktop_height = rect.height();

    auto x = desktop_width / 2;
    auto y = desktop_height / 3 / 5;

    auto x_bias = desktop_width / 2 - x / 2;


    this->setGeometry(x_bias, desktop_height/ 4, x, y);
    this->setFixedSize({x, y});
    qDebug() << "searchBar: " << x << " " << y;

    // 更改字体的大小
    QFont font;
    font.setPixelSize(y*0.45);
    this->setFont(font);

    // 更改字体的颜色
    QPalette palette = QApplication::palette();
    palette.setColor(QPalette::Text, Qt::black);
    setPalette(palette);

    setAttribute(Qt::WA_TranslucentBackground);

    this->setStyleSheet(QString(
                        "QLineEdit{"
                            "background-color: white;"
                            "border: 1px solid gray;"
                            "border-radius: 10px;"  // 设置圆角半径
                            "padding: 8px;"
                        "}"));
}

void SearchBar::focusOnSearchBar()
{
    this->setFocus();
}

void SearchBar::contextMenuEvent(QContextMenuEvent *event)
{
    // 创建新的上下文菜单
    QMenu menu(this);
    menu.setAttribute(Qt::WA_TranslucentBackground);

    // 添加自定义菜单项

    QAction* openSettingAction = createOpenSettingAction(&menu);
    QAction* reloadSettingAction = createReloadSettingAction(&menu);
    QAction* showHelpAction = createShowHelpAction(&menu);

    menu.addAction(openSettingAction);
    menu.addAction(reloadSettingAction);
    menu.addAction(showHelpAction);

    // 显示菜单
    menu.exec(event->globalPos());
}

void SearchBar::keyPressEvent(QKeyEvent *event)
{
    // 检查按下的键是否为 'j' 且 Ctrl 键是否按下
    if (event->key() == Qt::Key_J && event->modifiers() & Qt::ControlModifier) {
        qDebug() << "同时按下了ctrl + j";
        emit selectForwardItem();
    }else if(event->key() == Qt::Key_K && event->modifiers() & Qt::ControlModifier){
        qDebug() << "同时按下了ctrl + k";
        emit selectBackwardItem();
    }else if(event->key() == Qt::Key_Up){
        qDebug() << "按下了上";
        emit selectBackwardItem();
    }else if(event->key() == Qt::Key_Down){
        qDebug() << "按下了下";
        emit selectForwardItem();
    }else if(event->key() == Qt::Key_Escape){
        qDebug() << "按下了esc";
        pressESC();
    }else if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        qDebug() << "按下了enter";
        emit launchSelectedProgram();
    } else {
        // 调用父类的 keyPressEvent 方法以确保正常处理其他按键
        QLineEdit::keyPressEvent(event);
    }
}

bool SearchBar::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_ACTIVATE) {
            if (LOWORD(msg->wParam) != WA_INACTIVE) {
                setFocus();
            }
        }
    }
    return QLineEdit::nativeEvent(eventType, message, result);
}

void SearchBar::pressESC()
{
    if (text().isEmpty()) {
        emit hideProgram();
    } else {
        setText("");
        return;
    }
}

void SearchBar::show()
{
    this->setWindowOpacity(1.0); // 重置不透明度
    setText("");
    QWidget::show();
    raise();
    activateWindow();

    HWND hwnd = (HWND)winId();
    HWND fgWindow = GetForegroundWindow();
    if (fgWindow) {
        DWORD fgThread = GetWindowThreadProcessId(fgWindow, NULL);
        DWORD thisThread = GetCurrentThreadId();
        AttachThreadInput(fgThread, thisThread, TRUE);
        SetForegroundWindow(hwnd);
        AttachThreadInput(fgThread, thisThread, FALSE);
    } else {
        SetForegroundWindow(hwnd);
    }

    setFocus();
}

void SearchBar::hide()
{

    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(50); // 动画持续时间
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);

    connect(animation, &QPropertyAnimation::finished, this, &QLineEdit::hide);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
    //QLineEdit::hide();
}
