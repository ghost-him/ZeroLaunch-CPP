#ifndef RESULTFRAME_H
#define RESULTFRAME_H

#include <QWidget>
#include "resultitem.h"
#include <unordered_map>
#include <mutex>

namespace Ui {
class ResultFrame;
}

class ResultFrame : public QWidget
{
    Q_OBJECT

public:

    explicit ResultFrame(QWidget *parent = nullptr);
    ~ResultFrame();

    void updateInputText(const QString& inputText);

    void clearItem();

    void addItem(const QPixmap& programIcon, const QString& programName);

    const QPixmap& getIconWithPath(const std::wstring& path);

    void selectForwardItem();

    void selectBackwardItem();

    void confirmCurrentItem();

    void focusOutEvent(QFocusEvent *event) override;

    void initProgramIcon();

    void setResultItemNumber(int number);

    void clearIconCache();

private:
    int resultItemNumber;
    std::mutex cacheLock;

    // 调整大小以匹配项目
    void adjustSizeToFitItems();

    Ui::ResultFrame *ui;
    std::unordered_map<std::wstring, QPixmap> iconCache;
};

#endif // RESULTFRAME_H
