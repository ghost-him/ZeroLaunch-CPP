#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <unordered_set>

struct ProgramNode {
    std::wstring showName;

    // 精确匹配的名字，拼音的名字，拼音的首字母。
    std::vector<std::wstring> compareNames;

    std::wstring programPath;
    std::wstring iconPath;

    double compatibility;
    int stableBias;
    int launchTime;

    bool isUWPApp;

    size_t maxNameLength;

    const bool operator<(const ProgramNode& other) const {
        if (compatibility != other.compatibility) {
            return compatibility < other.compatibility;
        } else if (launchTime != other.launchTime) {
            return launchTime > other.launchTime;
        } else {
            return compareNames[0] < other.compareNames[0];
        }
    }
};

class Database
{

public:
    // 删除拷贝构造函数和拷贝赋值操作符
    Database(Database const&) = delete;
    Database& operator=(Database const&) = delete;
    // 提供一个全局唯一的接口
    static Database& getInstance() {
        static Database instance;
        return instance;
    }

    void insertProgramInfo(const std::wstring& programName, const std::wstring& programPath, const std::wstring& iconPath, int stableLevel, bool isUWPApp);

    void updateScores(std::wstring inputName);

    const std::vector<ProgramNode>& getProgramsFile();

    bool isExist(const std::wstring& key);

    void testCompareAlgorithm(std::wstring inputValue);

    void addLaunchTime(int index);

    void clearProgramInfo();

    void addForbiddenName(const std::wstring& name);

private:
    Database();
    std::vector<std::wstring> forbiddenNames;
    std::vector<ProgramNode> programs;
    std::unordered_set<std::wstring> cache;

    std::wstring& tolower(std::wstring& other);
    // 该函数会去除程序名字中的版本号与括号中的内容，同时还会转换为小写的名字
    std::wstring preprocess(const std::wstring& inputText);

    double calculateStringCompatibility(const std::wstring& compareName, const std::wstring& inputName);

    double calculateWeight(double inputLen);

    double calculateEditDistance(const std::wstring& compareName, const std::wstring& inputValue);

    double calculateCompatibility(const ProgramNode& node, const std::wstring& inputName);

    void debugProgramNode();

    std::wstring extractInitials(const std::wstring& name);

    bool isValidName(const std::wstring& s);

    std::wstring removeStringSpace(const std::wstring& str);

};

#endif // DATABASE_H
