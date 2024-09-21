#include "utils.h"
#include "qjsonobject.h"
#include <QApplication>
#include <QFile>
#include <QJsonArray>

// Helper function to extract HICON from a file
HICON ExtractIconFromFile(const QString& filePath) {
    WCHAR szFilePath[MAX_PATH];
    filePath.toWCharArray(szFilePath);
    szFilePath[filePath.length()] = 0;

    SHFILEINFO shFileInfo;
    if (SHGetFileInfo(szFilePath, 0, &shFileInfo, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_LARGEICON)) {
        return shFileInfo.hIcon;
    }
    return nullptr;
}

// Function to get QPixmap from exe, url, or lnk file
QPixmap getFileIcon(const QString& filePath) {
    HICON hIcon = ExtractIconFromFile(filePath);
    if (hIcon) {
        QImage image = QImage::fromHICON(hIcon);
        DestroyIcon(hIcon);  // Clean up the HICON after use
        return QPixmap::fromImage(image);
    }
    return QPixmap(":/icon/tips.svg");
}

QJsonObject getDefaultConfigJson() {
    QJsonObject jsonObject;
    jsonObject["configVersion"] = getConfigVersion();
    jsonObject["isAutoStart"] = false;
    jsonObject["searchStartMenu"] = true;
    jsonObject["searchRegistry"] = false;
    jsonObject["searchProgramFile"] = false;
    jsonObject["preLoadResource"] = false;
    jsonObject["isEnableStatictics"] = false;
    jsonObject["resultItemNumber"] = 4;
    jsonObject["autoReloadTime"] = 30;
    jsonObject["searchBarPlaceholderText"] = "Hello, ZeroLaunch!";
    jsonObject["resultFrameEmptyText"] = "当前搜索无结果";
    jsonObject["searchUWP"] = true;
    jsonObject["isSlientBoot"] = false;

    QJsonArray searchItems;
    QJsonArray bannedItems;

    QJsonObject searchItem = newSearchItem(getDefaultItemPlaceHolder());
    QJsonObject bannedItem = newBannedItem(getDefaultItemPlaceHolder());

    searchItems.push_back(searchItem);
    bannedItems.push_back(bannedItem);

    jsonObject["searchPaths"] = searchItems;
    jsonObject["bannedPaths"] = bannedItems;

    QJsonArray keyFilterItems;
    QJsonObject keyFilter1 = newKeyFilterObject("卸载", -5000, "忽略卸载程序");
    QJsonObject keyFilter2 = newKeyFilterObject("uninstall", -5000, "忽略卸载程序");

    keyFilterItems.push_back(keyFilter1);
    keyFilterItems.push_back(keyFilter2);

    jsonObject["keyFilters"] = keyFilterItems;
    return jsonObject;
}

QString getConfigPath()
{
    return QApplication::applicationDirPath() + "/Config.json";
}

void createFile(const QString &path, const QString &defaultContent) {
    if (std::filesystem::exists(path.toStdWString()))
        return ;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件进行写入";
    }
    QTextStream out(&file);
    // 写入字符串到文件
    out << defaultContent;

    file.close();
}

QJsonObject buildJsonWithClass(const SettingWindowConfigure& config) {
    QJsonObject json;
    json["configVersion"] = config.configVersion;
    json["isAutoStart"] = config.isAutoStart;
    json["searchStartMenu"] = config.isSearchStartMenu;
    json["searchRegistry"] = config.isSearchRegistry;
    json["searchProgramFile"] = config.isSearchProgramFile;
    json["preLoadResource"] = config.isPreLoadResource;
    json["resultItemNumber"] = config.resultItemNumber;
    json["searchBarPlaceholderText"] = config.searchBarPlaceholderText;
    json["resultFrameEmptyText"] = config.resultFrameEmptyText;
    json["searchUWP"] = config.isSearchUWP;
    json["autoReloadTime"] = config.autoReloadTime;
    json["isEnableStatictics"] = config.isEnableStatictics;
    json["isSlientBoot"] = config.isSlientBoot;

    QJsonArray bannedItems;
    for (const auto& i : config.bannedPaths) {
        QJsonObject obj = newBannedItem(i);
        bannedItems.push_back(std::move(obj));
    }
    json["bannedPaths"] = bannedItems;

    QJsonArray searchItems;
    for (const auto& i : config.searchPaths) {
        QJsonObject obj = newSearchItem(i);
        searchItems.push_back(std::move(obj));
    }
    json["searchPaths"] = searchItems;

    QJsonArray keyFilterItems;
    for (const auto& i : config.keyFilters) {
        QJsonObject obj = newKeyFilterObject(i.key, i.stableBias, i.note);
        keyFilterItems.push_back(std::move(obj));
    }
    json["keyFilters"] = keyFilterItems;

    return json;
}


