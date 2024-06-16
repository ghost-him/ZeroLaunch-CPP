#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QApplication>
#include <QSharedMemory>
#include <QSystemSemaphore>

class SingleApplication : public QApplication
{
public:
    SingleApplication(int &argc, char *argv[]);

    bool isRunning();

private:
    QSharedMemory sharedMemory;
    bool isRun;
};

#endif // SINGLEAPPLICATION_H
