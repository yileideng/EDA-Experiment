#include"algorithm.h"

// ML_RCS算法，返回周期数
int ML_RCS(Graph& g, unordered_map<Type, int> resouces) {
    // 每次循环是一个周期
    int currentCycle = 0;
    // 已调度节点的数量
    int processCount = 0;
    while (processCount < g.size) {
        // 当前周期的所有可以调度节点集
        vector<Node*> unprocessedNodes = getUnprocessedNodes(g, currentCycle);

        if (unprocessedNodes.empty()) {
            currentCycle++;
            continue;
        }

        // 对于每种资源进行检查
        for (auto& pair : resouces) {
            // 获取当前周期正在调度的某种资源数
            int processingCounts = getCurrentCycleProcessingCounts(g, currentCycle, pair.first);

            // 确定调度节点集(pair.second - processingCounts，总资源数 - 正在执行资源数 = 剩余资源数)
            vector<Node*> processingNodes = getProcessingNodes(unprocessedNodes, pair.second - processingCounts, pair.first);

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

    return currentCycle - 1;
}

// 获取某周期所有可调度节点集（无资源约束）
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

// 获取当前周期正在调度的某资源数
int getCurrentCycleProcessingCounts(Graph& g, int currentCycle, Type resourceType) {
    int counts = 0;
    for (auto& pair : g.nodes) {
        Node* currentNode = pair.second;
        // 1.必须是某种资源 2.该节点已调度 3.该节点正在调度
        if (currentNode->type == resourceType && currentNode->startCycle != -1 && currentNode->getEndCycle() > currentCycle) {
            counts++;
        }
    }
    return counts;
}

// 在可调度节点集基础上，获取指定资源下有约束的节点集
vector<Node*> getProcessingNodes(vector<Node*> unprocessedNode, int resourceNum, Type resourceType) {
    // 最早调度节点集S
    vector<Node*> processingNodes;

    while (!unprocessedNode.empty() && processingNodes.size() < resourceNum) {
        // 随机挑选一个指定资源的节点
        Node* minNode = getRequiredNode(unprocessedNode, resourceType);

        // 当没有指定资源节点的时候，就可以返回了
        if (minNode == NULL) {
            return processingNodes;
        }

        // 获取可调度节点集里的最早调度节点
        for (Node* node : unprocessedNode) {
            if (node->type == resourceType && node->alapStartCycle < minNode->alapStartCycle) {
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

// 随机获取指定资源的节点
Node* getRequiredNode(vector<Node*> nodes, Type requiredType) {
    for (Node* node : nodes) {
        if (node->type == requiredType) {
            return node;
        }
    }
    return NULL;
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