#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QString>
#include "database.h"

#include <QJsonObject>
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

    const std::vector<ProgramNode>& changedText(const QString& text);

    void runProgramWithIndex(int index);

    void setAutoStart(bool isAutoStart);

    const QJsonObject& getConfigure();

    void saveConfigure(const QJsonObject& json);

private:
    QString filePath;

    Controller();
    void loadConfigure();

    void initConfigureFile();

    bool startProcessWithElevation(const std::wstring& programPath, const std::wstring& workingDirectory);

    bool startProcessNormally(const std::wstring& programPath, const std::wstring& workingDirectory);


    QJsonObject configure;
};

#endif // CONTROLLER_H
