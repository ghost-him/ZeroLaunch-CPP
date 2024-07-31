#include "controller.h"
#include "initprogram.h"
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include "../model/database.h"
#include "../ui/searchbar.h"
#include <QSettings>
#include <QJsonObject>
#include <QFile>
#include <QMessageBox>
#include <QJsonParseError>
#include <windows.h>
#include <shlobj.h>
#include <QApplication>
#include "../controller/utils.h"
#include "../ui/settingwindow.h"
#include "../model/chineseconvertpinyin.h"
#include "uwpapp.h"

Controller::Controller() {
    SearchBar& searchBar = SearchBar::getInstance();
    SettingWindow& settingWindow = SettingWindow::getInstance();
    ResultFrame& resultFrame = ResultFrame::getInstance();

    QObject::connect(&settingWindow, &SettingWindow::confirmSetting, [this](SettingWindowConfigure configure){
        this->saveSetting(configure);
    });

    QObject::connect(&searchBar, &QLineEdit::textChanged, [this](const QString& inputText){
        this->inputText(inputText);
    });

    QObject::connect(&searchBar, &SearchBar::launchSelectedProgram, [this](){
        this->launchSelectedProgram();
    });

    QObject::connect(&resultFrame, &ResultFrame::sg_launchSelectedProgram, [this](){
        this->launchSelectedProgram();
    });

    QObject::connect(&settingWindow, &SettingWindow::sg_refreshIndexedApp, [this](){
        this->refreshIndexedApp();
    });

    autoReloadConfig = std::make_unique<QTimer>();
    QObject::connect(autoReloadConfig.get(), &QTimer::timeout, [this](){
        qDebug() << "auto reload config";
        this->init();
    });
}

void Controller::loadConfigFile()
{
    QString filePath = getConfigPath();
    // 检查文件是否存在
    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "File does not exist.";
        initConfigFile();
    }
    // 打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open the file.";
        QMessageBox::critical(nullptr, "检测到错误", (QString)"请打开程序目录，删除[Config.json]文件后重新运行本程序, error:" + file.errorString());
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
        initConfigFile();
        jsonDoc = QJsonDocument::fromJson(fileContent, &parseError);
    }
    // 正确的解析了文件，所以可以赋值
    configure = jsonDoc.object();
}

void Controller::initConfigFile()
{

    QJsonObject config = getDefaultConfigJson();
    QString configPath = getConfigPath();
    QJsonDocument configDoc(config);
    QFile configFile(configPath);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(nullptr, "检测到错误", (QString)"请打开程序目录，删除[Config.json]文件后重新运行本程序, error:" + configFile.errorString());
        exit(1);
    }
    configFile.write(configDoc.toJson());
    configFile.close();
}

bool Controller::startProcessWithElevation(const std::wstring &programPath, const std::wstring &workingDirectory)
{
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = L"runas"; // Request elevation
    sei.lpFile = programPath.c_str();
    sei.lpDirectory = workingDirectory.c_str();
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei)) {
        DWORD error = GetLastError();
        if (error == ERROR_CANCELLED) {
            qDebug() << "User declined the elevation request.";
        } else {
            qDebug() << "Failed to start process with elevation. Error: " << error;
        }
        return false;
    }
    return true;
}

bool Controller::startProcessNormally(const std::wstring &programPath, const std::wstring &workingDirectory)
{
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = nullptr; // Use default verb
    sei.lpFile = programPath.c_str();
    sei.lpDirectory = workingDirectory.c_str();
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei)) {
        DWORD error = GetLastError();
        qDebug() << "Failed to start process normally. Error: " << error;
        return false;
    }
    return true;
}

