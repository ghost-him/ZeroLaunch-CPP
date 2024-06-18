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
    Q_OBJECT
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

signals:
    void selectForwardItem();
    void selectBackwardItem();

    void launchSelectedProgram();

    void hideProgram();

private slots:
    void focusOnSearchBar();

protected:

    void keyPressEvent(QKeyEvent *event) override;

    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    SearchBar();

    void pressESC();
};

#endif // SEARCHBAR_H
