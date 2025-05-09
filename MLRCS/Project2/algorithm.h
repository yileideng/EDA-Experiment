#pragma once
#include "graph.h"
#include <unordered_map>

// ML_RCS：资源限制下，最小延迟算法
int ML_RCS(Graph& g, unordered_map<Type, int> resouces);
// 无约束时，在某周期可以调度的节点集
vector<Node*> getReadyNodes(Graph& g, int currentCycle);
// 在资源约束下，可以调度的节点集
vector<Node*> confirmProcessNodes(vector<Node*> readyNodes, int resourceNum, Type resourceType);

// MR_LCS：延迟限制下，最小资源算法
void MR_LCS(Graph& g, unordered_map<Type, int>& resouces, int requiredCycle);
// 在延迟约束下，可以调度的节点集
vector<Node*> confirmProcessNodes(vector<Node*> readyNodes, int processingCounts, int currentCycle, int& resourceNum, Type resourceType);

// 获取当前周期，某个资源的调度节点数
int getProcessingCounts(Graph& g, int currentCycle, Type resourceType);
// 随机获取一个指定资源的节点
Node* getHighestPriorityNode(vector<Node*> nodes, Type requiredType);

// ASAP
int ASAP(Graph& g);
// ALAP
void ALAP(Graph& g, int requiredCycle);