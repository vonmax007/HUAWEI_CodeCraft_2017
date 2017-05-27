#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <iostream>

using namespace std;
//算法全局变量
int numNode, numEdge, numConsumer;//numNode 是节点个数，numEdge是边的数量;
int costPerServer;	//单服务器花费
int s, t;			//源点和目的点ID
vector<Edge> edges;	//存储连接边，以边为核心
vector<int> G[N];	//存储图结构，从节点i出发的边的ID
int ConsumerInfo[CN][2];//第0位表示消费点直连网络节点ID，第1位代表其需求
map <int, int>consumr_node;//存储消费点ID - 网络点ID对
vector<int> nearConsumer[CN];//存储图消费节点i临近的消费节点ID
int nodeInfo[N] = {0};	//暂时存储节点i的输出流量能力
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
vector<int> serverLocation;
void searchNear(int cnid, int &hop)
{
	queue <int> Q;		//临近队列
	Q.push(cnid);
	//int searchHop = 0;
	while (!Q.empty() && hop < 10)//跳数暂且定义为10
	{
		int u = Q.front();
		Q.pop();
		for (unsigned int i = 0; i < G[u].size(); i++)
		{
			Edge& e = edges[G[u][i]];
			if (consumr_node.find(e.to) != consumr_node.end())
			{
				//如果e.to是消费节点

			}
			;
		}
	}
}


//选定服务器函数- 紧邻搜索算法
void selectServer(int sumRequireFlow)
{
    /*
	for (int i = 0; i < numConsumer; ++i)
	{
		
    		

		for (int j = 0; j != i && j < numNode; ++j)
		{
			ConsumerInfo[j][0];//第0位表示消费点直连网络节点ID，第1位代表其需求
		}
		
        
	}*/
    /*for(int i=0;i<numNode;i++){
       int r = random(30);
       if(r <5){
          serverLocation.push_back(i);
       }
    }*/
    /*
    serverLocation.push_back(7);
    serverLocation.push_back(22);
    serverLocation.push_back(43);
    serverLocation.push_back(15);
    serverLocation.push_back(37);
    serverLocation.push_back(13);
    serverLocation.push_back(38);
    */
    serverLocation.push_back(17);
    serverLocation.push_back(6);
    serverLocation.push_back(7);
    serverLocation.push_back(41);
    serverLocation.push_back(48);
    serverLocation.push_back(13);
    serverLocation.push_back(35);
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
	G[from].push_back(m - 2); //从from出发的有向边 编号
	G[to].push_back(m - 1);   //从to出发的有向边 编号
}

//寻找最小费用的增广路，使用引用同时修改原flow,cost
bool SPFA(int s, int t, int &flow, int &cost)
{
	for (int i = 0; i < numNode + 2; i++)	//节点变了啊
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
    int first =u;
	while (1)//更新正向边和反向边
	{
        if(u == s){
            break;
        }
        first = u;
		edges[parent[u]].flow += a[t];
		flowRecord[spfa_count].push_front(parent[u]);
		edges[parent[u] ^ 1].flow -= a[t];
		u = edges[parent[u]].from;
	}
    cout << "first:"<<first<<endl;
    cout << "consumr_node:"<<consumr_node[first]<<endl;
    flowRecord[spfa_count].push_front(consumr_node[first]);
	spfa_count++;
	return true;
}



