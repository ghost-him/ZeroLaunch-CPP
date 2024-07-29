#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QString>
#include "../model/database.h"
#include "../ui/settingwindow.h"
#include <QJsonObject>
#include "uicontroller.h"
class Controller
{
public:
    // 删除拷贝构造函数和拷贝赋值操作符
    Controller(Controller const&) = delete;
    Controller& operator=(Controller const&) = delete;
    // 提供一个全局唯一的接口
    static Controller& getInstance() {
        static Controller instance;
        return instance;
    }

    void init();

    void launchSelectedProgram();

    void setAutoStart(bool isAutoStart);

    const QJsonObject& getConfigure();

    void runProgramWithIndex(int index);

    void inputText(const QString& text);

    ~Controller() = default;
private:
    UIController uiController;

    Controller();

    void loadConfigFile();

    void initConfigFile();

    bool startProcessWithElevation(const std::wstring& programPath, const std::wstring& workingDirectory);

    bool startProcessNormally(const std::wstring& programPath, const std::wstring& workingDirectory);

    void saveSetting(SettingWindowConfigure configure);

    void refreshIndexedApp();

    QJsonObject configure;
};

#endif // CONTROLLER_H
