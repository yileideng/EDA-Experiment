#pragma once
#include "graph.h"

// ��������
void processInput(Graph& g, vector<std::string> blifContent);
// ��ӡÿ�����ڽڵ�������
void printCycles(Graph& g);
// ��ӡ��������ڵ�
void printInOut(Graph& g);
// ��ӡͼ����Ϣ
void printGraph(Graph& g);
// ��ö��ת��Ϊ�ַ���
std::string typeToString(Type type);