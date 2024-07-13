#include "singleapplication.h"

SingleApplication::SingleApplication(int &argc, char *argv[]) : QApplication(argc, argv), sharedMemory("ZeroLaunchKey") {

    // 创建一个系统信号量
    QSystemSemaphore semaphore("ZeroLaunchSemaphoreKey", 1);
    semaphore.acquire();
    // 尝试将共享内存附加到进程
    if (sharedMemory.attach())
    {
        isRun = true;
    }
    else
    {
        // 如果共享内存无法附加，则创建它
        sharedMemory.create(1);
        isRun = false;
    }
    semaphore.release();
}

bool SingleApplication::isRunning()
{
    return isRun;
}
