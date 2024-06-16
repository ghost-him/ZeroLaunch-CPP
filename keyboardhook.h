#ifndef KEYBOARDHOOK_H
#define KEYBOARDHOOK_H

#include <QObject>
#include <Windows.h>
#include <QMessageBox>
class KeyboardHook : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardHook(QObject *parent = nullptr);
    ~KeyboardHook();

    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static KeyboardHook* instance();

signals:
    void keyPressed(int keyCode);
    void altSpacePressed();

private:
    HHOOK hook;

};

#endif // KEYBOARDHOOK_H
