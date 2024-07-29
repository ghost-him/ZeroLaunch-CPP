#ifndef UTILS_H
#define UTILS_H

#include <QPixmap>
#include <QImage>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <QDebug>
#include <vector>

struct SettingWindowConfigure {
    struct KeyFilter {
        QString key;
        double stableBias;
        QString note;
    };

    bool isSearchStartMenu;
    bool isAutoStart;
    bool isPreLoadResource;
    bool isSearchProgramFile;
    bool isSearchRegistry;
    int resultItemNumber;
    QString searchBarPlaceholderText;
    QString resultFrameEmptyText;
    bool isSearchUWP;
    std::vector<QString> searchPaths;
    std::vector<QString> bannedPaths;
    std::vector<KeyFilter> keyFilters;
};


HICON ExtractIconFromFile(const QString& filePath);

QPixmap getFileIcon(const QString& filePath);

QJsonObject getDefaultConfigJson();

QString getConfigPath();

QString getPinyinConfigPath();

void createFile(const QString &path, const QString &defaultContent = "");

QJsonObject buildJsonWithClass(const SettingWindowConfigure& config);

SettingWindowConfigure buildClassWithJson(const QJsonObject& json);

QString getProgramVersion();

QString GetShellDirectory(int type);

QString getDefaultItemPlaceHolder();

// 创建一个用于表示关键字过滤器的json对象
QJsonObject newKeyFilterObject(const QString& key, double stableBias, const QString& note);

QJsonObject newSearchItem(const QString& searchPath);

QJsonObject newBannedItem(const QString& bannedPath);

#endif // UTILS_H
