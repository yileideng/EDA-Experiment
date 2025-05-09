#include"algorithm.h"
#include "utils.h"
// ML_RCS�㷨������������
int ML_RCS(Graph& g, unordered_map<Type, int> resouces) {
    // ִ��ASAP��ALAP����ʼ���ڵ�������������
    int minCycles = ASAP(g); 
    g.clear();
    ALAP(g, minCycles);
    g.clear();

    // ÿ��ѭ����һ������
    int currentCycle = 0;
    // �ѵ��Ƚڵ������
    int processCount = 0;
    while (processCount < g.size) {
        // ��ǰ���ڵ����п��Ե��Ƚڵ㼯��δ���ȣ���ǰ���ڵ�ȫ��������ɣ�
        vector<Node*> readyNodes = getReadyNodes(g, currentCycle);

        if (readyNodes.empty()) {
            currentCycle++;
            continue;
        }

        // �ֱ��ÿ����Դ�Ľڵ���е���
        for (auto& pair : resouces) {
            // ��ȡ��ǰ�������ڵ��ȵ�ĳ����Դ��
            int processingCounts = getProcessingCounts(g, currentCycle, pair.first);

            // ȷ�����Ƚڵ㼯(pair.second - processingCounts��������Դ�� - ����ִ����Դ�� = ʣ����Դ��)
            vector<Node*> processingNodes = confirmProcessNodes(readyNodes, pair.second - processingCounts, pair.first);

            // ����ѡ�еĽڵ�
            if (!processingNodes.empty()) {
                for (Node* node : processingNodes) {
                    node->startCycle = currentCycle;
                    processCount++;
                }
            }
        }

        currentCycle++;
    }

    return currentCycle;
}

// ��ȡĳ�������пɵ��Ƚڵ㼯������ԴԼ����
vector<Node*> getReadyNodes(Graph& g, int currentCycle) {
    // �ɵ��Ƚڵ㼯U
    vector<Node*> readyNodes;
    for (auto& pair : g.nodes) {
        Node* currentNode = pair.second;

        // �����ѵ��Ƚڵ�
        if (currentNode->startCycle != -1) {
            continue;
        }

        // ��ʾǰ���ڵ��Ƿ���ȫ�����ȹ�
        bool allPreProcessed = true;
        int maxPreNodeEndCycle = 0;
        // ��������ڵ��ǰ��
        if (!currentNode->pre.empty()) {
            for (Node* n : currentNode->pre) {
                // ����ǰ���ڵ�δ����
                if (n->startCycle == -1) {
                    allPreProcessed = false;
                    break;
                }
                // �������ǰ����������
                maxPreNodeEndCycle = max(maxPreNodeEndCycle, n->getEndCycle());
            }
        }

        // �����ǰ���ڵ�δ���ȣ���ǰ����С�����ǰ���������ڵĽڵ�
        if (!allPreProcessed || currentCycle < maxPreNodeEndCycle) {
            continue;
        }

        // ����ǰ�ڵ��������ڵ㼯
        readyNodes.push_back(currentNode);
    }
    return readyNodes;
}

// ��ȡ��ǰ�������ڵ��ȵ�ĳ��Դ��
int getProcessingCounts(Graph& g, int currentCycle, Type resourceType) {
    int counts = 0;
    for (auto& pair : g.nodes) {
        Node* currentNode = pair.second;
        // 1.������ĳ����Դ 2.�ýڵ��ѵ��� 3.�ýڵ����ڵ��ȣ��ѱ����Ȳ������ڵ��ȣ�
        if (currentNode->type == resourceType && currentNode->startCycle != -1 && currentNode->getEndCycle() > currentCycle) {
            counts++;
        }
    }
    return counts;
}

// �ڿɵ��Ƚڵ㼯�����ϣ���ȡָ����Դ����Լ���Ľڵ㼯
vector<Node*> confirmProcessNodes(vector<Node*> readyNodes, int resourceNum, Type resourceType) {
    // ���Ƚڵ㼯S
    vector<Node*> processingNodes;

    while (!readyNodes.empty() && processingNodes.size() < resourceNum) {
        // ��ȡ���ȼ���ߵĽڵ�
        Node* highestPriorityNode = getHighestPriorityNode(readyNodes, resourceType);

        // ��û��ָ����Դ�ڵ��ʱ�򣬾Ϳ��Է�����
        if (highestPriorityNode == NULL) {
            return processingNodes;
        }

        // ��processingNodes����highestPriorityNode
        processingNodes.push_back(highestPriorityNode);

        // ��readyNodes�Ƴ�highestPriorityNode
        auto it = std::remove(readyNodes.begin(), readyNodes.end(), highestPriorityNode);
        readyNodes.erase(it, readyNodes.end());
    }
    return processingNodes;
}