//计算最小费用流主函数
int MincotMaxflow()
{
	int flow = 0, cost = 0;
	//handleInput();		//处理输入
	selectServer(sumRequireFlow); //选择服务器位置
	//处理超级汇点
	for (int i = 0; i < serverLocation.size(); ++i)
	{
		addedge(serverLocation[i], numNode + 1, 1000, costPerServer);

        edges.push_back(Edge(serverLocation[i], numNode + 1, 1000, 0, costPerServer));
	    edges.push_back(Edge(numNode + 1, serverLocation[i], 0, 0, -costPerServer));
	    int m = edges.size();
	    G[serverLocation[i]].push_back(m - 2); //从from出发的有向边 编号
	    G[numNode + 1].push_back(m - 1);   //从to出发的有向边 编号
		//addedge(from, to, cap, percost); 上面那个我添加的流容量是1000，不知够不够
	}
    s = numNode;
    t = numNode + 1;
	//汇点连接完成
	while (SPFA(s, t, flow, cost));

	//主功能完成之后输入辅助信息
	for (int i = 0; i < spfa_count; ++i)
	{
		//cout << s << " -- "; 
        int cnt =1;
		for (list<int>::iterator it = flowRecord[i].begin();
			it != flowRecord[i].end(); ++it)
		{   
            if(cnt ==1){
                cout << *it<< "--";
            }else{
				cout << edges[*it].to << " -- ";
            }
            cnt++;
		}
		cout << " this flow value : " << flowValue[i];
		cout << endl;
	}

	cout << endl << "Toal flow count: " << spfa_count << endl << "and maximum flow : " << flow << endl;
	cout << "The require flow is :" << sumRequireFlow<<endl;
	//辅助信息完成后，输出最终开销
	return cost;
}
void handleInput()
{
	//处理输入
	FILE * fp;
	//freopen_s(&fp, "D:\\Projects\\costFlow\\03\\costFlow\\pri\\8.txt", "r", stdin);
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
		nodeInfo[from] += cap;
		nodeInfo[to] += cap;
	}

	//开始处理消费节点信息requireFlow
	int cid, nid, requireFlow;
	for (int i = 0; i < numConsumer; ++i)
	{
		cin >> cid >> nid >> requireFlow;
		ConsumerInfo[cid][0] = nid;			//0位代表直连网络节点id
		ConsumerInfo[cid][1] = requireFlow;	//1位代表需求
		consumr_node[nid] = cid;			//key是nid，对应一个cid
		//添加超级源点numNode到消费节点
		addedge(numNode, nid, requireFlow, 0);
		//addedge(from, to, cap, percost);
		sumRequireFlow += requireFlow;		//统计消费节点总需求流量
	}


	//输入结束
}

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    int consumerNum = 0;
    char *c;
    int spaceCount = 0;
    //string topo0 = topo[0];
    //cout<< "topo:" <<topo0<<endl;
    c = topo[0];
    const char* sep = " ";
    for (int i = 0; i < numNode + 2; i++)
		G[i].clear();
	edges.clear();
    int from, to, cap, percost;
    int cid, nid, requireFlow;
    for(int i=0;i<line_num;i++){
        string str = topo[i];
        
        char *cstr, *p;
	    vector<string> res;
        if(str.size()<=1){
            continue;
        }
	    cstr = new char[str.size()+1];
	    strcpy(cstr,str.c_str());
	    p = strtok(cstr,sep);
	    while(p!=NULL)
	    {
			res.push_back(p);
			p = strtok(NULL,sep);
	    }
        
        if(i ==0){
            numNode = atoi(res[0].c_str());
            numEdge = atoi(res[1].c_str());
            numConsumer = atoi(res[2].c_str());
            //cout<< "numNode"<<numNode<<endl;
			//cout<< "numEdge"<<numEdge<<endl;
			//cout<< "numConsumer"<<numConsumer<<endl;
        }else if(i == 2){
            costPerServer = atoi(res[0].c_str());
            //cout<< "costPerServer"<<costPerServer<<endl;
        }else if(i >=4 && i<=3+numEdge){
            
            from = atoi(res[0].c_str());
            to = atoi(res[1].c_str());
            cap = atoi(res[2].c_str());
            percost = atoi(res[3].c_str());
            //cout<<from<<" "<<to<<" "<<cap<<" "<<percost<<endl;
			addedge(from, to, cap, percost);
			//更新from 和 to节点输出流量能力
			nodeInfo[from] += cap;
			nodeInfo[to] += cap;
        }else if(i>4+numEdge && i<line_num){
            cid = atoi(res[0].c_str());
            nid = atoi(res[1].c_str());
            requireFlow = atoi(res[2].c_str());
            //cout<<cid<<" "<<nid<<" "<<requireFlow<<endl;
            ConsumerInfo[cid][0] = nid;			//0位代表直连网络节点id
			ConsumerInfo[cid][1] = requireFlow;	//1位代表需求
			consumr_node[nid] = cid;			//key是nid，对应一个cid
			//添加超级源点numNode到消费节点
			//addedge(numNode, nid, requireFlow, 0);
            edges.push_back(Edge(numNode, nid, requireFlow, 0, 0));
			edges.push_back(Edge(nid, numNode, 0, 0, 0));
			int m = edges.size();
			G[numNode].push_back(m - 2); //从from出发的有向边 编号
			G[nid].push_back(m - 1);   //从to出发的有向边 编号
			//addedge(from, to, cap, percost);
			sumRequireFlow += requireFlow;		//统计消费节点总需求流量
        }
    }
    cout<<MincotMaxflow()<<endl;
    string result;
   
   
    /*while (*c != '\0' && *c != '\n' && *c != '\r') 
    {
        if (*c == ' ') 
        {
            c++;
            spaceCount++;
            continue;
        }
        if (spaceCount == 2) 
        {
            consumerNum = *c - '0' + consumerNum * 10;
        }
        c++;
    }
    
    string res;
    char a[20];
    sprintf(a, "%d\n\n",consumerNum);
    res = a;
    int netnode, need;
    
    for (int i = 1; i < consumerNum+1; i++)
    {
        c = topo[line_num-i];
        netnode = need = spaceCount = 0;
        while (*c != '\0' && *c != '\n' && *c != '\r') 
        {
            if (*c == ' ') 
            {
                c++;
                spaceCount++;
                continue;
            }
            if (spaceCount == 1)
            {
                netnode = *c - '0' + netnode * 10;
            }
            else if (spaceCount == 2) 
            {
                need = *c - '0' + need * 10;
            }
            c++;
        }
        sprintf(a, "%d %d %d",netnode,consumerNum-i,need);
        res += a;
        if (i != consumerNum) 
        {
            res += "\n";
        }
    }*/
    
    char * topo_file = (char *)result.c_str();
    write_result(topo_file, filename);
}