SettingWindowConfigure buildClassWithJson(const QJsonObject &json)
{
    SettingWindowConfigure ret;
    ret.configVersion = json["configVersion"].toInt();
    ret.isAutoStart = json["isAutoStart"].toBool();
    ret.isSearchStartMenu = json["searchStartMenu"].toBool();
    ret.isSearchRegistry = json["searchRegistry"].toBool();
    ret.isSearchProgramFile = json["searchProgramFile"].toBool();
    ret.isPreLoadResource = json["preLoadResource"].toBool();
    ret.resultItemNumber = json["resultItemNumber"].toInt();
    ret.searchBarPlaceholderText = json["searchBarPlaceholderText"].toString();
    ret.resultFrameEmptyText = json["resultFrameEmptyText"].toString();
    ret.isSearchUWP = json["searchUWP"].toBool();
    ret.autoReloadTime = json["autoReloadTime"].toInt();
    ret.isEnableStatictics = json["isEnableStatictics"].toBool();
    ret.isSlientBoot = json["isSlientBoot"].toBool();

    QJsonArray bannedItems = json["bannedPaths"].toArray();
    for (const auto& i : bannedItems) {
        ret.bannedPaths.push_back(i.toObject()["bannedPath"].toString());
    }

    QJsonArray searchItems = json["searchPaths"].toArray();
    for (const auto& i : searchItems) {
        ret.searchPaths.push_back(i.toObject()["searchPath"].toString());
    }

    QJsonArray keyFilterItems = json["keyFilters"].toArray();
    for (const auto& i : keyFilterItems) {
        QJsonObject obj = i .toObject();
        SettingWindowConfigure::KeyFilter keyFilter {obj["key"].toString(), obj["stableBias"].toDouble(), obj["note"].toString()};
        ret.keyFilters.push_back(std::move(keyFilter));
    }

    return ret;
}

int getConfigVersion()
{
    return 1;
}

QString getProgramVersion()
{
    return "ZeroLaunch 0.8.1";
}

QString getPinyinConfigPath()
{
    return QApplication::applicationDirPath() + "/pinyin.json";
}

QString GetShellDirectory(int type) {
    wchar_t buffer[_MAX_PATH];
    SHGetFolderPathW(NULL, type, NULL, 0, buffer);
    return QString::fromWCharArray(buffer);
}

QString getDefaultItemPlaceHolder() {
    return "[在此输入目标路径]";
}

QJsonObject newKeyFilterObject(const QString &key, double stableBias, const QString &note)
{
    QJsonObject ret;
    ret["key"] = key;
    ret["stableBias"] = stableBias;
    ret["note"] = note;
    return ret;
}

QJsonObject newSearchItem(const QString &searchPath)
{
    QJsonObject ret;
    ret["searchPath"] = searchPath;
    return ret;
}

QJsonObject newBannedItem(const QString &bannedPath)
{
    QJsonObject ret;
    ret["bannedPath"] = bannedPath;
    return ret;
}

QString getStatisticJsonPath()
{
    return QApplication::applicationDirPath() + "/statictic.json";
}

QJsonObject getDefaultStaticticjson()
{
    QJsonObject obj;
    QJsonArray arr1, arr2;
    obj["totalOpenCount"] = arr1;
    obj["recentOpenProgram"] = arr2;
    return obj;
}
