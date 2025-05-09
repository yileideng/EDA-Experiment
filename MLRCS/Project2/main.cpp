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

    // ִ��ASAP��ALAP�㷨����ʼ���ڵ�������������
    int minCycles = ASAP(*g);
    g->clear();

    ALAP(*g, minCycles);
    g->clear();

    
    // ִ��ML_RCS�㷨
    unordered_map<Type, int> resouces = { {AND,1},{OR,1},{NOT,1},{INPUT,10} };
    ML_RCS(*g, resouces);

    // ��ӡML_RCS�㷨ִ�н��
    cout << "ML_RCS:" << endl;
    printInOut(*g);
    printCycles(*g);

    delete g;

    return 0;
}