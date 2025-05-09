#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <string>
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
    // 后继节点
    vector<Node*> next;
    // 前驱节点
    vector<Node*> pre;
    // 节点类型
    Type type;
    // 节点名称
    string name;
    // 调度开始周期，-1表示未调度
    int startCycle;
    // 操作执行时间
    int executionTime;
    // 节点调度优先级：alap的节点开始时间越早，优先级越高
    int priority;

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
    // 节点集
    map<string, Node*> nodes;
    // 图的大小
    int size;

    // 清空调度状态
    void clear() {
        for (auto& pair : nodes) {
            pair.second->startCycle = -1;
        }
    }

    // 初始化图
    Graph() : size(0) {
        size = 0;
    }
};