void Controller::init()
{
    loadConfigFile();
    //根据加载的配置来初始化程序

    InitProgram& init = InitProgram::getInstance();
    Database& db = Database::getInstance();

    SettingWindowConfigure config = buildClassWithJson(configure);

    uiController.initUI(config);

    ChineseConvertPinyin& pinyin = ChineseConvertPinyin::getInstance();
    pinyin.init();

    init.clearStore();

    for (const auto& i : config.keyFilters) {
        db.addKeyFilter(i.key.toStdWString(), i.stableBias);
    }

    init.initCustomPath(config.bannedPaths, config.searchPaths);

    if (config.isSearchUWP) {
        init.initUWPProgram();
    }
    if (config.isSearchStartMenu) {
        init.initProgramWithStartMenu();
    }
    if (config.isSearchRegistry) {
        init.initProgramWithRegistery();
    }
    if (config.isSearchProgramFile) {
        init.initProgramWithProgramFileDir();
    }
    if (config.isPreLoadResource) {
        uiController.preLoadProgramIcon();
    }

    setAutoStart(config.isAutoStart);

    refreshIndexedApp();
    // x min = x * 60 * 1000 ms
    qDebug() << "start count time: " << config.autoReloadTime;
    autoReloadConfig->start(config.autoReloadTime * 1000 * 60);
}

void Controller::launchSelectedProgram()
{
    ResultFrame& resultFrame = ResultFrame::getInstance();
    int index = resultFrame.getCurrentItemIndex();

    if (SearchBar::getInstance().text().isEmpty()) {
        uiController.hideSearchBar();
    } else {
        uiController.hideSearchBar();
        runProgramWithIndex(index);
    }
}

void Controller::runProgramWithIndex(int index) {
    Database& db = Database::getInstance();
    auto& programsFile = db.getProgramsFile();

    const std::wstring& programPath = programsFile[index].programPath;
    bool isUWPApp = programsFile[index].isUWPApp;

    db.addLaunchTime(index);
    qDebug() << "Attempting to run program: " << QString::fromStdWString(programPath);

    if (isUWPApp) {
        // 如果当前的程序是UWP应用
        UWPAppManager& app = UWPAppManager::getInstance();
        app.lunchUWPApp(programPath);
        return;
    }
    // 普通应用的启动方式
    std::filesystem::path path(programPath);
    std::wstring workingDirectory = path.parent_path().wstring();
    if (!startProcessNormally(programPath, workingDirectory)) {
        qDebug() << "Attempting to run process with elevation.";
        if (!startProcessWithElevation(programPath, workingDirectory)) {
            qDebug() << "Failed to start process even with elevation.";
        }
    }
}

void Controller::inputText(const QString &text)
{
    if (!text.isEmpty()) {
        Database& db = Database::getInstance();
        db.updateScores(text.toStdWString());
    }
    uiController.updateResultFrame(text.isEmpty());
}

void Controller::showSettingWindow()
{
    uiController.showSettingWindow();
}
void Controller::setAutoStart(bool isAutoStart)
{
    QString strApplicationName = QApplication::applicationName(); // 获取应用名称
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

    if (isAutoStart)
    {
        QString strApplicationFilePath = QApplication::applicationFilePath(); // 获取应用的目录
        strApplicationFilePath.replace("/", "\\"); // 替换路径中的斜杠

        // 检查是否已经存在相同的注册表项，避免重复添加
        if (settings.value(strApplicationName).toString() != strApplicationFilePath)
        {
            settings.setValue(strApplicationName, strApplicationFilePath); // 写入注册表
        }
    }
    else
    {
        settings.remove(strApplicationName); // 移除注册表
    }
}

const QJsonObject &Controller::getConfigure()
{
    return configure;
}

void Controller::saveSetting(SettingWindowConfigure configure)
{
    QJsonObject json;

    json = buildJsonWithClass(configure);

    QJsonDocument jsonDocument(json);

    QString filePath = getConfigPath();

    QFile file(filePath);
    // 打开文件以进行写入
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(nullptr, "检测到错误", "请打开程序目录，删除[Config.json]文件后重新运行本程序");
        exit(1);
    }
    // 将JSON文档写入文件
    file.write(jsonDocument.toJson());
    // 关闭文件
    file.close();

    // 保存好后，重新加载配置
    init();
}

void Controller::refreshIndexedApp()
{
    uiController.refreshIndexedApp();
}


