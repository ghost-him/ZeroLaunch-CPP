#ifndef RESULTITEM_H
#define RESULTITEM_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class ResultItem;
}

class ResultItem : public QWidget
{
    Q_OBJECT

public:
    explicit ResultItem(QWidget *parent = nullptr);
    ~ResultItem();

    void setProgramIcon(const QPixmap &pixmap);
    void setProgramName(const QString &name);

private:
    Ui::ResultItem *ui;
};

#endif // RESULTITEM_H