// ��ȡ���ȼ���ߵĽڵ�
Node* getHighestPriorityNode(vector<Node*> nodes, Type requiredType) {
    // ѡ��һ��ָ����Դ�Ľڵ���Ϊ��ʼ�ڵ�
    Node* highestPriorityNode = NULL;
    for (Node* node : nodes) {
        if (node->type == requiredType) {
            highestPriorityNode = node;
            break;
        }
    }
    // ��û��ָ����Դ�ڵ��ʱ�򣬾Ϳ��Է�����
    if (highestPriorityNode == NULL) {
        return NULL;
    }

    // ����Ѱ�����ȼ���ߵĽڵ�
    for (Node* node : nodes) {
        if (node->type == requiredType && node->priority < highestPriorityNode->priority) {
            highestPriorityNode = node;
        }
    }

    return highestPriorityNode;
}



// MR_LCS�㷨������������
void MR_LCS(Graph& g, unordered_map<Type, int>& resouces, int requiredCycle) {
    // ִ��ASAP,ȷ��Ҫ��ʱ���Ƿ����
    int minCycles = ASAP(g);
    g.clear();
    if (requiredCycle < minCycles) {
        std::cout << "Ҫ��ʱ����̣��޷���Ҫ��ʱ������ɵ���" << endl;
        return;
    }
    // ִ��ALAP�㷨����ʼ���ڵ�������������
    ALAP(g, requiredCycle);
    g.clear();

    // �ѵ��Ƚڵ������
    int processCount = 0;
    // ÿ��ѭ����һ������
    int currentCycle = 0;

    while (processCount < g.size) {
        // ��ǰ���ڵ����п��Ե��Ƚڵ㼯��δ���ȣ���ǰ���ڵ�ȫ��������ɣ�
        vector<Node*> readyNodes = getReadyNodes(g, currentCycle);

        if (readyNodes.empty()) {
            currentCycle++;
            continue;
        }

        // ����ÿ����Դ���м��
        for (auto& pair : resouces) {
            // ��ȡ��ǰ�������ڵ��ȵ�ĳ����Դ��
            int processingCounts = getProcessingCounts(g, currentCycle, pair.first);

            // ȷ�����Ƚڵ㼯
            vector<Node*> processingNodes = confirmProcessNodes(readyNodes, processingCounts, currentCycle, pair.second, pair.first);

            // ����ѡ�еĽڵ�
            if (!processingNodes.empty()) {
                for (Node* node : processingNodes) {
                    node->startCycle = currentCycle;
                    processCount++;
                }
            }
        }

        currentCycle++;
    }

    return;
}

vector<Node*> confirmProcessNodes(vector<Node*> readyNodes, int processingCounts, int currentCycle, int& resourceNum, Type resourceType) {
    // ���Ƚڵ㼯S
    vector<Node*> processingNodes;

    // ��������ȵĽڵ��ȵ�����
    for (Node* node : readyNodes) {
        if (node->type == resourceType && node->priority == currentCycle) {
            processingNodes.push_back(node);
        }
    }

    // �Ӵ�����ڵ����Ƴ��ѵ��ȵı���ڵ�
    for (Node* node : processingNodes) {
        auto it = find(readyNodes.begin(), readyNodes.end(), node);
        if (it != readyNodes.end()) {
            readyNodes.erase(it);
        }
    }
    
    // ʣ�����Դ����
    int availableSlots = resourceNum - processingCounts - processingNodes.size();

    // �����Դ�����ˣ���������Դ����
    if (availableSlots <= 0) {
        resourceNum = processingNodes.size() + processingCounts;
    }
    // �����Դδ���������Ե��Ȳ���ʣ���ͬ���ͽڵ�
    while (availableSlots > 0 && !readyNodes.empty() ) {
        // ��ȡ���ȼ���ߵĽڵ�
        Node* highestPriorityNode = getHighestPriorityNode(readyNodes, resourceType);

        // ��û��ָ����Դ�ڵ��ʱ�򣬾�����ѭ��
        if (highestPriorityNode == NULL) {
            break;
        }

        // ��processingNodes����highestPriorityNode
        processingNodes.push_back(highestPriorityNode);
        availableSlots--;

        // ��readyNodes�Ƴ�highestPriorityNode
        auto it = std::remove(readyNodes.begin(), readyNodes.end(), highestPriorityNode);
        readyNodes.erase(it, readyNodes.end());
    }
    
    return processingNodes;
}


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

    return currentCycle;
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
    int currentCycle = requiredCycle - 1;
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
            int minNextStartCycle = requiredCycle;

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
                node->priority = currentCycle;
                processCount++;
            }
        }
        currentCycle--;
    }
}