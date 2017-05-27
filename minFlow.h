#pragma once
//MCMF模板:

#include <queue>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <time.h>
using namespace std;

#define N 1008		//不超过 1000 个网络节点
#define CN 508		//不超过 500 个消费节点
#define INF 100000000

struct Edge
{
	int from, to, cap, flow, cost;
	Edge(int u, int v, int ca, int f, int co) :from(u), to(v), cap(ca), flow(f), cost(co) {};
};

struct NodeInfo
{
	int nodeID;
	int numNear;
	NodeInfo(int _nodeID, int _numNear) :nodeID(_nodeID), numNear(_numNear) {};
};

int unstai = 0;


struct Result
{
	int flow;
	int cost;
	list<int> path;
};

int preV[N];	//记录父节点
vector <Result> bestPath;


//算法全局变量
int numNode, numEdge, numConsumer;//numNode 是节点个数，numEdge是边的数量;
int costPerServer;	//单服务器花费
int s, t;			//源点和目的点ID
vector<Edge> edges;	//存储连接边，以边为核心
vector<int> G[N];	//存储图结构，从节点i出发的边的ID
int ConsumerInfo[CN][2];//第0位表示消费点直连网络节点ID，第1位代表其需求
bool nodeSelected[N] = {0};//暂时存储节点i是否被选定为服务器
bool inq[N];		//是否在队列中
int d[N];			//距离
int parent[N];		//上一条弧（连接边)ID
int a[N];			//可改进量
int spfa_count = 0;	//统计spfa调用次数
int flowValue[1000];//每条流的流值，在这里我们假定流不超过1000条
list<int> flowRecord[1000];//每条流流经边ID
int sumRequireFlow = 0;//总共消费节点需要的流量值
int totalCost = 0;	//总共成本，包含服务器成本+流量成本
vector<int> serverLocation;//已选择服务器位置
vector <NodeInfo>candidateSLI;	//候选服务器位置信息
int consumerSatisfy[N] = { 0 };
int node_consumer[N] = { 0 };	//存储网络节点对应的消费节点ID
int consumer_node[CN] = { 0 };	//存储消费节点对应的网络节点ID
int nodeCap[N] = { 0 };			//网络节点i的输出容量
vector <int> nearConsumer[N];	//网络节点i临近的消费节点ID
bool nodeVisited[N] = { 0 };	//表示网络顶点i是否被访问过;
int edgeReStart = 0;
set <int> serverSet;

//声明处理函数
void addedge(int from, int to, int cap, int cost);//加边函数
bool SPFA(int s, int t, int &flow, int &cost);// 寻找最小费用的增广路，使用引用同时修改原flow, cost
void handleInput();
int MincotMaxflow();//计算最小费用流主函数
void selectServer();//选定服务器函数;
void searchNear(int nidPri, int nid, int hop);//搜索网络节点临近消费节点;
void resetFlow();	//重置流参数的函数
void Init();		//初始化所有参数
void DFS(int u, Result & r, int & flow);
void dfs(int u,int& flow);


//定义处理函数


//搜索网络节点临近消费节点;
void searchNear(int nidPri, int nid, int hop)
{
	nodeVisited[nid] = true;
	if (hop >= 1)return;	//1跳内截止,经验统计之谈...
	for (unsigned int i = 0; i < G[nid].size(); ++i)
	{
		Edge& e = edges[G[nid][i]];
		if(!nodeVisited[e.to])
		{
			if (node_consumer[e.to] != -1)//此节点是消费节点，并且还没有压进去
				nearConsumer[nidPri].push_back(e.to);
			searchNear(nidPri, e.to, hop + 1);
		}
	}
}

//初始化所有参数
void Init()
{
	serverLocation.clear();
	//初始服务器位置
	for (int i = 0; i < numNode; ++i)
	{
		if (node_consumer[i] != -1)
			nearConsumer[i].push_back(i);
		searchNear(i, i, 0);
		for (int j = 0; j < N; ++j)nodeVisited[j] = false;//允许有重合邻居
		candidateSLI.push_back(NodeInfo(i, nearConsumer[i].size()));
		if (nearConsumer[i].size() >= 3)//能满足2个以上的，可以
		{
			serverLocation.push_back(i);
			nodeSelected[i] = true;
		}
	}
	cout << "\n\nthere are " << serverLocation.size() << " nodes maybe need \n\n";
}

