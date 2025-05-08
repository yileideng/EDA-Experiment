#include <iostream>
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
    // ��̽ڵ�
    vector<Node*> next;
    // ǰ���ڵ�
    vector<Node*> pre;
    // �ڵ�����
    Type type;
    // �ڵ�����
    string name;
    // ���ȿ�ʼ���ڣ�-1��ʾδ����
    int startCycle;
    // ����ִ��ʱ��
    int executionTime;
    // alap�Ľڵ㿪ʼʱ��
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
};

class Graph {
public:
    // �ڵ㼯
    map<string, Node*> nodes;
    // ͼ�Ĵ�С
    int size;
    // �ѵ��Ƚڵ������
    int processCount;

    // ��յ���״̬
    void clear() {
        for (auto& pair : nodes) {
            pair.second->startCycle = -1;
        }
        processCount = 0;
    }

    // ��ʼ��ͼ
    Graph() : size(0) {
        size = 0;
        processCount = 0;
    }
};

vector<Node*> getUnprocessedNodes(Graph& g, int currentCycle);
vector<Node*> getProcessingNodes(vector<Node*> unprocessedNode, int resource);
int getCurrentCycleProcessingCounts(Graph& g, int currentCycle);

// ASAP�㷨������������
int ASAP(Graph& g) {
    // ÿ��ѭ����һ������
    int currentCycle = 0;
    // �ѵ��Ƚڵ������
    int processCount = 0;
    while (processCount < g.size) {
        // ��ǰ���ڵĵ��Ƚڵ㼯
        vector<Node*> processingNodes;

        // ���ҵ�ǰ���ڿ��Ե��ȵĽڵ�
        for (auto& pair : g.nodes) {
            Node* currentNode = pair.second;

            // �����ѵ��Ƚڵ�
            if (currentNode->startCycle != -1) {
                continue;
            }

            // ��ʾǰ���ڵ��Ƿ���ȫ�����ȹ�
            bool preProcessFinished = true;
            int maxPreEndCycle = 0;
            // ��������ڵ��ǰ��
            if (!currentNode->pre.empty()) {
                for (Node* n : currentNode->pre) {
                    // ����ǰ���ڵ�δ����
                    if (n->startCycle == -1) {
                        preProcessFinished = false;
                        break;
                    }
                    // �������ǰ����������
                    maxPreEndCycle = max(maxPreEndCycle, n->getEndCycle());
                }
            }

            // ֻ�е�ǰ���ڵ�ȫ��������ɣ��ҵ�ǰ���ڴ��ڵ������ǰ���������ڵĽڵ�ż��뵱ǰ����
            if (preProcessFinished && currentCycle >= maxPreEndCycle) {
                processingNodes.push_back(currentNode);
            }
        }

        // ���ȵ�ǰ���ڵĽڵ�
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

// ALAP�㷨
void ALAP(Graph& g, int requiredCycle) {
    int minCycle = ASAP(g);
    g.clear();
    if (requiredCycle < minCycle) {
        std::cout << "ʱ����̣��޷����" << endl;
        return;
    }

    // ÿ��ѭ����һ������,���ڴӺ���ǰ��
    int currentCycle = requiredCycle; 
    // �ѵ��Ƚڵ������
    int processCount = 0;
    while (processCount < g.size) {
        // ά����ǰ���ڵĵ��Ƚڵ㼯
        vector<Node*> processingNodes;

        // ���ҵ�ǰ���ڿ��Ե��ȵĽڵ�
        for (auto& pair : g.nodes) {
            Node* currentNode = pair.second;

            // �����ѵ��Ƚڵ�
            if (currentNode->startCycle != -1) {
                continue;
            }

            // ��ʾ��̽ڵ��Ƿ���ȫ�����ȹ�
            bool nextProcessed = true;
            int minNextStartCycle = requiredCycle + 1;

            if (!currentNode->next.empty()) {
                for (Node* n : currentNode->next) {
                    if (n->startCycle == -1) {
                        nextProcessed = false;
                        break;
                    }
                    // ��ʾ��̽ڵ����С��ʼʱ��
                    minNextStartCycle = min(minNextStartCycle, n->startCycle);
                }
            }

            // ��ǰ�ڵ��ִ��ʱ�䲻�ܳ����ӵ�ǰ���ڵ���̽ڵ����翪ʼ���ڵ�ʱ���
            if (nextProcessed && currentNode->executionTime <= (minNextStartCycle - currentCycle)) {
                processingNodes.push_back(currentNode);
            }
        }

        // ���ȵ�ǰ���ڵĽڵ�
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

// Hu�㷨������������
int Hu(Graph& g, int resource) {
    // ÿ��ѭ����һ������
    int currentCycle = 0;
    // �ѵ��Ƚڵ������
    int processCount = 0;
    while (processCount < g.size) {
        // ��ǰ���ڵĿ��Ե��Ƚڵ㼯
        vector<Node*> unprocessedNodes = getUnprocessedNodes(g, currentCycle);

        if (unprocessedNodes.empty()) {
            currentCycle++;
            continue;
        }

        // ��ȡ��ǰ�������ڵ��ȵĽڵ���
        int processingCounts = getCurrentCycleProcessingCounts(g, currentCycle);

        // ȷ�����Ƚڵ㼯
        vector<Node*> processingNodes = getProcessingNodes(unprocessedNodes, resource - processingCounts);

        if (processingNodes.empty()) {
            currentCycle++;
            continue;
        }

        // ���ȵ�ǰ���ڵĽڵ�
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

// δ������ǰ���ڵ��ѵ�����ɵĽڵ㼯U
vector<Node*> getUnprocessedNodes(Graph& g, int currentCycle) {
    // �ɵ��Ƚڵ㼯U
    vector<Node*> unprocessedNodes;
    for (auto& pair : g.nodes) {
        Node* currentNode = pair.second;

        // �����ѵ��Ƚڵ�
        if (currentNode->startCycle != -1) {
            continue;
        }

        // ��ʾǰ���ڵ��Ƿ���ȫ�����ȹ�
        bool preProcessFinished = true;
        int maxPreEndCycle = 0;
        // ��������ڵ��ǰ��
        if (!currentNode->pre.empty()) {
            for (Node* n : currentNode->pre) {
                // ����ǰ���ڵ�δ����
                if (n->startCycle == -1) {
                    preProcessFinished = false;
                    break;
                }
                // �������ǰ����������
                maxPreEndCycle = max(maxPreEndCycle, n->getEndCycle());
            }
        }

        // �����ǰ���ڵ�δ���ȣ���ǰ����С�����ǰ���������ڵĽڵ�
        if (!preProcessFinished || currentCycle < maxPreEndCycle) {
            continue;
        }

        unprocessedNodes.push_back(currentNode);
    }
    return unprocessedNodes;
}

// ��ȡ������Ƚڵ㼯S
vector<Node*> getProcessingNodes(vector<Node*> unprocessedNode, int resource) {
    // ������Ƚڵ㼯S
    vector<Node*> processingNodes;

    while (!unprocessedNode.empty() && processingNodes.size() < resource) {
        // ��ȡ�ɵ��Ƚڵ㼯���������Ƚڵ�
        Node* minNode = unprocessedNode[0];
        for (Node* node : unprocessedNode) {
            if (node->alapStartCycle < minNode->alapStartCycle) {
                minNode = node;
            }
        }
        processingNodes.push_back(minNode);

        // ʹ�ñ�׼���㷨��ȫ�Ƴ�minNode
        auto it = std::remove(unprocessedNode.begin(), unprocessedNode.end(), minNode);
        unprocessedNode.erase(it, unprocessedNode.end());
    }
    return processingNodes;
}
// ��ȡ��ǰ�������ڵ��ȵĽڵ���
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

// �������룬��ʼ��ͼ
void processInput(Graph& g, vector<std::string> blifContent) {
    int size = blifContent.size();
    int index = 0;
    while (index++ < size) {
        string line = blifContent[index];
        if (line.find(".names") != string::npos) {
            // ��ȡ�ڵ�����
            string names = line.substr(line.find(" ") + 1);
            size_t space;
            // ����ڵ�
            vector<string> inputs;
            while ((space = names.find(" ")) != string::npos) {
                inputs.push_back(names.substr(0, space));
                names = names.substr(space + 1);
            }
            // ����ڵ�
            string output = names;

            // ��������ڵ�
            for (auto input : inputs) {
                if (g.nodes.find(input) == g.nodes.end()) {
                    g.nodes[input] = new Node(input);
                    g.nodes[input]->name = input;
                    g.size++;
                }
            }
            // ��������ڵ�
            if (g.nodes.find(output) == g.nodes.end()) {
                g.nodes[output] = new Node(output);
                g.nodes[output]->name = output;
                g.size++;
            }

            // ���ǰ���ͺ�̹�ϵ
            for (auto input : inputs) {
                g.nodes[input]->next.push_back(g.nodes[output]);
                g.nodes[output]->pre.push_back(g.nodes[input]);
            }

            // �жϽڵ�����
            Type type;
            // ƫ����
            int offset = 1;
            // ��һ��
            string nextLine = blifContent[index + offset];

            while (index + offset < size) {
                if (nextLine.find(".names") != string::npos || nextLine.find(".inputs") != string::npos ||
                    nextLine.find(".outputs") != string::npos || nextLine.find(".model") != string::npos ||
                    nextLine.find(".end") != string::npos) {
                    break;
                }
                // NOT�ַ���ֻ��3��
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
                g.nodes[output]->executionTime = 2; 
            }
            // OR
            if (offset > 2) {
                type = OR;
                g.nodes[output]->executionTime = 3; 
            }
            g.nodes[output]->type = type;
            continue;
        }
        else if (line.find(".inputs") != string::npos) {
            // ����ڵ�
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
            // ����ڵ�
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
            // ģ����Ϣ
            std::cout << "ģ�����ƣ�" << line.substr(line.find(" ") + 1) << endl;
        }
        else if (line.find(".end") != string::npos) {
            break;
        }
    }
}

// ��ӡ���е�������
void printCycles(Graph& g) {
    // �ҵ��������
    int totalCycles = 0;
    for (auto& pair : g.nodes) {
        Node* current = pair.second;
        totalCycles = totalCycles > current->startCycle + current->executionTime - 1 ? totalCycles : current->startCycle + current->executionTime - 1;
    }
    std::cout << "Total " << totalCycles+1 << " Cycles" << endl;

    // �ҵ�ÿ�����ڵĽڵ�
    for (int i = 0; i <= totalCycles; i++) {
        std::cout << "Cycle " << i << ":";
        vector<Node*> andNodes, orNodes, notNodes, inputNodes;
        // �����ڵ㣬�ҵ�i���ڿ�ʼ�Ľڵ�
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

// ��ӡ��������ڵ�
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

// ��ӡ����ͼ
void printGraph(Graph& g) {
    std::cout << "ͼ�й��� " << g.size << " ���ڵ㣺" << endl;
    for (auto& pair : g.nodes) {
        std::cout << "�ڵ����ƣ�" << pair.second->name << endl;
        std::cout << "ǰ���ڵ㣺";
        for (Node* preNode : pair.second->pre) {
            std::cout << preNode->name << " ";
        }
        std::cout << endl;

        std::cout << "��̽ڵ㣺";
        for (Node* nextNode : pair.second->next) {
            std::cout << nextNode->name << " ";
        }
        std::cout << endl;

        std::cout << "�ڵ����ͣ�";
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
        std::cout << "��ʼ����ʱ��" << pair.second->startCycle << endl;
        std::cout << "ִ��ʱ��: " << pair.second->executionTime << endl;
        std::cout << endl << endl;
    }
}

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

    // ִ��ALAP�㷨
    int minCycles = ASAP(*g);
    // ��ӡASAPִ�н��
    printInOut(*g);
    printCycles(*g);

    // ���õ���״̬
    g->clear();
    std::cout << endl << endl;

    // ִ��ALAP�㷨
    ALAP(*g, 9);
    // ��ӡALAPִ�н��
    printInOut(*g);
    printCycles(*g);

    // ���õ���״̬
    g->clear();
    std::cout << endl << endl;

    // ִ��Hu�㷨
    Hu(*g, 20);
    // ��ӡHu�㷨ִ�н��
    printInOut(*g);
    printCycles(*g);

    delete g;

    return 0;
}    