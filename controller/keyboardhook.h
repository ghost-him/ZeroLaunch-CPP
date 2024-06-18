#ifndef KEYBOARDHOOK_H
#define KEYBOARDHOOK_H

#include <QObject>
#include <Windows.h>
#include <QMessageBox>
class KeyboardHook : public QObject
{
public:
    // 删除拷贝构造函数和拷贝赋值操作符
    KeyboardHook(KeyboardHook const&) = delete;
    KeyboardHook& operator=(KeyboardHook const&) = delete;
    // 提供一个全局唯一的接口
    static KeyboardHook& getInstance() {
        static KeyboardHook instance;
        return instance;
    }


    using Callback = std::function<void()>;



    void setCallback(Callback callback);

    bool startHook(const std::vector<int>& keys, const std::vector<std::pair<int, int>>& comboKeys);

    ~KeyboardHook();
private:
    KeyboardHook();


    HHOOK hHook;
    std::vector<int> keys;
    std::vector<std::pair<int, int>> comboKeys;
    Callback callback;

    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

    bool isKeyInList(int vkCode) const;

    bool isComboKeyPressed(int vkCode) const;

    static void* instance;
};

#endif // KEYBOARDHOOK_H
