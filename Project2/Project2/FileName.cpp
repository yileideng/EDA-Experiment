#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <fstream>
using namespace std;


enum Type {
    AND,
    OR,
    NOT,
    INPUT,
    OUTPUT,
    DEFAULT
};

class Node {
public:
    // 后继节点
    list<Node*> next;
    // 前驱节点
    list<Node*> pre;
    // 节点类型
    Type type;
    // 节点名称
    string name;
    // 是否已调度
    bool processed = false;

    int startCycle = -1;

    Node(string nodeName) : name(nodeName), type(DEFAULT) {}
    Node() {}
};

class Graph {
public:
    // 节点集
    map<string, Node*> nodes;
    // 记录每个周期的调度节点集
    vector<vector<Node*>> cycles;
    // 图的大小
    int size;
    // 已调度节点的数量
    int processCount;

    // 向调度节点集添加节点
    void addVector(int cycleIndex, Node* node) {
        if (cycleIndex >= cycles.size()) {
            cycles.resize(cycleIndex + 1);
        }
        cycles[cycleIndex].push_back(node);
    }

    // 清空调度状态
    void clear() {
        for (auto& pair : nodes) {
            pair.second->processed = false;
            pair.second->startCycle = -1;
        }
        processCount = 0;

        for (vector<Node*> cycle : cycles) {
            cycle.clear();
        }
        cycles.clear();
    }

    // 初始化图
    Graph() : size(0) {
        size = 0;
        processCount = 0;
    }
};

// 处理输入，初始化图
void processInput(Graph& g, vector<std::string> blifContent, int size) {
    int index = 0;
    while (index++ < size) {
        string line = blifContent[index];
        if (line.find(".names") != string::npos) {
            // 提取节点名称
            string names = line.substr(line.find(" ") + 1);
            size_t space;
            // 输入节点
            list<string> inputs;
            while ((space = names.find(" ")) != string::npos) {
                inputs.push_back(names.substr(0, space));
                names = names.substr(space + 1);
            }
            // 输出节点
            string output = names;

            // 创建输入节点
            for (auto input : inputs) {
                if (g.nodes.find(input) == g.nodes.end()) {
                    g.nodes[input] = new Node(input);
                    g.nodes[input]->name = input;
                    g.size++;
                }
            }
            // 创建输出节点
            if (g.nodes.find(output) == g.nodes.end()) {
                g.nodes[output] = new Node(output);
                g.nodes[output]->name = output;
                g.size++;
            }

            // 添加前驱和后继关系
            for (auto input : inputs) {
                g.nodes[input]->next.push_back(g.nodes[output]);
                g.nodes[output]->pre.push_back(g.nodes[input]);
            }


            // 判断节点类型
            Type type;
            // 偏移量
            int offset = 1;
            // 下一行
            string nextLine = blifContent[index + offset];  

            while (index + offset < size) { 
                if (nextLine.find(".names") != string::npos || nextLine.find(".inputs") != string::npos ||
                    nextLine.find(".outputs") != string::npos || nextLine.find(".model") != string::npos ||
                    nextLine.find(".end") != string::npos) {
                    break;
                }
                // NOT字符串只有3列
                if (nextLine.size() == 3) {
                    break;
                }
                offset++;
                nextLine = blifContent[index + offset];
            }
            if (offset == 1) {
                type = NOT;
            }
            if (offset == 2) {
                type = AND;
            }if (offset > 2) {
                type = OR;
            }
            // 判断节点类型
            g.nodes[output]->type = type;
            continue;
        }
        else if (line.find(".inputs") != string::npos) {
            // 输入节点
            string inputs = line.substr(line.find(" ") + 1);
            size_t space;
            while ((space = inputs.find(" ")) != string::npos) {
                string input = inputs.substr(0, space);
                if (g.nodes.find(input) == g.nodes.end()) {
                    g.nodes[input] = new Node(input);
                    g.nodes[input]->type = INPUT;
                    g.nodes[input]->name = input;
                    g.size++;
                }
                inputs = inputs.substr(space + 1);
            }
            if (!inputs.empty()) {
                if (g.nodes.find(inputs) == g.nodes.end()) {
                    g.nodes[inputs] = new Node(inputs);
                    g.nodes[inputs]->type = INPUT;
                    g.nodes[inputs]->name = inputs;
                    g.size++;
                }
            }
        }
        else if (line.find(".outputs") != string::npos) {
            // 输出节点
            string outputs = line.substr(line.find(" ") + 1);
            size_t space;
            while ((space = outputs.find(" ")) != string::npos) {
                string output = outputs.substr(0, space);
                if (g.nodes.find(output) == g.nodes.end()) {
                    g.nodes[output] = new Node(output);
                    g.nodes[output]->type = OUTPUT;
                    g.nodes[output]->name = output;
                    g.size++;
                }
                outputs = outputs.substr(space + 1);
            }
            if (!outputs.empty()) {
                if (g.nodes.find(outputs) == g.nodes.end()) {
                    g.nodes[outputs] = new Node(outputs);
                    g.nodes[outputs]->type = OUTPUT;
                    g.nodes[outputs]->name = outputs;
                    g.size++;
                }
            }
        }
        else if (line.find(".model") != string::npos) {
            // 模型信息
            std::cout << "模型名称：" << line.substr(line.find(" ") + 1) << endl;
        }
        else if (line.find(".end") != string::npos) {
            break;
        }
    }
}

