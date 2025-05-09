#pragma once
#include "graph.h"
#include <unordered_map>

int ML_RCS(Graph& g, unordered_map<Type, int> resouces);
vector<Node*> getUnprocessedNodes(Graph& g, int currentCycle);
vector<Node*> getProcessingNodes(vector<Node*> unprocessedNode, int resourceNum, Type resourceType);
int getCurrentCycleProcessingCounts(Graph& g, int currentCycle, Type resourceType);
Node* getRequiredNode(vector<Node*> nodes, Type requiredType);
int ASAP(Graph& g);
void ALAP(Graph& g, int requiredCycle);