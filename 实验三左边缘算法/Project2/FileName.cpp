#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
#include <algorithm>
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
    vector<Node*> next;
    // 前驱节点
    vector<Node*> pre;
    // 节点类型
    Type type;
    // 节点名称
    string name;
    // 调度开始周期，-1表示未调度
    int startCycle;
    // 操作执行时间
    int executionTime;
    // alap的节点开始时间
    int alapStartCycle;

    int getEndCycle() {
        return startCycle + executionTime;
    }

    Node(string nodeName, int execTime = 1) : name(nodeName), type(DEFAULT), executionTime(execTime) {
        startCycle = -1;
    }
    Node(int execTime = 1) : type(DEFAULT), executionTime(execTime) {
        startCycle = -1;
    }
    Node(string nodeName, int startCycle, int endCycle) {
        this->name = nodeName;
        this->startCycle = startCycle;
        this->executionTime = endCycle - startCycle + 1;
    }
};

class Graph {
public:
    // 节点集
    map<string, Node*> nodes;
    // 图的大小
    int size;

    // 清空调度状态
    void clear() {
        for (auto& pair : nodes) {
            pair.second->startCycle = -1;
        }
    }

    // 初始化图
    Graph() : size(0) {
        size = 0;
    }
};

vector<Node*> getUnprocessedNodes(Graph& g, int currentCycle);
vector<Node*> getProcessingNodes(vector<Node*> unprocessedNode, int resource);
int getCurrentCycleProcessingCounts(Graph& g, int currentCycle);

// ASAP算法，返回周期数
int ASAP(Graph& g) {
    // 每次循环是一个周期
    int currentCycle = 0;
    // 已调度节点的数量
    int processCount = 0;
    while (processCount < g.size) {
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
                processCount++;
            }
        }
        currentCycle++;
    }

    return currentCycle - 1;
}

// ALAP算法
void ALAP(Graph& g, int requiredCycle) {
    int minCycle = ASAP(g);
    g.clear();
    if (requiredCycle < minCycle) {
        std::cout << "时间过短，无法完成" << endl;
        return;
    }

    // 每次循环是一个周期,周期从后往前推
    int currentCycle = requiredCycle; 
    // 已调度节点的数量
    int processCount = 0;
    while (processCount < g.size) {
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
                node->alapStartCycle = currentCycle;
                processCount++;
            }
        }
        currentCycle--;
    }
}

// Hu算法，返回周期数
int Hu(Graph& g, int resource) {
    // 每次循环是一个周期
    int currentCycle = 0;
    // 已调度节点的数量
    int processCount = 0;
    while (processCount < g.size) {
        // 当前周期的可以调度节点集
        vector<Node*> unprocessedNodes = getUnprocessedNodes(g, currentCycle);

        if (unprocessedNodes.empty()) {
            currentCycle++;
            continue;
        }

        // 获取当前周期正在调度的节点数
        int processingCounts = getCurrentCycleProcessingCounts(g, currentCycle);

        // 确定调度节点集
        vector<Node*> processingNodes = getProcessingNodes(unprocessedNodes, resource - processingCounts);

        if (processingNodes.empty()) {
            currentCycle++;
            continue;
        }

        // 调度当前周期的节点
        if (!processingNodes.empty()) {
            for (Node* node : processingNodes) {
                node->startCycle = currentCycle;
                processCount++;
            }
        }
        currentCycle++;
    }

    return currentCycle - 1;
}

// 未调度且前驱节点已调度完成的节点集U
vector<Node*> getUnprocessedNodes(Graph& g, int currentCycle) {
    // 可调度节点集U
    vector<Node*> unprocessedNodes;
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

        // 如果有前驱节点未调度，或当前周期小于最大前驱结束周期的节点
        if (!preProcessFinished || currentCycle < maxPreEndCycle) {
            continue;
        }

        unprocessedNodes.push_back(currentNode);
    }
    return unprocessedNodes;
}

