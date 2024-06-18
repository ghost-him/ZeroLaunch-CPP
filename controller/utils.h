#ifndef UTILS_H
#define UTILS_H

#include <QPixmap>
#include <QImage>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <QDebug>

struct SettingWindowConfigure {
    bool isSearchStartMenu;
    bool isAutoStart;
    bool isPreLoadResource;
    bool isSearchProgramFile;
    bool isSearchRegistry;
    int resultItemNumber;
    QString searchBarPlaceholderText;
    QString resultFrameEmptyText;
};


HICON ExtractIconFromFile(const QString& filePath);

QPixmap getFileIcon(const QString& filePath);

QJsonObject getDefaultSettingJson();

QString getConfigureFilePath();

QString getCustomDirectoryPath();

QString getBannedDirectoryPath();

void createFile(const QString &path, const QString &defaultContent);

QJsonObject buildJsonWithClass(const SettingWindowConfigure& config);

SettingWindowConfigure buildClassWithJson(const QJsonObject& json);

QString getProgramVersion();

#endif // UTILS_H
