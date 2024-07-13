#include "searchbar.h"
#include <QScreen>
#include <windows.h>
#include <QTimer>
#include <QApplication>
#include <QMenu>

HWND searchBar_hwnd;
/*
void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (event == EVENT_SYSTEM_FOREGROUND) {
        // 检测到前台窗口变化
        if (hwnd != searchBar_hwnd) {
            SearchBar& bar = SearchBar::getInstance();
            bar.hide();
        }
    }
}
*/
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
    QPalette palette = this->palette();
    palette.setColor(QPalette::Text, Qt::black);
    setPalette(palette);

    this->setPlaceholderText("Hello, QuickLaunch!");
    setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet("background-color: white;"
                            "border: 1px solid gray;"
                            "border-radius: 10px;"  // 设置圆角半径
                            "padding: 0 8px;"
                            "selection-background-color: lightblue;"
                            "}");
    // 设置事件钩子
/*
    HWINEVENTHOOK hWinEventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        NULL, WinEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    if (!hWinEventHook) {
        qDebug() << "Failed to set event hook." ;
    }
*/
}

void SearchBar::focusOnSearchBar()
{
    this->setFocus();
}

void SearchBar::contextMenuEvent(QContextMenuEvent *event)
{
    // 创建新的上下文菜单
    std::shared_ptr<QMenu> menu = std::make_shared<QMenu>(new QMenu(this));

    // 添加默认的中文菜单项
    QAction *undoAction = menu->addAction(tr("撤销"));
    QAction *redoAction = menu->addAction(tr("重做"));
    menu->addSeparator();
    QAction *cutAction = menu->addAction(tr("剪切"));
    QAction *copyAction = menu->addAction(tr("复制"));
    QAction *pasteAction = menu->addAction(tr("粘贴"));
    menu->addSeparator();
    QAction *deleteAction = menu->addAction(tr("删除"));
    QAction *selectAllAction = menu->addAction(tr("全选"));
    menu->addSeparator();
    // 添加自定义菜单项
    QAction *customAction = menu->addAction(tr("打开设置"));

    // 连接默认菜单项的动作
    connect(undoAction, &QAction::triggered, this, &QLineEdit::undo);
    connect(redoAction, &QAction::triggered, this, &QLineEdit::redo);
    connect(cutAction, &QAction::triggered, this, &QLineEdit::cut);
    connect(copyAction, &QAction::triggered, this, &QLineEdit::copy);
    connect(pasteAction, &QAction::triggered, this, &QLineEdit::paste);
    connect(deleteAction, &QAction::triggered, this, &QLineEdit::del);
    connect(selectAllAction, &QAction::triggered, this, &QLineEdit::selectAll);

    // 连接自定义菜单项的动作
    connect(customAction, &QAction::triggered, this, &SearchBar::openSettingWindow);

    // 显示菜单
    menu->exec(event->globalPos());
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

void SearchBar::openSettingWindow()
{
    emit sg_openSettingWindow();
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
    QWidget::hide();
}
