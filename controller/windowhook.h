#ifndef WINDOWHOOK_H
#define WINDOWHOOK_H

#include <functional>
#include <QWidget>
#include <windows.h>

class WindowHook
{
public:
    WindowHook();

    void setTargetWidget(QWidget* parentWidget);

    void setCallback(std::function<void()> callback);

    ~WindowHook();

private:
    QWidget* parentWidget;
    std::function<void()> callback;
    HWINEVENTHOOK hHook;
    static WindowHook* instance;

    static void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);


    void installHook();
};

#endif // WINDOWHOOK_H
