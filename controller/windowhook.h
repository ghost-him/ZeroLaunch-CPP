#ifndef WINDOWHOOK_H
#define WINDOWHOOK_H

#include <functional>
#include <QWidget>
#include <windows.h>
#include <unordered_map>

class WindowHook
{
public:
    // 删除拷贝构造函数和拷贝赋值操作符
    WindowHook(WindowHook const&) = delete;
    WindowHook& operator=(WindowHook const&) = delete;
    // 提供一个全局唯一的接口
    static WindowHook& getInstance() {
        static WindowHook instance;
        return instance;
    }

    void stop();

    ~WindowHook();

    void hideWindow(HWND targetWinId);

    void registerWinID(WId winID, std::function<void()> callbackFunc);

    void registerDefaultCallback(std::function<void()> callbackFunc);

private:
    WindowHook();
    bool isStop;
    QWidget* parentWidget;
    std::function<void()> defaultCallback;
    HWINEVENTHOOK hHook;
    WindowHook* instance;

    void installHook();

    std::unordered_map<WId, std::function<void()>> store;

};

#endif // WINDOWHOOK_H
