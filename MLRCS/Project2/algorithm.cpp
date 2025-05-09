#include"algorithm.h"

// ML_RCS�㷨������������
int ML_RCS(Graph& g, unordered_map<Type, int> resouces) {
    // ÿ��ѭ����һ������
    int currentCycle = 0;
    // �ѵ��Ƚڵ������
    int processCount = 0;
    while (processCount < g.size) {
        // ��ǰ���ڵ����п��Ե��Ƚڵ㼯
        vector<Node*> unprocessedNodes = getUnprocessedNodes(g, currentCycle);

        if (unprocessedNodes.empty()) {
            currentCycle++;
            continue;
        }

        // ����ÿ����Դ���м��
        for (auto& pair : resouces) {
            // ��ȡ��ǰ�������ڵ��ȵ�ĳ����Դ��
            int processingCounts = getCurrentCycleProcessingCounts(g, currentCycle, pair.first);

            // ȷ�����Ƚڵ㼯(pair.second - processingCounts������Դ�� - ����ִ����Դ�� = ʣ����Դ��)
            vector<Node*> processingNodes = getProcessingNodes(unprocessedNodes, pair.second - processingCounts, pair.first);

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

    return currentCycle - 1;
}

// ��ȡĳ�������пɵ��Ƚڵ㼯������ԴԼ����
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

// ��ȡ��ǰ�������ڵ��ȵ�ĳ��Դ��
int getCurrentCycleProcessingCounts(Graph& g, int currentCycle, Type resourceType) {
    int counts = 0;
    for (auto& pair : g.nodes) {
        Node* currentNode = pair.second;
        // 1.������ĳ����Դ 2.�ýڵ��ѵ��� 3.�ýڵ����ڵ���
        if (currentNode->type == resourceType && currentNode->startCycle != -1 && currentNode->getEndCycle() > currentCycle) {
            counts++;
        }
    }
    return counts;
}

// �ڿɵ��Ƚڵ㼯�����ϣ���ȡָ����Դ����Լ���Ľڵ㼯
vector<Node*> getProcessingNodes(vector<Node*> unprocessedNode, int resourceNum, Type resourceType) {
    // ������Ƚڵ㼯S
    vector<Node*> processingNodes;

    while (!unprocessedNode.empty() && processingNodes.size() < resourceNum) {
        // �����ѡһ��ָ����Դ�Ľڵ�
        Node* minNode = getRequiredNode(unprocessedNode, resourceType);

        // ��û��ָ����Դ�ڵ��ʱ�򣬾Ϳ��Է�����
        if (minNode == NULL) {
            return processingNodes;
        }

        // ��ȡ�ɵ��Ƚڵ㼯���������Ƚڵ�
        for (Node* node : unprocessedNode) {
            if (node->type == resourceType && node->alapStartCycle < minNode->alapStartCycle) {
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

// �����ȡָ����Դ�Ľڵ�
Node* getRequiredNode(vector<Node*> nodes, Type requiredType) {
    for (Node* node : nodes) {
        if (node->type == requiredType) {
            return node;
        }
    }
    return NULL;
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