//重新选定服务器函数- 紧邻搜索算法
void selectServer()
{
	for (int i = 0; i < numConsumer; ++i)
	{
		if (ConsumerInfo[i][1] > consumerSatisfy[i])	//1位代表需求
		{
			if (find(serverLocation.begin(),
				serverLocation.end(), consumer_node[i]) == serverLocation.end())
			{
				serverLocation.push_back(consumer_node[i]);
			}
		}
	}
	//serverLocation.erase(serverLocation.begin() + 8, serverLocation.begin() + 38);
}

//加边函数
void addedge(int from, int to, int cap, int cost)
{
	//添加上行链路边
	edges.push_back(Edge(from, to, cap, 0, cost));
	edges.push_back(Edge(to, from, 0, 0, -cost));
	int m = edges.size();
	G[from].push_back(m - 2); //从from出发的有向边 编号
	G[to].push_back(m - 1);   //从to出发的有向边 编号

	//添加下行链路边
	edges.push_back(Edge(to, from, cap, 0, cost));
	edges.push_back(Edge(from, to, 0, 0, -cost));
	m = edges.size();
	G[to].push_back(m - 2); //从from出发的有向边 编号
	G[from].push_back(m - 1);   //从to出发的有向边 编号
	//尼玛这里写错了啊，to和from写反了
}

//重置流参数的函数
void resetFlow()
{
	for (vector<Edge>::iterator it = edges.begin();
		it != edges.end(); ++it)it->flow = 0;
	
	spfa_count = 0;	//统计spfa调用次数
	
	for (int i = 0; i < 1000; ++i)
	{
		flowValue[i] = 0;
		flowRecord[i].clear();
	}
	memset(a, INF, sizeof(a));  //初始化为INF
	for (int i = 0; i < CN; ++i)consumerSatisfy[i] = 0;
}

//寻找最小费用的增广路，使用引用同时修改原flow,cost
bool SPFA(int s, int t, int &flow, int &cost)
{
	for (int i = 0; i < numNode + 2; i++)	//节点数量变了啊
		d[i] = INF;
	memset(inq, 0, sizeof(inq));  //初始化为0，开始都没有入队
	d[s] = 0;
	inq[s] = true;
	parent[s] = -1;		//s的父连接边为-1
	a[s] = INF;
	queue<int> Q;
	Q.push(s);
	while (!Q.empty())
	{
		int u = Q.front();
		Q.pop();
		inq[u] = false;
		for (unsigned int i = 0; i < G[u].size(); i++)
		{
			Edge& e = edges[G[u][i]];
			if (e.cap > e.flow && d[e.to] > d[u] + e.cost)//满足可增广且可变短
			{
				d[e.to] = d[u] + e.cost;
				parent[e.to] = G[u][i];
				a[e.to] = min(a[u], e.cap - e.flow);
				if (!inq[e.to])
				{
					inq[e.to] = true;
					Q.push(e.to);
				}
			}
		}
	}
	if (d[t] == INF) return false;//汇点不可达则退出
	flow += a[t];		//找到的新流的流值
	cost += d[t] * a[t];//更新新流路径耗费
	flowValue[spfa_count] = a[t];
	int u = t;
	while (u != s)//更新正向边和反向边
	{
		edges[parent[u]].flow += a[t];
		flowRecord[spfa_count].push_front(parent[u]);
		edges[parent[u] ^ 1].flow -= a[t];
		u = edges[parent[u]].from;
	}
	spfa_count++;
	return true;
}

