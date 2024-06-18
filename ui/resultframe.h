#ifndef RESULTFRAME_H
#define RESULTFRAME_H

#include <QWidget>

#include <unordered_map>
#include <mutex>

namespace Ui {
class ResultFrame;
}

class ResultFrame : public QWidget
{
    Q_OBJECT

public:

    // 删除拷贝构造函数和拷贝赋值操作符
    ResultFrame(ResultFrame const&) = delete;
    ResultFrame& operator=(ResultFrame const&) = delete;
    // 提供一个全局唯一的接口
    static ResultFrame& getInstance() {
        static ResultFrame instance;
        return instance;
    }

    void setCurrentItemIndex(int row);

    int getCurrentItemIndex();

    void clearItem();

    void addItem(const QPixmap& programIcon, const QString& programName);

    void selectForwardItem();

    void selectBackwardItem();

    void show();

    // 调整大小以匹配项目
    void adjustSizeToFitItems();

private:
    ResultFrame(QWidget *parent = nullptr);
    ~ResultFrame();

    Ui::ResultFrame *ui;
};

#endif // RESULTFRAME_H
