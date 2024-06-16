#include "searchbar.h"
#include <QScreen>
#include <windows.h>
#include <QTimer>
#include <QApplication>

HWND searchBar_hwnd;
void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (event == EVENT_SYSTEM_FOREGROUND) {
        // 检测到前台窗口变化
        if (hwnd != searchBar_hwnd) {
            SearchBar& bar = SearchBar::getInstance();
            bar.hide();
        }
    }
}

SearchBar::SearchBar() {
    searchBar_hwnd = (HWND)winId();
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool); // 不显示任务栏图标
    setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    setWindowFlags(windowFlags() & ~Qt::WindowDoesNotAcceptFocus);

    setFocusPolicy(Qt::StrongFocus);

    connect(qApp, &QApplication::focusChanged, this, [this](QWidget *old, QWidget *now) {
        if (now != this && now != &resultFrame && now != this->parentWidget()) {
            qDebug() << "Focus changed to another widget, hiding...";
            this->hide();
        }
    });

    QList<QScreen *> list_screen =  QGuiApplication::screens();
    QRect rect = list_screen.at(0)->geometry();
    auto desktop_width = rect.width();
    auto desktop_height = rect.height();

    auto x = desktop_width / 2;
    auto y = desktop_height / 3 / 5;

    auto x_bias = desktop_width / 2 - x / 2;


    this->setGeometry(x_bias, desktop_height/ 4, x, y);

    // 更改字体的大小
    QFont font;
    font.setPixelSize(20);
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

    connect(this, &QLineEdit::textChanged, &resultFrame, &ResultFrame::updateInputText);
    // 设置事件钩子

    HWINEVENTHOOK hWinEventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        NULL, WinEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    if (!hWinEventHook) {
        qDebug() << "Failed to set event hook." ;
    }
    settingWindow.hide();
}

void SearchBar::focusOnSearchBar()
{
    this->setFocus();
}


void SearchBar::pressESC()
{
    if (text().isEmpty()) {
        // 隐藏
        hide();
        return;
    } else {
        setText("");
        return;
    }
}

void SearchBar::pressEnter()
{
    resultFrame.confirmCurrentItem();
    hide();
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
    resultFrame.show();
    resultFrame.raise();
}

void SearchBar::hide()
{
    resultFrame.hide();
    QWidget::hide();
}

void SearchBar::initProgramIcon()
{
    resultFrame.clearIconCache();
    qDebug() << "开始预加载图标";
    resultFrame.initProgramIcon();
}

void SearchBar::initSettingWindow(const QJsonObject &json)
{
    settingWindow.initWindow(json);
}

void SearchBar::setResultItemNumber(int number)
{
    resultFrame.setResultItemNumber(number);
}

const SettingWindow &SearchBar::getSettingWindow()
{
    return settingWindow;
}
