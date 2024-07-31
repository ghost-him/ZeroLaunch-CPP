#include "uicontroller.h"

#include "../ui/resultframe.h"
#include "../ui/searchbar.h"
#include "../model/database.h"
#include "keyboardhook.h"
#include "utils.h"
#include <QtSvg/QSvgRenderer>
#include <QPainter>

UIController::UIController() {
    SearchBar& searchBar = SearchBar::getInstance();
    ResultFrame& resultFrame = ResultFrame::getInstance();

    QObject::connect(&searchBar, &SearchBar::selectForwardItem, &resultFrame, &ResultFrame::selectForwardItem);
    QObject::connect(&searchBar, &SearchBar::selectBackwardItem, &resultFrame, &ResultFrame::selectBackwardItem);
    QObject::connect(&searchBar, &SearchBar::hideProgram, [this](){
        this->hideSearchBar();
    });

    // 初始化键盘钩子
    KeyboardHook& hook = KeyboardHook::getInstance();
    hook.startHook({}, {{ VK_SPACE, VK_MENU }});
    hook.setCallback([](){
        SearchBar& searchBar = SearchBar::getInstance();
        ResultFrame& resultFrame = ResultFrame::getInstance();

        searchBar.show();
        resultFrame.show();
    });
    // 初始化窗口监视器
    WindowHook& windowHook = WindowHook::getInstance();

    windowHook.setTargetWidget(&searchBar);
    windowHook.setCallback([](){
        SearchBar& searchBar = SearchBar::getInstance();
        ResultFrame& resultFrame = ResultFrame::getInstance();

        searchBar.hide();
        resultFrame.hide();
    });

    searchBar.show();
    resultFrame.show();
}

void UIController::initUI(const SettingWindowConfigure &configure)
{
    SettingWindow& settingWindow = SettingWindow::getInstance();

    settingWindow.initWindow(configure);


    resultItemNumber = configure.resultItemNumber;
    resultFrameEmptyText = configure.resultFrameEmptyText;
    SearchBar& searchBar = SearchBar::getInstance();
    searchBar.setPlaceholderText(configure.searchBarPlaceholderText);


    settingWindow.clearSearchList();
    for (const auto& i : configure.searchPaths) {
        settingWindow.addSearchListItem(i);
    }
    settingWindow.clearBannedList();
    for (const auto& i : configure.bannedPaths) {
        settingWindow.addBannedListItem(i);
    }
    settingWindow.clearKeyFilterTable();
    for (const auto& i : configure.keyFilters) {
        settingWindow.addkeyFilterItem(i.key, i.stableBias, i.note);
    }

    updateResultFrame(true);
}

void UIController::showSearchBar()
{
    hideSettingWindow();
    SearchBar& searchBar = SearchBar::getInstance();
    ResultFrame& resultFrame = ResultFrame::getInstance();
    searchBar.show();
    resultFrame.show();
}

void UIController::hideSearchBar()
{
    SearchBar& searchBar = SearchBar::getInstance();
    ResultFrame& resultFrame = ResultFrame::getInstance();
    searchBar.hide();
    resultFrame.hide();
}

void UIController::preLoadProgramIcon()
{
    clearIconCache();
    qDebug() << "开始预加载图标";
    initProgramIcon();
}

void UIController::initProgramIcon()
{
    Database& db = Database::getInstance();
    const auto& programs = db.getProgramsFile();
    int count = 0;
    for (int i = 0; i < programs.size(); i ++) {
        const ProgramNode& program = programs[i];
        getIcon(program.iconPath, program.isUWPApp);
        count ++;
    }
    qDebug() << "已成功加载：" << count ;
}

void UIController::clearIconCache()
{
    iconCache.clear();
}

void UIController::showSettingWindow()
{
    hideSearchBar();
    SettingWindow& settingWindow = SettingWindow::getInstance();
    settingWindow.show();
}

void UIController::hideSettingWindow() {
    SettingWindow& settingWindow = SettingWindow::getInstance();
    settingWindow.hide();
}

void UIController::refreshIndexedApp()
{
    SettingWindow& settingWindow {SettingWindow::getInstance()};
    Database& db {Database::getInstance()};

    settingWindow.clearIndexedAppTable();
    auto& files {db.getProgramsFile()};
    for (auto& i : files) {
        settingWindow.addIndexedAppItem(QString::fromStdWString(i.showName),
                                        i.isUWPApp,
                                        i.stableBias,
                                        QString::fromStdWString(i.programPath));
    }
    settingWindow.adjustIndexedAppTable();
}

const QPixmap &UIController::getIcon(const std::wstring &iconPath, bool isUWPApp)
{
    if (isUWPApp)
        return getIconWithABPath(iconPath);
    else
        return getIconWithEXE(iconPath);
}

QPixmap UIController::addMarginToPixmap(const QPixmap &originalPixmap, int margin)
{
    // 计算新的 QPixmap 的大小
    int newWidth = originalPixmap.width() + 2 * margin;
    int newHeight = originalPixmap.height() + 2 * margin;

    // 创建一个新的 QPixmap
    QPixmap newPixmap(newWidth, newHeight);
    newPixmap.fill(Qt::transparent); // 填充为透明色

    // 使用 QPainter 绘制原始的 QPixmap 到新的 QPixmap 中
    QPainter painter(&newPixmap);
    painter.drawPixmap(margin, margin, originalPixmap);

    return newPixmap;
}

const QPixmap &UIController::getIconWithEXE(const std::wstring &path)
{
    if (iconCache.contains(path)) {
        return iconCache[path];
    }
    QString str = QString::fromStdWString(path);
    QPixmap icon = addMarginToPixmap(getFileIcon(str), 10);

    iconCache[path] = std::move(icon);
    return iconCache[path];
}

const QPixmap &UIController::getIconWithABPath(const std::wstring &path)
{
    if (iconCache.contains(path)) {
        return iconCache[path];
    }
    QString Qpath = QString::fromStdWString(path);
    QPixmap icon(Qpath);
    iconCache[path] = std::move(icon);
    return iconCache[path];
}

void UIController::updateResultFrame(bool isEmptyText)
{
    ResultFrame& resultFrame = ResultFrame::getInstance();
    Database& db = Database::getInstance();


    resultFrame.clearItem();
    if (isEmptyText) {
        QIcon icon(":/icon/tips.svg");

        QPixmap pixmap(QSize(100, 100));
        pixmap = addMarginToPixmap(icon.pixmap(100, 100), 20);

        resultFrame.addItem(pixmap, resultFrameEmptyText);
        resultFrame.adjustSizeToFitItems();
        return ;
    }
    const std::vector<ProgramNode>& programs = db.getProgramsFile();
    for (int i = 0; i < resultItemNumber && i < programs.size(); i ++) {
        auto& programItem = programs[i];
        const std::wstring& programName = programItem.showName;
        const std::wstring& iconPath = programItem.iconPath;

        const QPixmap& programIcon = getIcon(iconPath, programItem.isUWPApp);
        QString str = QString::fromStdWString(programName);

        resultFrame.addItem(programIcon, str);
    }
    resultFrame.adjustSizeToFitItems();
    resultFrame.setCurrentItemIndex(0);
}