void handleInput()
{
	//初始化网络-消费节点 对
	for (unsigned int i = 0; i < N; ++i)
		node_consumer[i] = -1;
	for (unsigned int i = 0; i < CN; ++i)
		consumer_node[i] = -1;
	//处理输入
	FILE * fp;
	freopen_s(&fp, "F:\\f\\0HUAWEI\\case_example\\second\\case2.txt", "r", stdin);
	cin >> numNode >> numEdge >> numConsumer;
	cin >> costPerServer;
	s = numNode; t = numNode + 1;	//s 和 t 固定为这俩数,选定的超级源点和超级汇点.
	//初始化图结构，边结构
	for (int i = 0; i < numNode + 2; i++)
		G[i].clear();
	edges.clear();
	//输入边信息
	int from, to, cap, percost;
	for (int i = 0; i < numEdge; ++i)
	{
		cin >> from >> to >> cap >> percost;
		addedge(from, to, cap, percost);
		//更新from 和 to节点输出流量能力
		nodeCap[from] += cap;
		nodeCap[to] += cap;
	}

	//开始处理消费节点信息
	int cid, nid, requireFlow;
	for (int i = 0; i < numConsumer; ++i)
	{
		cin >> cid >> nid >> requireFlow;
		ConsumerInfo[cid][0] = nid;			//0位代表直连网络节点id
		ConsumerInfo[cid][1] = requireFlow;	//1位代表需求
		consumer_node[cid] = nid;			//消费ID - 网络ID
		node_consumer[nid] = cid;			//网络ID - 消费ID
		//添加超级源点numNode + 1到消费节点
		addedge(numNode, nid, requireFlow, 0);
		//addedge(from, to, cap, percost);
		sumRequireFlow += requireFlow;		//统计消费节点总需求流量
	}

	//输入结束
}

//计算最小费用流主函数
int MincotMaxflow()
{
	int flow = 0, cost = 0;
	handleInput();		//处理输入
	Init();
	
	serverLocation.clear();
	serverLocation.push_back(48);
	serverLocation.push_back(18);
	serverLocation.push_back(29);
	serverLocation.push_back(38);
	serverLocation.push_back(12);
	serverLocation.push_back(23);


	vector<Edge>temp(edges);
	//处理超级汇点,服务器节点
	for (unsigned int i = 0; i < serverLocation.size(); ++i)
		addedge(serverLocation[i], numNode + 1, 1000, 0);//流容量是1000
		//addedge(from, to, cap, percost); 
	
	//汇点连接完成
	while (SPFA(s, t, flow, cost));
	//测试文件流
	ofstream fout("result.txt");
	fout<<spfa_count;
	fout<<endl<<endl;
	//测试文件流

	//主功能完成之后输出辅助信息
	for (int i = 0; i < spfa_count; ++i)
	{
		cout << s << " -- ";
		list<int>::iterator it = flowRecord[i].begin();
		consumerSatisfy[node_consumer[edges[*it].to]] += flowValue[i];
		for (int j = 0;j < flowRecord[i].size()-1; ++it,++j)
		{
			cout << edges[*it].to << " -- ";
			fout << edges[*it].to << " ";
		}
		cout << edges[*it].to; 
		//consumerSatisfy[node_consumer[edges[*it].from]] += flowValue[i];
		cout << "  - flow value : " << flowValue[i];
		fout << flowValue[i];
		cout << endl;
		fout<<endl;
	}
	fout.close();
	cout << endl << "Toal flow count: " << spfa_count << endl << "and maximum flow : " << flow << endl;
	cout << "The require flow is :" << sumRequireFlow<<endl;

	for (int i = 0; i < numConsumer; ++i)
	{
		if(ConsumerInfo[i][1] > consumerSatisfy[i])	//1位代表需求
		{
			unstai++;
			cout << i << "  require more  " << ConsumerInfo[i][1] - consumerSatisfy[i] << " flow \n";
		}
	}
	cout<<"\n\n\t *** "<<unstai<<" consumer not happy\n\n";

	//针对无法满足的点，重连信息
	cout << "\n\t********************************\n" << endl;
	selectServer(); //选择服务器位置
					//主功能完成之后输入辅助信息
	unstai = 0;
	resetFlow();
	flow = 0; cost = 0;
	edges.clear();
	edges.assign(temp.begin(), temp.end());//将v2赋值给v1
	//处理超级汇点,服务器节点
	for (unsigned int i = 0; i < serverLocation.size(); ++i)
		addedge(serverLocation[i], numNode + 1, 1000, 0);//流容量是1000
	while (SPFA(s, t, flow, cost));
	for (int i = 0; i < spfa_count; ++i)
	{
		cout << s << " -- ";
		list<int>::iterator it = flowRecord[i].begin();
		consumerSatisfy[node_consumer[edges[*it].to]] += flowValue[i];
		for (int j = 0; j < flowRecord[i].size() - 1; ++it, ++j)
		{
			cout << edges[*it].to << " -- ";
		}cout << edges[*it].to;
		//consumerSatisfy[node_consumer[edges[*it].from]] += flowValue[i];
		cout << "  - flow value : " << flowValue[i];
		cout << endl;
	}

	cout << endl << "Toal flow count: " << spfa_count << endl << "and maximum flow : " << flow << endl;
	cout << "The require flow is :" << sumRequireFlow << endl;

	for (int i = 0; i < numConsumer; ++i)
	{
		if (ConsumerInfo[i][1] > consumerSatisfy[i])	//1位代表需求
		{
			unstai++;
			cout << i << "  require more  " << ConsumerInfo[i][1] - consumerSatisfy[i] << " flow \n";
		}	
	}
	cout<<"\n\n\t *** "<<unstai<<" consumer not happy\n\n";

	/*int u = 50;
	for(int i = 0; i < G[u].size(); ++i)
	{
		if(edges[G[u][i]].flow > 0 && edges[G[u][i]].cost >= 0)
			cout<<G[u][i]<<" - flow > 0 :"<<edges[G[u][i]].flow<<endl;
	}*/

	Result r;
	int flowt = INF;
	DFS(s, r, flowt);
	//辅助信息输出完成后，输出最终开销
	return cost;
}





