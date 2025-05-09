#pragma once
#include "graph.h"

// 处理输入
void processInput(Graph& g, vector<std::string> blifContent);
// 打印每个周期节点调度情况
void printCycles(Graph& g);
// 打印输入输出节点
void printInOut(Graph& g);
// 打印图的信息
void printGraph(Graph& g);
// 将枚举转换为字符串
std::string typeToString(Type type);