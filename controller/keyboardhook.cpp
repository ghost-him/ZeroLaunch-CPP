#include "keyboardhook.h"
#include <QCoreApplication>

void* KeyboardHook::instance = nullptr;

KeyboardHook::KeyboardHook() : hHook(NULL) {
    instance = this; // Set the instance pointer
}

KeyboardHook::~KeyboardHook()
{
    if (hHook) {
        UnhookWindowsHookEx(hHook);
    }
}

void KeyboardHook::setCallback(Callback callback)
{
    this->callback = callback;
}

bool KeyboardHook::startHook(const std::vector<int> &keys, const std::vector<std::pair<int, int> > &comboKeys)
{
    if (hHook) {
        return false; // Hook already set
    }

    this->keys = keys;
    this->comboKeys = comboKeys;

    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    if (!hHook) {
        return false;
    }

    return true;
}

LRESULT KeyboardHook::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
        KeyboardHook* pThis = static_cast<KeyboardHook*>(instance);

        if (pThis && ( pThis->isKeyInList(pKeyboard->vkCode) || pThis->isComboKeyPressed(pKeyboard->vkCode))) {
            if (pThis->callback) {
                pThis->callback();
                return 1;
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

bool KeyboardHook::isKeyInList(int vkCode) const
{
    for (int key : keys) {
        if (key == vkCode) {
            return true;
        }
    }
    return false;
}

bool KeyboardHook::isComboKeyPressed(int vkCode) const
{
    for (const auto& combo : comboKeys) {
        if (combo.first == vkCode && (GetAsyncKeyState(combo.second) & 0x8000)) {
            return true;
        }
    }
    return false;
}
