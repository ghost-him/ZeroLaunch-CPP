#include "controller.h"
#include "initprogram.h"
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include "database.h"
#include "searchbar.h"
#include <QSettings>
#include <QJsonObject>
#include <QFile>
#include <QMessageBox>
#include <QJsonParseError>
#include <windows.h>
#include <shlobj.h>
#include <QApplication>

Controller::Controller() {}

void Controller::loadConfigure()
{
    filePath = QApplication::applicationDirPath() + "/configure";
    qDebug() <<filePath;
    // 检查文件是否存在
    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "File does not exist.";
        initConfigureFile();

    }

    // 打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open the file.";
        QMessageBox::critical(nullptr, "检测到错误", (QString)"请打开程序目录，删除configure文件后重新运行本程序, error:" + file.errorString());
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
        initConfigureFile();
        jsonDoc = QJsonDocument::fromJson(fileContent, &parseError);
    }
    // 正确的解析了文件，所以可以赋值
    configure = jsonDoc.object();
}

void Controller::initConfigureFile()
{
    QJsonObject jsonObject;

    jsonObject["isAutoStart"] = false;
    jsonObject["searchStartMenu"] = true;
    jsonObject["searchRegistry"] = false;
    jsonObject["searchProgramFile"] = false;
    jsonObject["preLoadResource"] = false;
    jsonObject["resultItemNumber"] = 4;
    jsonObject["searchBarPlaceholderText"] = "Hello, QuickLaunch!";

    QJsonDocument jsonDocument(jsonObject);

    QFile file(filePath);
    // 打开文件以进行写入
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(nullptr, "检测到错误", (QString)"请打开程序目录，删除configure文件后重新运行本程序, error:" + file.errorString());
        exit(1);
    }
    // 将JSON文档写入文件
    file.write(jsonDocument.toJson());
    // 关闭文件
    file.close();
}

void Controller::init()
{
    loadConfigure();
    //根据加载的配置来初始化程序

    InitProgram& init = InitProgram::getInstance();
    SearchBar::getInstance().initSettingWindow(configure);

    init.clearStore();

    init.initCustomPath();
    if (configure["searchStartMenu"].toBool()) {
        init.initProgramWithStartMenu();
    }
    if (configure["searchRegistry"].toBool()) {
        init.initProgramWithRegistery();
    }
    if (configure["searchProgramFile"].toBool()) {
        init.initProgramWithProgramFileDir();
    }
    if (configure["preLoadResource"].toBool()) {
        SearchBar::getInstance().initProgramIcon();
    }

    SearchBar::getInstance().setResultItemNumber(configure["resultItemNumber"].toInt());
    SearchBar::getInstance().setPlaceholderText(configure["searchBarPlaceholderText"].toString());
    setAutoStart(configure["isAutoStart"].toBool());

}

const std::vector<ProgramNode>& Controller::changedText(const QString &text)
{
    qDebug() << "change text";
    Database& db = Database::getInstance();
    db.updateProgramInfo(text.toStdWString());
    return db.getProgramsFile();
}

void Controller::runProgramWithIndex(int index) {

    Database& db = Database::getInstance();
    auto& programsFile = db.getProgramsFile();

    const std::wstring& programPath = programsFile[index].programPath;
    db.addLaunchTime(index);

    // Prepare the command line to run the program using cmd.exe
    std::wstring commandLine = L"cmd.exe /c start \"\" \"" + programPath + L"\"";

    // Log the command line to ensure it is correct
    qDebug() << "Attempting to run command: " << commandLine;

    // Prepare the STARTUPINFO structure
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Create a writable copy of the command line
    std::vector<wchar_t> commandLineBuffer(commandLine.begin(), commandLine.end());
    commandLineBuffer.push_back(0);

    // Extract the directory part of the path using std::filesystem
    std::filesystem::path path(programPath);
    std::wstring workingDirectory = path.parent_path().wstring();

    // Create the process
    if (!CreateProcess(
            NULL,           // Module name (use command line directly)
            commandLineBuffer.data(), // Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,          // Set handle inheritance to FALSE
            CREATE_NO_WINDOW | DETACHED_PROCESS, // No console window, detached process
            NULL,           // Use parent's environment block
            workingDirectory.c_str(),           // Use parent's starting directory
            &si,            // Pointer to STARTUPINFO structure
            &pi)            // Pointer to PROCESS_INFORMATION structure
        ) {
        qDebug() << "Failed to create process. Error: " << GetLastError();
    } else {
        // Close process and thread handles to ensure they are completely detached
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        qDebug() << "Process created successfully: " << commandLine;
    }
}
void Controller::setAutoStart(bool isAutoStart)
{
    QString  strApplicationName = QApplication::applicationName();//获取应用名称
    qDebug() << strApplicationName;
    QSettings * settings = new QSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

    if(isAutoStart)
    {
        QString strApplicationFilePath = QApplication::applicationFilePath();//获取应用的目录
        settings->setValue(strApplicationName, strApplicationFilePath.replace("/", "\\"));//写入注册表
    }
    else
        settings->remove(strApplicationName);//移除注册表
}

const QJsonObject &Controller::getConfigure()
{
    return configure;
}

void Controller::saveConfigure(const QJsonObject& json)
{
    QJsonDocument jsonDocument(json);

    QFile file(filePath);
    // 打开文件以进行写入
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(nullptr, "检测到错误", "请打开程序目录，删除configure文件后重新运行本程序");
        exit(1);
    }
    // 将JSON文档写入文件
    file.write(jsonDocument.toJson());
    // 关闭文件
    file.close();
}