// 打印一个调度周期的节点集
void printNodes(vector<Node*> nodes) {
    for (int nodeType = 0; nodeType < 3; nodeType++) {
        std::cout << "{ ";
        for (Node* node : nodes) {
            // AND,OR,NOT
            if (node->type == (Type)nodeType){
                std::cout << node->name + " ";
            }
        }
        std::cout << "},";
    }
}
// 打印所有调度周期
void printCycles(vector<vector<Node*>> cycles) {
    std::cout << "Total " << cycles.size() - 1 << " Cycles" << endl;

    int index = 0;   
    for (vector<Node*> cycle : cycles) {
        if (cycle.empty()) {
            index++;
            continue;
        }
        if (cycle.front()->type == INPUT) {
            index++;
            continue;
        }
        std::cout << "Cycle " << index << ":";
        printNodes(cycle);
        std::cout << endl;
        index++;
    }
}
// 打印输入输出节点
void printInOut(Graph& g) {
    std::cout << "Input:";
    for (auto& pair : g.nodes) {
        Node* current = pair.second;
        if (current->pre.empty()) {
            std::cout << current->name << " ";
        }
    }

    std::cout << "Output:";
    for (auto& pair : g.nodes) {
        Node* current = pair.second;
        if (current->next.empty()) {
            std::cout << current->name << " ";
        }
    }

    std::cout << endl;
}
// 打印整张图
void printGraph(Graph& g) {
    std::cout << "图中共有 " << g.size << " 个节点：" << endl;
    for (auto& pair : g.nodes) {
        std::cout << "节点名称：" << pair.second->name << endl;
        std::cout << "前驱节点：";
        for (Node* preNode : pair.second->pre) {
            std::cout << preNode->name << " ";
        }
        std::cout << endl;

        std::cout << "后继节点：";
        for (Node* nextNode : pair.second->next) {
            std::cout << nextNode->name << " ";
        }
        std::cout << endl;

        std::cout << "节点类型：";
        switch (pair.second->type) {
        case  AND:
            std::cout << "AND" << endl;
            break;
        case OR:
            std::cout << "OR" << endl;
            break;
        case NOT:
            std::cout << "NOT" << endl;
            break;
        case INPUT:
            std::cout << "INPUT" << endl;
            break;
        case OUTPUT:
            std::cout << "OUTPUT" << endl;
            break;
        default:
            std::cout << "DEFAULT" << endl;
        }
        std::cout << endl << endl;

    }
}

// ASAP算法，返回周期数
int ASAP(Graph& g) {
  
    // 每次循环是一个周期
    int cycleIndex = 0;
    while (g.processCount < g.size) {
        // 当前周期的调度节点集
        vector<Node*> processingNodes;

        // 查找当前周期可以调度的节点
        for (auto& pair : g.nodes) {
            Node* currentNode = pair.second;

            // 跳过已调度节点
            if (currentNode->processed) { 
                continue;
            }

            // 表示前驱节点是否已全部调度过
            bool preProcessed = true;
            // 检查非输入节点的前驱
            if (!currentNode->pre.empty()) {
                for (Node* n : currentNode->pre) {
                    // 存在前驱节点未调度，则不调度该节点
                    if (!n->processed) { 
                        preProcessed = false;
                        break;
                    }
                }
            }

            // 只有当前驱节点全部调度完成的节点才加入当前周期
            if (preProcessed) { 
                processingNodes.push_back(currentNode);
            }
        }

        // 如果没有可处理的节点，说明图中存在循环依赖
        if (processingNodes.empty()) {
            std::cerr << "Error: Graph contains a cycle." << std::endl;
            break;
        }

        // 调度当前周期的节点
        for (Node* node : processingNodes) {
            g.addVector(cycleIndex, node);
            node->processed = true;
            g.processCount++;
        }
        cycleIndex++;
    }

    return cycleIndex;
}

// ALAP算法
void ALAP(Graph& g, int requiredCycle) {

    // 每次循环是一个周期
    while (g.processCount < g.size) {
        // 当前周期的调度节点集
        vector<Node*> processingNodes;

        // 查找当前周期可以调度的节点
        for (auto& pair : g.nodes) {
            Node* currentNode = pair.second;

            // 跳过已调度节点
            if (currentNode->processed) {
                continue;
            }

            // 表示后继节点是否已全部调度过
            bool nextProcessed = true;
            // 检查非输出节点的后继
            if (!currentNode->next.empty()) {
                for (Node* n : currentNode->next) {
                    // 存在后继节点未调度，则不调度该节点
                    if (!n->processed) {
                        nextProcessed = false;
                        break;
                    }
                }
            }

            // 只有当前驱节点全部调度完成的节点才加入当前周期
            if (nextProcessed) {
                processingNodes.push_back(currentNode);
            }
        }

        // 如果没有可处理的节点，说明图中存在循环依赖
        if (processingNodes.empty()) {
            std::cerr << "Error: Graph contains a cycle." << std::endl;
            break;
        }

        // 调度当前周期的节点
        for (Node* node : processingNodes) {
            g.addVector(requiredCycle, node);
            node->processed = true;
            g.processCount++;
        }
        requiredCycle--;
    }

    return;
}

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
    processInput(*g, blifContent, sizeof(blifContent));
    // 打印图中的所有节点
    printGraph(*g);

    // 执行ALAP算法
    int minCycles = ASAP(*g);
    // 打印ASAP执行结果
    printInOut(*g);
    printCycles(g->cycles);

    // 执行ALAP算法
    ALAP(*g, minCycles+5);
    // 打印ALAP执行结果
    printInOut(*g);
    printCycles(g->cycles);

    // 重置调度状态
    g->clear();


    delete g;

    return 0;
}