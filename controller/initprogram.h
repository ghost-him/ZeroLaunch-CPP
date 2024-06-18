#ifndef INITPROGRAM_H
#define INITPROGRAM_H

#include <windows.h>
#include <QString>
#include <filesystem>
#include <unordered_set>

namespace fs = std::filesystem;


class InitProgram
{
public:
    // 删除拷贝构造函数和拷贝赋值操作符
    InitProgram(InitProgram const&) = delete;
    InitProgram& operator=(InitProgram const&) = delete;
    // 提供一个全局唯一的接口
    static InitProgram& getInstance() {
        static InitProgram instance;
        return instance;
    }

    void initProgramWithRegistery();

    void initProgramWithStartMenu();

    void initProgramWithProgramFileDir();

    void initCustomPath();

    void clearStore();

private:
    InitProgram();
    // path:遍历的目录， fulldirectory：是否要遍历子目录，level_bias：遍历出来后，要添加的等级的偏移
    void initTargetDirectory(const std::wstring& path, unsigned int depth, int level_bias = 0);

    std::unordered_set<std::wstring> initTargetRegistery(HKEY hKeyRoot);

    void initTargetStartMenu(const QString &path);

    bool isValidFile(const std::filesystem::path& path);

    bool isValidPath(const fs::directory_entry& entry);

    QString getDefaultProgramFilesPath(bool is64Bit);

    QString getSystemStartMenuPath();
    QString getUserStartMenuPath();
    QString getUserHomePath();

    bool starts_with_directory(const fs::directory_entry& entry, const std::wstring& dir);

    std::vector<std::wstring> bannedPath;
};

#endif // INITPROGRAM_H
