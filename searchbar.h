#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QPainter>
#include <QStyleOption>
#include <QKeyEvent>
#include "resultframe.h"
#include <windows.h>
#include <QTimer>
#include "settingwindow.h"

class SearchBar : public QLineEdit
{
public:
    // 删除拷贝构造函数和拷贝赋值操作符
    SearchBar(SearchBar const&) = delete;
    SearchBar& operator=(SearchBar const&) = delete;
    // 提供一个全局唯一的接口
    static SearchBar& getInstance() {
        static SearchBar instance;
        return instance;
    }

    void show();
    void hide();

    void initProgramIcon();
    void initSettingWindow(const QJsonObject& json);

    void setResultItemNumber(int number);

    const SettingWindow& getSettingWindow();

private slots:
    void focusOnSearchBar();

protected:

    void keyPressEvent(QKeyEvent *event) override {
        // 检查按下的键是否为 'j' 且 Ctrl 键是否按下
        if (event->key() == Qt::Key_J && event->modifiers() & Qt::ControlModifier) {
            qDebug() << "同时按下了ctrl + j";
            resultFrame.selectForwardItem();

        }else if(event->key() == Qt::Key_K && event->modifiers() & Qt::ControlModifier){
            qDebug() << "同时按下了ctrl + k";
            resultFrame.selectBackwardItem();
        }else if(event->key() == Qt::Key_Up){
            qDebug() << "按下了上";
            resultFrame.selectBackwardItem();
        }else if(event->key() == Qt::Key_Down){
            qDebug() << "按下了下";
            resultFrame.selectForwardItem();
        }else if(event->key() == Qt::Key_Escape){
            qDebug() << "按下了esc";
            pressESC();
        }else if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
            qDebug() << "按下了enter";
            pressEnter();
        } else {
            // 调用父类的 keyPressEvent 方法以确保正常处理其他按键
            QLineEdit::keyPressEvent(event);
        }
    }

    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override
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

private:

    SearchBar();
    ResultFrame resultFrame;
    SettingWindow settingWindow;

    void pressESC();

    void pressEnter();
};

#endif // SEARCHBAR_H
