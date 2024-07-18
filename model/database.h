#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <unordered_set>

struct ProgramNode {
    std::wstring showName;

    std::wstring compareName;
    std::vector<std::wstring> nameParts;

    std::wstring programPath;
    std::wstring iconPath;

    double compatibility;
    int stableBias;
    int launchTime;

    bool isUWPApp;

    int pinyinLength;

    const bool operator<(const ProgramNode& other) const {
        if (compatibility != other.compatibility) {
            return compatibility < other.compatibility;
        } else if (launchTime != other.launchTime) {
            return launchTime > other.launchTime;
        } else {
            return compareName < other.compareName;
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

    void updateScores(const std::wstring& inputName);

    const std::vector<ProgramNode>& getProgramsFile();

    bool isExist(const std::wstring& key);

    void testCompareAlgorithm(std::wstring inputValue);

    void addLaunchTime(int index);

    void clearProgramInfo();

private:
    Database();
    std::vector<ProgramNode> programs;
    std::unordered_set<std::wstring> cache;

    std::wstring& tolower(std::wstring& other);
    std::wstring preprocess(const std::wstring& inputText);

    std::vector<std::wstring> splitStringBySpace(const std::wstring& str);

    std::vector<std::vector<std::wstring>> splitString(const std::wstring& s);
    // 辅助函数：按大小写分割
    std::vector<std::wstring> splitStringByCamelCase(const std::wstring& s);

    void calculateCompareName(ProgramNode& app, std::wstring inputName);

    void calculateNameParts(ProgramNode& app, const std::vector<std::vector<std::wstring>>& splits, double alpha);

    double calculateScore(const std::wstring& inputPart, const std::wstring& targetPart);

    double calculateWeight(double inputLen);

    double calculatePenalty(double x);

    double calculateEditDistance(const std::wstring& compareName, const std::wstring& inputValue);

    void debugProgramNode();

    bool isNumericSequence(const std::wstring& s, size_t start, size_t length);


};

#endif // DATABASE_H