void DFS(int u, Result & r, int & flow)
{
	if(u == t)  //其中的T为汇点，初始的u为源点
	{
		r.flow = flow;	
		cout << "dfs: ";
		int k = t;//当前节点
		while(k != s)
		{
			cout << k << " ";
			r.path.push_back(k);
			edges[parent[k]].flow -= flow;//更新父边流
			k = preV[k];
		}
		cout << k << " ";
		cout<<" - flow is: "<<flow<<endl;
		flow = INF;
		bestPath.push_back(r);
		r.path.clear();
		return;
	}
	for(int i = 0; i < G[u].size(); ++i)
	{
		if(edges[G[u][i]].flow > 0 && edges[G[u][i]].cost >= 0)
		{	
			Edge edge = edges[G[u][i]];
			int v = edge.to;
			preV[v] = u;
			parent[v] = G[u][i];//指向父边
			//r.path.push_back(v);//压入路径序列
			if( edge.flow < flow )flow = edge.flow;
			//递归遍历
			DFS(v,r,flow);
			//r.path.pop_back();
		}
	}
}

void dfs(int u,int& flow)
{
	if(u == t)  //其中的T为汇点，初始的u为源点
	{
		Result r;
		r.flow = flow;
		//cout << "dfs ";
		int k;
		int curCost =0;
		for( k=t;preV[k]!=s ; k=preV[k])
		{    
			int i = parent[k];
			//cout << k << " ";
			if(k != t){
				//cout << "edges[i].cost:"<<edges[i].cost<<endl;
				curCost += flow * edges[i].cost;
				r.path.push_back(k);
			}else{
				serverSet.insert(preV[k]);
			}
			edges[i].flow -= flow;
		}
		//cout << "curCost:"<< curCost<<endl;
		r.cost = curCost;
		r.path.push_back(k);
		r.path.push_back(node_consumer[k]);
		bestPath.push_back(r);
		edges[parent[k]].flow -= flow;
		//cout << k << " "<<node_consumer[k] <<endl;
		flow = INF;
		return;
	}
	for(int i=0;i<G[u].size();i++){
		Edge edge = edges[G[u][i]];
		int v = edge.to;

		if(edge.flow >0){
			//cout << "edges[G[u][i]] cost" << edge.cost<<endl;
			preV[v] = u;
			parent[v] = G[u][i];
			//r.path.push_back(v);
			if(edge.flow < flow){
				flow = edge.flow;
			}
			dfs(v,flow);
			//r.path.pop_back();
		}
	}
}
