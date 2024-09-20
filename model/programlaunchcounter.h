#ifndef PROGRAMLAUNCHCOUNTER_H
#define PROGRAMLAUNCHCOUNTER_H

#include "../controller/utils.h"
#include <unordered_map>
#include <deque>

class ProgramLaunchCounter
{
public:

    // 删除拷贝构造函数和拷贝赋值操作符
    ProgramLaunchCounter(ProgramLaunchCounter const&) = delete;
    ProgramLaunchCounter& operator=(ProgramLaunchCounter const&) = delete;
    // 提供一个全局唯一的接口
    static ProgramLaunchCounter& getInstance() {
        static ProgramLaunchCounter instance;
        return instance;
    }
    // 检查一个程序的总打开次数
    size_t queryTotalOpenCount(const std::wstring& programName);
    // 检查最近xxx次的打开次数
    size_t queryRecentOpenCount(const std::wstring& programName);
    // 打开一次程序
    void openProgramName(const std::wstring& programName);
    // 将数据导出为json文件
    void save();
    // 将数据加载
    void load(const QJsonObject& jsonData);
    // 设置统计最近多少次的
    void setRecentCount(size_t count);
    // 初始化
    void init();

    void enableStatistic(bool isEnable);

private:
    void initStatisticJson();


    std::unordered_map<std::wstring, int> totalOpenCount;
    std::unordered_map<std::wstring, int> recentOpenCount;
    std::deque<std::wstring> recentOpenProgram;
    int recentCount = 15;

    bool isInited;
    bool isEnable;

    ProgramLaunchCounter();
};

#endif // PROGRAMLAUNCHCOUNTER_H
