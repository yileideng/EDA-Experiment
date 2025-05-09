#include"algorithm.h"
#include "utils.h"
// ML_RCS算法，返回周期数
int ML_RCS(Graph& g, unordered_map<Type, int> resouces) {
    // 执行ASAP和ALAP，初始化节点的最晚调度周期
    int minCycles = ASAP(g); 
    g.clear();
    ALAP(g, minCycles);
    g.clear();

    // 每次循环是一个周期
    int currentCycle = 0;
    // 已调度节点的数量
    int processCount = 0;
    while (processCount < g.size) {
        // 当前周期的所有可以调度节点集（未调度，或前驱节点全部调度完成）
        vector<Node*> readyNodes = getReadyNodes(g, currentCycle);

        if (readyNodes.empty()) {
            currentCycle++;
            continue;
        }

        // 分别对每种资源的节点进行调度
        for (auto& pair : resouces) {
            // 获取当前周期正在调度的某种资源数
            int processingCounts = getProcessingCounts(g, currentCycle, pair.first);

            // 确定调度节点集(pair.second - processingCounts即：总资源数 - 正在执行资源数 = 剩余资源数)
            vector<Node*> processingNodes = confirmProcessNodes(readyNodes, pair.second - processingCounts, pair.first);

            // 调度选中的节点
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

// 获取某周期所有可调度节点集（无资源约束）
vector<Node*> getReadyNodes(Graph& g, int currentCycle) {
    // 可调度节点集U
    vector<Node*> readyNodes;
    for (auto& pair : g.nodes) {
        Node* currentNode = pair.second;

        // 跳过已调度节点
        if (currentNode->startCycle != -1) {
            continue;
        }

        // 表示前驱节点是否已全部调度过
        bool allPreProcessed = true;
        int maxPreNodeEndCycle = 0;
        // 检查非输入节点的前驱
        if (!currentNode->pre.empty()) {
            for (Node* n : currentNode->pre) {
                // 存在前驱节点未调度
                if (n->startCycle == -1) {
                    allPreProcessed = false;
                    break;
                }
                // 更新最大前驱结束周期
                maxPreNodeEndCycle = max(maxPreNodeEndCycle, n->getEndCycle());
            }
        }

        // 如果有前驱节点未调度，或当前周期小于最大前驱结束周期的节点
        if (!allPreProcessed || currentCycle < maxPreNodeEndCycle) {
            continue;
        }

        // 将当前节点加入就绪节点集
        readyNodes.push_back(currentNode);
    }
    return readyNodes;
}

// 获取当前周期正在调度的某资源数
int getProcessingCounts(Graph& g, int currentCycle, Type resourceType) {
    int counts = 0;
    for (auto& pair : g.nodes) {
        Node* currentNode = pair.second;
        // 1.必须是某种资源 2.该节点已调度 3.该节点正在调度（已被调度并且正在调度）
        if (currentNode->type == resourceType && currentNode->startCycle != -1 && currentNode->getEndCycle() > currentCycle) {
            counts++;
        }
    }
    return counts;
}

// 在可调度节点集基础上，获取指定资源下有约束的节点集
vector<Node*> confirmProcessNodes(vector<Node*> readyNodes, int resourceNum, Type resourceType) {
    // 调度节点集S
    vector<Node*> processingNodes;

    while (!readyNodes.empty() && processingNodes.size() < resourceNum) {
        // 获取优先级最高的节点
        Node* highestPriorityNode = getHighestPriorityNode(readyNodes, resourceType);

        // 当没有指定资源节点的时候，就可以返回了
        if (highestPriorityNode == NULL) {
            return processingNodes;
        }

        // 在processingNodes加入highestPriorityNode
        processingNodes.push_back(highestPriorityNode);

        // 从readyNodes移除highestPriorityNode
        auto it = std::remove(readyNodes.begin(), readyNodes.end(), highestPriorityNode);
        readyNodes.erase(it, readyNodes.end());
    }
    return processingNodes;
}

// 获取优先级最高的节点
Node* getHighestPriorityNode(vector<Node*> nodes, Type requiredType) {
    // 选择一个指定资源的节点作为开始节点
    Node* highestPriorityNode = NULL;
    for (Node* node : nodes) {
        if (node->type == requiredType) {
            highestPriorityNode = node;
            break;
        }
    }
    // 当没有指定资源节点的时候，就可以返回了
    if (highestPriorityNode == NULL) {
        return NULL;
    }

    // 遍历寻找优先级最高的节点
    for (Node* node : nodes) {
        if (node->type == requiredType && node->priority < highestPriorityNode->priority) {
            highestPriorityNode = node;
        }
    }

    return highestPriorityNode;
}



// MR_LCS算法，返回周期数
void MR_LCS(Graph& g, unordered_map<Type, int>& resouces, int requiredCycle) {
    // 执行ASAP,确定要求时间是否合理
    int minCycles = ASAP(g);
    g.clear();
    if (requiredCycle < minCycles) {
        std::cout << "要求时间过短，无法在要求时间内完成调度" << endl;
        return;
    }
    // 执行ALAP算法，初始化节点的最晚调度周期
    ALAP(g, requiredCycle);
    g.clear();

    // 已调度节点的数量
    int processCount = 0;
    // 每次循环是一个周期
    int currentCycle = 0;

    while (processCount < g.size) {
        // 当前周期的所有可以调度节点集（未调度，或前驱节点全部调度完成）
        vector<Node*> readyNodes = getReadyNodes(g, currentCycle);

        if (readyNodes.empty()) {
            currentCycle++;
            continue;
        }

        // 对于每种资源进行检查
        for (auto& pair : resouces) {
            // 获取当前周期正在调度的某种资源数
            int processingCounts = getProcessingCounts(g, currentCycle, pair.first);

            // 确定调度节点集
            vector<Node*> processingNodes = confirmProcessNodes(readyNodes, processingCounts, currentCycle, pair.second, pair.first);

            // 调度选中的节点
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
    // 调度节点集S
    vector<Node*> processingNodes;

    // 将必须调度的节点先调度了
    for (Node* node : readyNodes) {
        if (node->type == resourceType && node->priority == currentCycle) {
            processingNodes.push_back(node);
        }
    }

    // 从待处理节点中移除已调度的必须节点
    for (Node* node : processingNodes) {
        auto it = find(readyNodes.begin(), readyNodes.end(), node);
        if (it != readyNodes.end()) {
            readyNodes.erase(it);
        }
    }
    
    // 剩余的资源数量
    int availableSlots = resourceNum - processingCounts - processingNodes.size();

    // 如果资源超出了，则增加资源数量
    if (availableSlots <= 0) {
        resourceNum = processingNodes.size() + processingCounts;
    }
    // 如果资源未超出，可以调度部分剩余的同类型节点
    while (availableSlots > 0 && !readyNodes.empty() ) {
        // 获取优先级最高的节点
        Node* highestPriorityNode = getHighestPriorityNode(readyNodes, resourceType);

        // 当没有指定资源节点的时候，就跳出循环
        if (highestPriorityNode == NULL) {
            break;
        }

        // 在processingNodes加入highestPriorityNode
        processingNodes.push_back(highestPriorityNode);
        availableSlots--;

        // 从readyNodes移除highestPriorityNode
        auto it = std::remove(readyNodes.begin(), readyNodes.end(), highestPriorityNode);
        readyNodes.erase(it, readyNodes.end());
    }
    
    return processingNodes;
}


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

    return currentCycle;
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
    int currentCycle = requiredCycle - 1;
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
            int minNextStartCycle = requiredCycle;

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
                node->priority = currentCycle;
                processCount++;
            }
        }
        currentCycle--;
    }
}