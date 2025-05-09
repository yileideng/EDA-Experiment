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
    // ��̽ڵ�
    vector<Node*> next;
    // ǰ���ڵ�
    vector<Node*> pre;
    // �ڵ�����
    Type type;
    // �ڵ�����
    string name;
    // ���ȿ�ʼ���ڣ�-1��ʾδ����
    int startCycle;
    // ����ִ��ʱ��
    int executionTime;
    // �ڵ�������ȼ���alap�Ľڵ㿪ʼʱ��Խ�磬���ȼ�Խ��
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
    // �ڵ㼯
    map<string, Node*> nodes;
    // ͼ�Ĵ�С
    int size;

    // ��յ���״̬
    void clear() {
        for (auto& pair : nodes) {
            pair.second->startCycle = -1;
        }
    }

    // ��ʼ��ͼ
    Graph() : size(0) {
        size = 0;
    }
};