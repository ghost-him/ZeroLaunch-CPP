#include "utils.h"

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
QPixmap GetFileIcon(const QString& filePath) {
    HICON hIcon = ExtractIconFromFile(filePath);
    if (hIcon) {
        QImage image = QImage::fromHICON(hIcon);
        DestroyIcon(hIcon);  // Clean up the HICON after use
        return QPixmap::fromImage(image);
    }
    return QPixmap(":/icon/tips.svg");
}
