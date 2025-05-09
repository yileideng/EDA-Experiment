#include "algorithm.h"
#include "utils.h"
#include <fstream>

int main() {
    std::ifstream file("./sample.blif");
    if (!file.is_open()) {
        std::cerr << "�޷����ļ�" << std::endl;
        return 1;
    }

    std::string line;
    std::vector<std::string> blifContent;  // ʹ��vector�洢ÿһ��

    while (std::getline(file, line)) {
        blifContent.push_back(line);  // ��ÿһ����ӵ�vector��
    }

    Graph* g = new Graph();

    // �������룬��ʼ��ͼ 
    processInput(*g, blifContent);
    // ��ӡͼ�е����нڵ�
    printGraph(*g);

    
    // ִ��ML_RCS�㷨
    unordered_map<Type, int> mlResouces = { {AND,2},{OR,2},{NOT,1},{INPUT,10} };
    int minLatancy = ML_RCS(*g, mlResouces);

    // ��ӡML_RCS�㷨ִ�н��
    cout << "ML_RCS:" << endl;
    std::cout << "Constrained Resource ��{" << endl;
    for (auto& pair : mlResouces) {
        std::cout << "  " << typeToString(pair.first) << "��" << pair.second << endl;
    }
    std::cout << "}" << std::endl;
    std::cout << "Minimum Latancy ��" << minLatancy << endl << endl;

    printInOut(*g);
    printCycles(*g);

    
    g = new Graph();
    // �������룬��ʼ��ͼ 
    processInput(*g, blifContent);

    // ִ��MR_LCS�㷨
    unordered_map<Type, int> mrResouces = { {AND,1},{OR,1},{NOT,1},{INPUT,1} };
    int constrainedLatancy = 9;
    MR_LCS(*g, mrResouces, constrainedLatancy);

    // ��ӡMR_LCS�㷨ִ�н��
    cout << "MR_LCS:" << endl;
    std::cout << "Constrained Latancy ��" << constrainedLatancy << std::endl;
    std::cout << "Minimum  Resource ��{" << endl;
    for (auto& pair : mrResouces) {
        std::cout << "  " << typeToString(pair.first) << "��" << pair.second << endl;
    }
    std::cout << "}" << endl << endl;

    printInOut(*g);
    printCycles(*g);

    delete g;

    return 0;
}