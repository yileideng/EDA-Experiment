#include "algorithm.h"
#include "utils.h"
#include <fstream>

int main() {
    std::ifstream file("./sample.blif");
    if (!file.is_open()) {
        std::cerr << "无法打开文件" << std::endl;
        return 1;
    }

    std::string line;
    std::vector<std::string> blifContent;  // 使用vector存储每一行

    while (std::getline(file, line)) {
        blifContent.push_back(line);  // 将每一行添加到vector中
    }

    Graph* g = new Graph();

    // 处理输入，初始化图 
    processInput(*g, blifContent);
    // 打印图中的所有节点
    printGraph(*g);

    // 执行ASAP和ALAP算法，初始化节点的最晚调度周期
    int minCycles = ASAP(*g);
    g->clear();

    ALAP(*g, minCycles);
    g->clear();

    
    // 执行ML_RCS算法
    unordered_map<Type, int> resouces = { {AND,1},{OR,1},{NOT,1},{INPUT,10} };
    ML_RCS(*g, resouces);

    // 打印ML_RCS算法执行结果
    cout << "ML_RCS:" << endl;
    printInOut(*g);
    printCycles(*g);

    delete g;

    return 0;
}