// 获取最早调度节点集S
vector<Node*> getProcessingNodes(vector<Node*> unprocessedNode, int resource) {
    // 最早调度节点集S
    vector<Node*> processingNodes;

    while (!unprocessedNode.empty() && processingNodes.size() < resource) {
        // 获取可调度节点集里的最早调度节点
        Node* minNode = unprocessedNode[0];
        for (Node* node : unprocessedNode) {
            if (node->alapStartCycle < minNode->alapStartCycle) {
                minNode = node;
            }
        }
        processingNodes.push_back(minNode);

        // 使用标准库算法安全移除minNode
        auto it = std::remove(unprocessedNode.begin(), unprocessedNode.end(), minNode);
        unprocessedNode.erase(it, unprocessedNode.end());
    }
    return processingNodes;
}
// 获取当前周期正在调度的节点数
int getCurrentCycleProcessingCounts(Graph& g, int currentCycle) {
    int counts = 0;
    for (auto& pair : g.nodes) {
        Node* currentNode = pair.second;
        if (currentNode->startCycle!=-1 && currentNode->getEndCycle() > currentCycle) {
            counts++;
        }
    }
    return counts;
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
            vector<string> inputs;
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
            // NOT
            if (offset == 1) {
                type = NOT;
                g.nodes[output]->executionTime = 1; 
            }
            // AND
            if (offset == 2) {
                type = AND;
                g.nodes[output]->executionTime = 1; 
            }
            // OR
            if (offset > 2) {
                type = OR;
                g.nodes[output]->executionTime = 1; 
            }
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
    std::cout << "Total " << totalCycles+1 << " Cycles" << endl;

    // 找到每个周期的节点
    for (int i = 0; i <= totalCycles; i++) {
        std::cout << "Cycle " << i << ":";
        vector<Node*> andNodes, orNodes, notNodes, inputNodes;
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
                else if (n->type == INPUT) {
                    inputNodes.push_back(n);
                }
            }
        }
        std::cout << "{ ";
        for (Node* n : inputNodes) {
            std::cout << n->name + " ";
        }
        std::cout << "},";

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
        std::cout << "开始调度时间" << pair.second->startCycle << endl;
        std::cout << "执行时间: " << pair.second->executionTime << endl;
        std::cout << endl << endl;
    }
}



// 左边缘算法
vector<vector<Node*>> leftEdge(vector<Node*> nodes) {
    vector<vector<Node*>> result;
    // 根据开始时间升序排序
    sort(nodes.begin(), nodes.end(), [](Node* a, Node* b) { return a->startCycle < b->startCycle; }); // 排序
    while (!nodes.empty()) {
        vector<Node*> temp;
        // 初始右端点为-1
        int endCycle = -1; 
        for (auto it = nodes.begin(); it != nodes.end();) {
            Node* node = *it;
            // 满足不重叠条件
            if (node->startCycle > endCycle) { 
                temp.push_back(node);
                // 更新右端点为当前节点的结束时间
                endCycle = node->getEndCycle() - 1; 
                // 从列表中删除该节点
                it = nodes.erase(it); 
            }
            else {
                it++;
            }
        }
        result.push_back(temp);
    }
    return result;
}

// 打印结果
void printLeftEdge(const vector<vector<Node*>>& result) {
    cout << "染色数是" << result.size() << "，独立集包含的元素分别是：" << endl;
    for (const auto& group : result) {
        cout << "{ ";
        for (Node* node : group) {
            cout << node->name << " ";
        }
        cout << "}" << endl;
    }
}

// 从文件读取节点信息并转换为Node*向量
vector<Node*> readNodesFromFile(const string& filename) {
    vector<Node*> nodes;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "无法打开文件: " << filename << endl;
        return nodes;
    }

    string line;
    while (getline(file, line)) {
        // 跳过空行
        if (line.empty()) continue;

        // 解析每行数据，格式为 "(A,1,4)"
        size_t pos1 = line.find('(');
        size_t pos2 = line.find(',');
        size_t pos3 = line.find(',', pos2 + 1);
        size_t pos4 = line.find(')');

        // 验证格式是否正确
        if (pos1 == string::npos || pos2 == string::npos ||
            pos3 == string::npos || pos4 == string::npos ||
            pos1 >= pos2 || pos2 >= pos3 || pos3 >= pos4) {
            cerr << "格式错误: " << line << endl;
            continue;
        }

        // 提取节点名称和时间
        string name = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string startStr = line.substr(pos2 + 1, pos3 - pos2 - 1);
        string endStr = line.substr(pos3 + 1, pos4 - pos3 - 1);

        // 转换为整数
        try {
            int startCycle = stoi(startStr);
            int endCycle = stoi(endStr);

            // 创建节点并添加到向量
            nodes.push_back(new Node(name, startCycle, endCycle));
        }
        catch (const invalid_argument& e) {
            cerr << "无法转换为整数: " << line << endl;
        }
    }

    file.close();
    return nodes;
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

    // 执行ASAP算法
    int minCycles = ASAP(*g);
    // 打印ASAP执行结果
    std::cout << "ASAP:" << endl;
    printInOut(*g);
    printCycles(*g);
    cout << endl;

    // 左边缘算法
    vector<Node*> nodes;
    for (auto& pair : g->nodes) {
        nodes.push_back(pair.second);
    }
    vector<vector<Node*>> result = leftEdge(nodes);
    printLeftEdge(result);
    cout << endl;
    delete g;


    // 读取文件
    vector<Node*> nodes2 = readNodesFromFile("./input.txt");

    if (nodes2.empty()) {
        cerr << "没有读取到有效节点" << endl;
        return 1;
    }

    // 左边缘算法
    vector<vector<Node*>> result2 = leftEdge(nodes2);
    printLeftEdge(result2);
    return 0;
}    