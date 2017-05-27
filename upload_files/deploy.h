#ifndef __ROUTE_H__
#define __ROUTE_H__
#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include "lib_io.h"
#define N 1008		//不超过 1000 个网络节点
#define CN 508		//不超过 500 个消费节点
#define INF 100000000
#define random(x) (rand()%x)
using namespace std;
struct Edge
{
	int from, to, cap, flow, cost;
	Edge(int u, int v, int ca, int f, int co) :from(u), to(v), cap(ca), flow(f), cost(co) {};
};

//声明处理函数
void addedge(int from, int to, int cap, int cost);//加边函数
bool SPFA(int s, int t, int &flow, int &cost);// 寻找最小费用的增广路，使用引用同时修改原flow, cost
void handleInput();
int MincotMaxflow();//计算最小费用流主函数
void selectServer(int sumRequireFlow);//选定服务器函数;

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);

	

#endif
