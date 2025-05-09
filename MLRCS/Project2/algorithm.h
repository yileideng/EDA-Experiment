#pragma once
#include "graph.h"
#include <unordered_map>

// ML_RCS����Դ�����£���С�ӳ��㷨
int ML_RCS(Graph& g, unordered_map<Type, int> resouces);
// ��Լ��ʱ����ĳ���ڿ��Ե��ȵĽڵ㼯
vector<Node*> getReadyNodes(Graph& g, int currentCycle);
// ����ԴԼ���£����Ե��ȵĽڵ㼯
vector<Node*> confirmProcessNodes(vector<Node*> readyNodes, int resourceNum, Type resourceType);

// MR_LCS���ӳ������£���С��Դ�㷨
void MR_LCS(Graph& g, unordered_map<Type, int>& resouces, int requiredCycle);
// ���ӳ�Լ���£����Ե��ȵĽڵ㼯
vector<Node*> confirmProcessNodes(vector<Node*> readyNodes, int processingCounts, int currentCycle, int& resourceNum, Type resourceType);

// ��ȡ��ǰ���ڣ�ĳ����Դ�ĵ��Ƚڵ���
int getProcessingCounts(Graph& g, int currentCycle, Type resourceType);
// �����ȡһ��ָ����Դ�Ľڵ�
Node* getHighestPriorityNode(vector<Node*> nodes, Type requiredType);

// ASAP
int ASAP(Graph& g);
// ALAP
void ALAP(Graph& g, int requiredCycle);