#include "utils.h"
#include "qjsonobject.h"
#include <QApplication>
#include <QFile>

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

QJsonObject getDefaultSettingJson()
{
    QJsonObject jsonObject;

    jsonObject["isAutoStart"] = false;
    jsonObject["searchStartMenu"] = true;
    jsonObject["searchRegistry"] = false;
    jsonObject["searchProgramFile"] = false;
    jsonObject["preLoadResource"] = false;
    jsonObject["resultItemNumber"] = 4;
    jsonObject["searchBarPlaceholderText"] = "Hello, QuickLaunch!";
    jsonObject["resultFrameEmptyText"] = "当前搜索无结果";

    return jsonObject;
}

QString getConfigureFilePath()
{
    return QApplication::applicationDirPath() + "/configure";
}

QString getCustomDirectoryPath()
{
    return QApplication::applicationDirPath() + "/customProgramDir.txt";
}

QString getBannedDirectoryPath()
{
    return QApplication::applicationDirPath() + "/bannedProgramDir.txt";
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

    json["isAutoStart"] = config.isAutoStart;
    json["searchStartMenu"] = config.isSearchStartMenu;
    json["searchRegistry"] = config.isSearchRegistry;
    json["searchProgramFile"] = config.isSearchProgramFile;
    json["preLoadResource"] = config.isPreLoadResource;
    json["resultItemNumber"] = config.resultItemNumber;
    json["searchBarPlaceholderText"] = config.searchBarPlaceholderText;
    json["resultFrameEmptyText"] = config.resultFrameEmptyText;
    return json;
}


SettingWindowConfigure buildClassWithJson(const QJsonObject &json)
{
    SettingWindowConfigure ret;

    ret.isAutoStart = json["isAutoStart"].toBool();
    ret.isSearchStartMenu = json["searchStartMenu"].toBool();
    ret.isSearchRegistry = json["searchRegistry"].toBool();
    ret.isSearchProgramFile = json["searchProgramFile"].toBool();
    ret.isPreLoadResource = json["preLoadResource"].toBool();
    ret.resultItemNumber = json["resultItemNumber"].toInt();
    ret.searchBarPlaceholderText = json["searchBarPlaceholderText"].toString();
    ret.resultFrameEmptyText = json["resultFrameEmptyText"].toString();

    return ret;
}

QString getProgramVersion()
{
    return "QuickLaunch 0.2 alpha 2";
}
