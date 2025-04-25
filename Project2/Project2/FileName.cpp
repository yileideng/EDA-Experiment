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
    // 调度开始周期，-1表示未调度
    int startCycle;
    // 操作执行时间
    int executionTime;

    int getEndCycle() {
        return startCycle + executionTime;
    }

    Node(string nodeName, int execTime = 1) : name(nodeName), type(DEFAULT), executionTime(execTime) {
        startCycle = -1;
    }
    Node(int execTime = 1) : type(DEFAULT), executionTime(execTime) {
        startCycle = -1;
    }
};

class Graph {
public:
    // 节点集
    map<string, Node*> nodes;
    // 图的大小
    int size;
    // 已调度节点的数量
    int processCount;

    // 清空调度状态
    void clear() {
        for (auto& pair : nodes) {
            pair.second->startCycle = -1;
        }
        processCount = 0;
    }

    // 初始化图
    Graph() : size(0) {
        size = 0;
        processCount = 0;
    }
};

// ASAP算法，返回周期数
int ASAP(Graph& g) {
    // 每次循环是一个周期
    int currentCycle = 0;
    while (g.processCount < g.size) {
        // 当前周期的调度节点集
        vector<Node*> processingNodes;

        // 查找当前周期可以调度的节点
        for (auto& pair : g.nodes) {
            Node* currentNode = pair.second;

            // 跳过已调度节点
            if (currentNode->startCycle != -1) {
                continue;
            }

            // 表示前驱节点是否已全部调度过
            bool preProcessFinished = true;
            int maxPreEndCycle = 0;
            // 检查非输入节点的前驱
            if (!currentNode->pre.empty()) {
                for (Node* n : currentNode->pre) {
                    // 存在前驱节点未调度
                    if (n->startCycle == -1) {
                        preProcessFinished = false;
                        break;
                    }
                    // 更新最大前驱结束周期
                    maxPreEndCycle = max(maxPreEndCycle, n->getEndCycle());
                }
            }

            // 只有当前驱节点全部调度完成，且当前周期大于等于最大前驱结束周期的节点才加入当前周期
            if (preProcessFinished && currentCycle >= maxPreEndCycle) {
                processingNodes.push_back(currentNode);
            }
        }

        // 调度当前周期的节点
        if (!processingNodes.empty()) {
            for (Node* node : processingNodes) {
                node->startCycle = currentCycle;
                g.processCount++;
            }
        }
        currentCycle++;
    }

    return currentCycle - 1;
}

void ALAP(Graph& g, int requiredCycle) {
    int minCycle = ASAP(g);
    cout << "ASAP Cycle Count: " << minCycle << endl;
    g.clear();
    if (requiredCycle < minCycle) {
        std::cout << "时间过短，无法完成" << endl;
        return;
    }

    // 每次循环是一个周期,周期从后往前推
    int currentCycle = requiredCycle; 
    while (g.processCount < g.size) {
        // 维护当前周期的调度节点集
        vector<Node*> processingNodes;

        // 查找当前周期可以调度的节点
        for (auto& pair : g.nodes) {
            Node* currentNode = pair.second;

            // 跳过已调度节点
            if (currentNode->startCycle != -1) {
                continue;
            }

            // 表示后继节点是否已全部调度过
            bool nextProcessed = true;
            int minNextStartCycle = requiredCycle + 1;

            if (!currentNode->next.empty()) {
                for (Node* n : currentNode->next) {
                    if (n->startCycle == -1) {
                        nextProcessed = false;
                        break;
                    }
                    // 表示后继节点的最小开始时间
                    minNextStartCycle = min(minNextStartCycle, n->startCycle);
                }
            }

            // 当前节点的执行时间不能超过从当前周期到后继节点最早开始周期的时间差
            if (nextProcessed && currentNode->executionTime <= (minNextStartCycle - currentCycle)) {
                processingNodes.push_back(currentNode);
            }
        }

        // 调度当前周期的节点
        if (!processingNodes.empty()) {
            for (Node* node : processingNodes) {
                node->startCycle = currentCycle;
                g.processCount++;
            }
        }
        currentCycle--;
    }
}

// 处理输入，初始化图
void processInput(Graph& g, vector<std::string> blifContent) {
    int size = blifContent.size();
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
                g.nodes[output]->executionTime = 1; // 可根据需要修改
            }
            if (offset == 2) {
                type = AND;
                g.nodes[output]->executionTime = 3; // 可根据需要修改
            }
            if (offset > 2) {
                type = OR;
                g.nodes[output]->executionTime = 2; // 可根据需要修改
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

// 打印所有调度周期
void printCycles(Graph& g) {
    // 找到最大周期
    int totalCycles = 0;
    for (auto& pair : g.nodes) {
        Node* current = pair.second;
        totalCycles = totalCycles > current->startCycle + current->executionTime - 1 ? totalCycles : current->startCycle + current->executionTime - 1;
    }
    std::cout << "Total " << totalCycles << " Cycles" << endl;

    // 找到每个周期的节点
    for (int i = 1; i <= totalCycles; i++) {
        std::cout << "Cycle " << i << ":";
        vector<Node*> andNodes, orNodes, notNodes;
        // 遍历节点，找第i周期开始的节点
        for (auto& pair : g.nodes) {
            Node* n = pair.second;
            if (n->startCycle <= i && i < n->startCycle + n->executionTime) {
                if (n->type == AND) {
                    andNodes.push_back(n);
                }
                else if (n->type == OR) {
                    orNodes.push_back(n);
                }
                else if (n->type == NOT) {
                    notNodes.push_back(n);
                }
            }
        }

        std::cout << "{ ";
        for (Node* n : andNodes) {
            std::cout << n->name + " ";
        }
        std::cout << "},";

        std::cout << "{ ";
        for (Node* n : orNodes) {
            std::cout << n->name + " ";
        }
        std::cout << "},";

        std::cout << "{ ";
        for (Node* n : notNodes) {
            std::cout << n->name + " ";
        }
        std::cout << "} ";

        std::cout << endl;
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
        std::cout << "执行时间: " << pair.second->executionTime << endl;
        std::cout << endl << endl;
    }
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
    processInput(*g, blifContent);
    // 打印图中的所有节点
    printGraph(*g);

    // 执行ALAP算法
    int minCycles = ASAP(*g);
    // 打印ASAP执行结果
    printInOut(*g);
    printCycles(*g);

    // 重置调度状态
    g->clear();
    std::cout << endl << endl;

    // 执行ALAP算法
    ALAP(*g, minCycles);
    // 打印ALAP执行结果
    printInOut(*g);
    printCycles(*g);

    delete g;

    return 0;
}    