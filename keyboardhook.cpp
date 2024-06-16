#include "keyboardhook.h"
#include <QCoreApplication>

KeyboardHook::KeyboardHook(QObject *parent)
    : QObject{parent}
{
    hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!hook) {
        QMessageBox::critical(nullptr, "Error", "Failed to install keyboard hook!");
    }
}

KeyboardHook::~KeyboardHook()
{
    UnhookWindowsHookEx(hook);
}

LRESULT KeyboardHook::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            if (p->vkCode == VK_SPACE) {
                // 检查Alt键是否同时被按下
                if (GetAsyncKeyState(VK_MENU) & 0x8000) {  // VK_MENU 是 Alt 键的虚拟键码
                    emit instance()->altSpacePressed();
                    return 1;
                }
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

KeyboardHook *KeyboardHook::instance()
{
    return static_cast<KeyboardHook*>(qApp->property("KeyboardHook").value<void*>());
}
