#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void focusOutEvent(QFocusEvent *event) override ;
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
