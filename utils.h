#ifndef UTILS_H
#define UTILS_H

#include <QPixmap>
#include <QImage>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <QDebug>

HICON ExtractIconFromFile(const QString& filePath);

QPixmap GetFileIcon(const QString& filePath);

#endif // UTILS_H
