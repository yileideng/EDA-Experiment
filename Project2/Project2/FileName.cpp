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
    // ��̽ڵ�
    list<Node*> next;
    // ǰ���ڵ�
    list<Node*> pre;
    // �ڵ�����
    Type type;
    // �ڵ�����
    string name;
    // �Ƿ��ѵ���
    bool processed = false;

    int startCycle = -1;

    Node(string nodeName) : name(nodeName), type(DEFAULT) {}
    Node() {}
};

class Graph {
public:
    // �ڵ㼯
    map<string, Node*> nodes;
    // ��¼ÿ�����ڵĵ��Ƚڵ㼯
    vector<vector<Node*>> cycles;
    // ͼ�Ĵ�С
    int size;
    // �ѵ��Ƚڵ������
    int processCount;

    // ����Ƚڵ㼯��ӽڵ�
    void addVector(int cycleIndex, Node* node) {
        if (cycleIndex >= cycles.size()) {
            cycles.resize(cycleIndex + 1);
        }
        cycles[cycleIndex].push_back(node);
    }

    // ��յ���״̬
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

    // ��ʼ��ͼ
    Graph() : size(0) {
        size = 0;
        processCount = 0;
    }
};

// �������룬��ʼ��ͼ
void processInput(Graph& g, vector<std::string> blifContent, int size) {
    int index = 0;
    while (index++ < size) {
        string line = blifContent[index];
        if (line.find(".names") != string::npos) {
            // ��ȡ�ڵ�����
            string names = line.substr(line.find(" ") + 1);
            size_t space;
            // ����ڵ�
            list<string> inputs;
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
            if (offset == 1) {
                type = NOT;
            }
            if (offset == 2) {
                type = AND;
            }if (offset > 2) {
                type = OR;
            }
            // �жϽڵ�����
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

// ��ӡһ���������ڵĽڵ㼯
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
// ��ӡ���е�������
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
        std::cout << endl << endl;

    }
}

// ASAP�㷨������������
int ASAP(Graph& g) {
  
    // ÿ��ѭ����һ������
    int cycleIndex = 0;
    while (g.processCount < g.size) {
        // ��ǰ���ڵĵ��Ƚڵ㼯
        vector<Node*> processingNodes;

        // ���ҵ�ǰ���ڿ��Ե��ȵĽڵ�
        for (auto& pair : g.nodes) {
            Node* currentNode = pair.second;

            // �����ѵ��Ƚڵ�
            if (currentNode->processed) { 
                continue;
            }

            // ��ʾǰ���ڵ��Ƿ���ȫ�����ȹ�
            bool preProcessed = true;
            // ��������ڵ��ǰ��
            if (!currentNode->pre.empty()) {
                for (Node* n : currentNode->pre) {
                    // ����ǰ���ڵ�δ���ȣ��򲻵��ȸýڵ�
                    if (!n->processed) { 
                        preProcessed = false;
                        break;
                    }
                }
            }

            // ֻ�е�ǰ���ڵ�ȫ��������ɵĽڵ�ż��뵱ǰ����
            if (preProcessed) { 
                processingNodes.push_back(currentNode);
            }
        }

        // ���û�пɴ���Ľڵ㣬˵��ͼ�д���ѭ������
        if (processingNodes.empty()) {
            std::cerr << "Error: Graph contains a cycle." << std::endl;
            break;
        }

        // ���ȵ�ǰ���ڵĽڵ�
        for (Node* node : processingNodes) {
            g.addVector(cycleIndex, node);
            node->processed = true;
            g.processCount++;
        }
        cycleIndex++;
    }

    return cycleIndex;
}

// ALAP�㷨
void ALAP(Graph& g, int requiredCycle) {

    // ÿ��ѭ����һ������
    while (g.processCount < g.size) {
        // ��ǰ���ڵĵ��Ƚڵ㼯
        vector<Node*> processingNodes;

        // ���ҵ�ǰ���ڿ��Ե��ȵĽڵ�
        for (auto& pair : g.nodes) {
            Node* currentNode = pair.second;

            // �����ѵ��Ƚڵ�
            if (currentNode->processed) {
                continue;
            }

            // ��ʾ��̽ڵ��Ƿ���ȫ�����ȹ�
            bool nextProcessed = true;
            // ��������ڵ�ĺ��
            if (!currentNode->next.empty()) {
                for (Node* n : currentNode->next) {
                    // ���ں�̽ڵ�δ���ȣ��򲻵��ȸýڵ�
                    if (!n->processed) {
                        nextProcessed = false;
                        break;
                    }
                }
            }

            // ֻ�е�ǰ���ڵ�ȫ��������ɵĽڵ�ż��뵱ǰ����
            if (nextProcessed) {
                processingNodes.push_back(currentNode);
            }
        }

        // ���û�пɴ���Ľڵ㣬˵��ͼ�д���ѭ������
        if (processingNodes.empty()) {
            std::cerr << "Error: Graph contains a cycle." << std::endl;
            break;
        }

        // ���ȵ�ǰ���ڵĽڵ�
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
    processInput(*g, blifContent, sizeof(blifContent));
    // ��ӡͼ�е����нڵ�
    printGraph(*g);

    // ִ��ALAP�㷨
    int minCycles = ASAP(*g);
    // ��ӡASAPִ�н��
    printInOut(*g);
    printCycles(g->cycles);

    // ִ��ALAP�㷨
    ALAP(*g, minCycles+5);
    // ��ӡALAPִ�н��
    printInOut(*g);
    printCycles(g->cycles);

    // ���õ���״̬
    g->clear();


    delete g;

    return 0;
}