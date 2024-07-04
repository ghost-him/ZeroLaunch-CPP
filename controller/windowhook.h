#ifndef WINDOWHOOK_H
#define WINDOWHOOK_H

#include <functional>
#include <QWidget>
#include <windows.h>

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

    void setTargetWidget(QWidget* parentWidget);

    void setCallback(std::function<void()> callback);

    void stop();

    WId getWinID();

    void doCallBack();

    ~WindowHook();

private:
    WindowHook();
    bool isStop;
    QWidget* parentWidget;
    std::function<void()> callback;
    HWINEVENTHOOK hHook;
    WindowHook* instance;

    void installHook();
};

#endif // WINDOWHOOK_H
