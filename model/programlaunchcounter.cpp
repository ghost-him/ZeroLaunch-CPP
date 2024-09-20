#include "programlaunchcounter.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QMessageBox>

ProgramLaunchCounter::ProgramLaunchCounter() {
    this->recentCount = 20;
    this->isInited = false;
}

size_t ProgramLaunchCounter::queryTotalOpenCount(const std::wstring &programName)
{
    if (!isEnable) {
        return 0;
    }
    if (!totalOpenCount.contains(programName)) {
        return 0;
    }
    return totalOpenCount[programName];
}

size_t ProgramLaunchCounter::queryRecentOpenCount(const std::wstring &programName)
{
    if (!isEnable) {
        return 0;
    }
    if (!recentOpenCount.contains(programName)) {
        return 0;
    }
    return recentOpenCount[programName];
}

void ProgramLaunchCounter::openProgramName(const std::wstring &programName)
{
    totalOpenCount[programName] ++;
    recentOpenCount[programName] ++;
    if (recentOpenProgram.size() >= recentCount) {
        recentOpenCount[recentOpenProgram.front()] --;
        recentOpenProgram.pop_front();
    }
    recentOpenProgram.push_back(programName);
}

void ProgramLaunchCounter::save()
{
    if (!isEnable) {
        initStatisticJson();
    }

    QJsonObject obj;
    {
        // 存储总次数
        QJsonArray arr;
        for (const auto& item : totalOpenCount) {
            const std::wstring& programName = item.first;
            const int& count = item.second;
            QJsonObject countItem;

            countItem["programName"] = QString::fromStdWString(programName);
            countItem["count"] = count;
            arr.push_back(countItem);
        }

        obj["totalOpenCount"] = arr;
    }

    {
        QJsonArray arr;
        for (const auto& item : recentOpenProgram) {
            QJsonObject countItem;
            countItem["programName"] = QString::fromStdWString(item);
            arr.push_back(countItem);
            qDebug() << "save : " << item;
        }
        obj["recentOpenProgram"] = arr;
    }

    QFile file(getStatisticJsonPath());
    QJsonDocument doc(obj);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(nullptr, "检测到错误", (QString)"请打开程序目录，删除[statictic.json]文件后重新运行本程序, error:" + file.errorString());
        exit(1);
    }
    file.write(doc.toJson());
    file.close();
}

void ProgramLaunchCounter::load(const QJsonObject &jsonData)
{
    {
        // 加载总次数统计
        QJsonArray arr;
        arr = jsonData["totalOpenCount"].toArray();
        for (const auto& countItem : arr) {
            QJsonObject item = countItem.toObject();
            std::wstring programName = item["programName"].toString().toStdWString();
            int count = item["count"].toInt();
            totalOpenCount[programName] = count;
        }
    }
    {
        // 加载最近打开次数统计
        QJsonArray arr;
        arr = jsonData["recentOpenProgram"].toArray();
        for (const auto& countItem : arr) {
            QJsonObject item = countItem.toObject();
            std::wstring programName = item["programName"].toString().toStdWString();
            qDebug() << "load : " << programName;
            recentOpenProgram.push_back(programName);
        }
        while(recentOpenProgram.size() >= recentCount) {
            recentOpenProgram.pop_front();
        }
        for (const auto& i : recentOpenProgram) {
            recentOpenCount[i] ++;
        }
    }
}

void ProgramLaunchCounter::setRecentCount(size_t count)
{
    this->recentCount = count;
}

void ProgramLaunchCounter::init()
{
    // 初始化
    if (isInited)
        return;
    isInited = true;


    QString jsonPath = getStatisticJsonPath();
    QFile file(jsonPath);

    if (!file.exists()) {
        initStatisticJson();
    }
    // 打开文件
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open the file.";
        QMessageBox::critical(nullptr, "检测到错误", (QString)"请打开程序目录，删除[statictic.json]文件后重新运行本程序, error:" + file.errorString());
        exit(1);
    }

    // 读取文件内容
    QByteArray fileContent = file.readAll();
    file.close();

    // 解析JSON
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileContent, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Invalid JSON file:" << parseError.errorString();
        initStatisticJson();
        jsonDoc = QJsonDocument::fromJson(fileContent, &parseError);
    }
    load(jsonDoc.object());
}

void ProgramLaunchCounter::enableStatistic(bool isEnable)
{
    this->isEnable = isEnable;
}

void ProgramLaunchCounter::initStatisticJson()
{
    QJsonObject config = getDefaultStaticticjson();
    QString jsonPath = getStatisticJsonPath();
    QJsonDocument configDoc(config);
    QFile configFile(jsonPath);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(nullptr, "检测到错误", (QString)"请打开程序目录，删除[statictic.json]文件后重新运行本程序, error:" + configFile.errorString());
        exit(1);
    }
    configFile.write(configDoc.toJson());
    configFile.close();
}
