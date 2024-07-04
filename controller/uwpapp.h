#ifndef UWPAPPMANAGER_H
#define UWPAPPMANAGER_H

#include <string>
#include <vector>
#include <QString>

struct UWP {
    std::wstring name;
    std::wstring appID;
    std::wstring picturePath;
};


class UWPAppManager
{
public:
    // 删除拷贝构造函数和拷贝赋值操作符
    UWPAppManager(UWPAppManager const&) = delete;
    UWPAppManager& operator=(UWPAppManager const&) = delete;
    // 提供一个全局唯一的接口
    static UWPAppManager& getInstance() {
        static UWPAppManager instance;
        return instance;
    }

    std::vector<UWP> getUWPApp();

    void lunchUWPApp(const std::wstring appID);

private:

    QString validateIconPath(const QString& iconPath);

    UWPAppManager();


};

#endif // UWPAPPMANAGER_H
