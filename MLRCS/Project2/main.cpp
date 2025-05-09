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

    
    // 执行ML_RCS算法
    unordered_map<Type, int> mlResouces = { {AND,2},{OR,2},{NOT,1},{INPUT,10} };
    int minLatancy = ML_RCS(*g, mlResouces);

    // 打印ML_RCS算法执行结果
    cout << "ML_RCS:" << endl;
    std::cout << "Constrained Resource ：{" << endl;
    for (auto& pair : mlResouces) {
        std::cout << "  " << typeToString(pair.first) << "：" << pair.second << endl;
    }
    std::cout << "}" << std::endl;
    std::cout << "Minimum Latancy ：" << minLatancy << endl << endl;

    printInOut(*g);
    printCycles(*g);

    
    g = new Graph();
    // 处理输入，初始化图 
    processInput(*g, blifContent);

    // 执行MR_LCS算法
    unordered_map<Type, int> mrResouces = { {AND,1},{OR,1},{NOT,1},{INPUT,1} };
    int constrainedLatancy = 9;
    MR_LCS(*g, mrResouces, constrainedLatancy);

    // 打印MR_LCS算法执行结果
    cout << "MR_LCS:" << endl;
    std::cout << "Constrained Latancy ：" << constrainedLatancy << std::endl;
    std::cout << "Minimum  Resource ：{" << endl;
    for (auto& pair : mrResouces) {
        std::cout << "  " << typeToString(pair.first) << "：" << pair.second << endl;
    }
    std::cout << "}" << endl << endl;

    printInOut(*g);
    printCycles(*g);

    delete g;

    return 0;
}