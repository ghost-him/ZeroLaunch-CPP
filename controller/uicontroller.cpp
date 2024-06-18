#include "uicontroller.h"

#include "../ui/resultframe.h"
#include "../ui/searchbar.h"
#include "../model/database.h"
#include "keyboardhook.h"
#include "utils.h"

UIController::UIController() {
    SearchBar& searchBar = SearchBar::getInstance();
    ResultFrame& resultFrame = ResultFrame::getInstance();

    QObject::connect(&searchBar, &SearchBar::selectForwardItem, &resultFrame, &ResultFrame::selectForwardItem);
    QObject::connect(&searchBar, &SearchBar::selectBackwardItem, &resultFrame, &ResultFrame::selectBackwardItem);
    QObject::connect(&searchBar, &SearchBar::hideProgram, [this](){
        this->hide();
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

    updateResultFrame(true);
}

void UIController::show()
{
    SearchBar& searchBar = SearchBar::getInstance();
    ResultFrame& resultFrame = ResultFrame::getInstance();
    searchBar.show();
    resultFrame.show();
}

void UIController::hide()
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
        const std::wstring& programPath = program.programPath;
        getIconWithPath(programPath);
        count ++;
    }
    qDebug() << "已成功加载：" << count ;
}

void UIController::clearIconCache()
{
    iconCache.clear();
}

const QPixmap &UIController::getIconWithPath(const std::wstring &path)
{
    if (iconCache.contains(path)) {
        return iconCache[path];
    }
    QString str = QString::fromStdWString(path);
    QPixmap icon = getFileIcon(str);

    iconCache[path] = std::move(icon);
    return iconCache[path];
}


void UIController::updateResultFrame(bool isEmptyText)
{
    ResultFrame& resultFrame = ResultFrame::getInstance();
    Database& db = Database::getInstance();


    resultFrame.clearItem();
    if (isEmptyText) {
        QPixmap pixmap(":/icon/tips.svg");
        resultFrame.addItem(pixmap, resultFrameEmptyText);
        resultFrame.adjustSizeToFitItems();
        return ;
    }
    const std::vector<ProgramNode>& programs = db.getProgramsFile();
    for (int i = 0; i < resultItemNumber && i < programs.size(); i ++) {
        auto& programItem = programs[i];
        const std::wstring& programName = programItem.programName;
        const std::wstring& programPath = programItem.programPath;

        const QPixmap& programIcon = getIconWithPath(programPath);
        QString str = QString::fromStdWString(programName);

        resultFrame.addItem(programIcon, str);
    }
    resultFrame.adjustSizeToFitItems();
    resultFrame.setCurrentItemIndex(0);
}
