#pragma once
//MCMFģ��:

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

#define N 1008		//������ 1000 ������ڵ�
#define CN 508		//������ 500 �����ѽڵ�
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

int preV[N];	//��¼���ڵ�
vector <Result> bestPath;


//�㷨ȫ�ֱ���
int numNode, numEdge, numConsumer;//numNode �ǽڵ������numEdge�Ǳߵ�����;
int costPerServer;	//������������
int s, t;			//Դ���Ŀ�ĵ�ID
vector<Edge> edges;	//�洢���ӱߣ��Ա�Ϊ����
vector<int> G[N];	//�洢ͼ�ṹ���ӽڵ�i�����ıߵ�ID
int ConsumerInfo[CN][2];//��0λ��ʾ���ѵ�ֱ������ڵ�ID����1λ����������
bool nodeSelected[N] = {0};//��ʱ�洢�ڵ�i�Ƿ�ѡ��Ϊ������
bool inq[N];		//�Ƿ��ڶ�����
int d[N];			//����
int parent[N];		//��һ���������ӱ�)ID
int a[N];			//�ɸĽ���
int spfa_count = 0;	//ͳ��spfa���ô���
int flowValue[1000];//ÿ��������ֵ�����������Ǽٶ���������1000��
list<int> flowRecord[1000];//ÿ����������ID
int sumRequireFlow = 0;//�ܹ����ѽڵ���Ҫ������ֵ
int totalCost = 0;	//�ܹ��ɱ��������������ɱ�+�����ɱ�
vector<int> serverLocation;//��ѡ�������λ��
vector <NodeInfo>candidateSLI;	//��ѡ������λ����Ϣ
int consumerSatisfy[N] = { 0 };
int node_consumer[N] = { 0 };	//�洢����ڵ��Ӧ�����ѽڵ�ID
int consumer_node[CN] = { 0 };	//�洢���ѽڵ��Ӧ������ڵ�ID
int nodeCap[N] = { 0 };			//����ڵ�i���������
vector <int> nearConsumer[N];	//����ڵ�i�ٽ������ѽڵ�ID
bool nodeVisited[N] = { 0 };	//��ʾ���綥��i�Ƿ񱻷��ʹ�;
int edgeReStart = 0;
set <int> serverSet;

//������������
void addedge(int from, int to, int cap, int cost);//�ӱߺ���
bool SPFA(int s, int t, int &flow, int &cost);// Ѱ����С���õ�����·��ʹ������ͬʱ�޸�ԭflow, cost
void handleInput();
int MincotMaxflow();//������С������������
void selectServer();//ѡ������������;
void searchNear(int nidPri, int nid, int hop);//��������ڵ��ٽ����ѽڵ�;
void resetFlow();	//�����������ĺ���
void Init();		//��ʼ�����в���
void DFS(int u, Result & r, int & flow);
void dfs(int u,int& flow);


//���崦������


//��������ڵ��ٽ����ѽڵ�;
void searchNear(int nidPri, int nid, int hop)
{
	nodeVisited[nid] = true;
	if (hop >= 1)return;	//1���ڽ�ֹ,����ͳ��̸֮...
	for (unsigned int i = 0; i < G[nid].size(); ++i)
	{
		Edge& e = edges[G[nid][i]];
		if(!nodeVisited[e.to])
		{
			if (node_consumer[e.to] != -1)//�˽ڵ������ѽڵ㣬���һ�û��ѹ��ȥ
				nearConsumer[nidPri].push_back(e.to);
			searchNear(nidPri, e.to, hop + 1);
		}
	}
}

//��ʼ�����в���
void Init()
{
	serverLocation.clear();
	//��ʼ������λ��
	for (int i = 0; i < numNode; ++i)
	{
		if (node_consumer[i] != -1)
			nearConsumer[i].push_back(i);
		searchNear(i, i, 0);
		for (int j = 0; j < N; ++j)nodeVisited[j] = false;//�������غ��ھ�
		candidateSLI.push_back(NodeInfo(i, nearConsumer[i].size()));
		if (nearConsumer[i].size() >= 3)//������2�����ϵģ�����
		{
			serverLocation.push_back(i);
			nodeSelected[i] = true;
		}
	}
	cout << "\n\nthere are " << serverLocation.size() << " nodes maybe need \n\n";
}

//����ѡ������������- ���������㷨
void selectServer()
{
	for (int i = 0; i < numConsumer; ++i)
	{
		if (ConsumerInfo[i][1] > consumerSatisfy[i])	//1λ��������
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

//�ӱߺ���
void addedge(int from, int to, int cap, int cost)
{
	//����������·��
	edges.push_back(Edge(from, to, cap, 0, cost));
	edges.push_back(Edge(to, from, 0, 0, -cost));
	int m = edges.size();
	G[from].push_back(m - 2); //��from����������� ���
	G[to].push_back(m - 1);   //��to����������� ���

	//����������·��
	edges.push_back(Edge(to, from, cap, 0, cost));
	edges.push_back(Edge(from, to, 0, 0, -cost));
	m = edges.size();
	G[to].push_back(m - 2); //��from����������� ���
	G[from].push_back(m - 1);   //��to����������� ���
	//��������д���˰���to��fromд����
}

//�����������ĺ���
void resetFlow()
{
	for (vector<Edge>::iterator it = edges.begin();
		it != edges.end(); ++it)it->flow = 0;
	
	spfa_count = 0;	//ͳ��spfa���ô���
	
	for (int i = 0; i < 1000; ++i)
	{
		flowValue[i] = 0;
		flowRecord[i].clear();
	}
	memset(a, INF, sizeof(a));  //��ʼ��ΪINF
	for (int i = 0; i < CN; ++i)consumerSatisfy[i] = 0;
}

//Ѱ����С���õ�����·��ʹ������ͬʱ�޸�ԭflow,cost
bool SPFA(int s, int t, int &flow, int &cost)
{
	for (int i = 0; i < numNode + 2; i++)	//�ڵ��������˰�
		d[i] = INF;
	memset(inq, 0, sizeof(inq));  //��ʼ��Ϊ0����ʼ��û�����
	d[s] = 0;
	inq[s] = true;
	parent[s] = -1;		//s�ĸ����ӱ�Ϊ-1
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
			if (e.cap > e.flow && d[e.to] > d[u] + e.cost)//����������ҿɱ��
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
	if (d[t] == INF) return false;//��㲻�ɴ����˳�
	flow += a[t];		//�ҵ�����������ֵ
	cost += d[t] * a[t];//��������·���ķ�
	flowValue[spfa_count] = a[t];
	int u = t;
	while (u != s)//��������ߺͷ����
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
	//��ʼ������-���ѽڵ� ��
	for (unsigned int i = 0; i < N; ++i)
		node_consumer[i] = -1;
	for (unsigned int i = 0; i < CN; ++i)
		consumer_node[i] = -1;
	//��������
	FILE * fp;
	freopen_s(&fp, "F:\\f\\0HUAWEI\\case_example\\second\\case2.txt", "r", stdin);
	cin >> numNode >> numEdge >> numConsumer;
	cin >> costPerServer;
	s = numNode; t = numNode + 1;	//s �� t �̶�Ϊ������,ѡ���ĳ���Դ��ͳ������.
	//��ʼ��ͼ�ṹ���߽ṹ
	for (int i = 0; i < numNode + 2; i++)
		G[i].clear();
	edges.clear();
	//�������Ϣ
	int from, to, cap, percost;
	for (int i = 0; i < numEdge; ++i)
	{
		cin >> from >> to >> cap >> percost;
		addedge(from, to, cap, percost);
		//����from �� to�ڵ������������
		nodeCap[from] += cap;
		nodeCap[to] += cap;
	}

	//��ʼ�������ѽڵ���Ϣ
	int cid, nid, requireFlow;
	for (int i = 0; i < numConsumer; ++i)
	{
		cin >> cid >> nid >> requireFlow;
		ConsumerInfo[cid][0] = nid;			//0λ����ֱ������ڵ�id
		ConsumerInfo[cid][1] = requireFlow;	//1λ��������
		consumer_node[cid] = nid;			//����ID - ����ID
		node_consumer[nid] = cid;			//����ID - ����ID
		//���ӳ���Դ��numNode + 1�����ѽڵ�
		addedge(numNode, nid, requireFlow, 0);
		//addedge(from, to, cap, percost);
		sumRequireFlow += requireFlow;		//ͳ�����ѽڵ�����������
	}

	//�������
}

//������С������������
int MincotMaxflow()
{
	int flow = 0, cost = 0;
	handleInput();		//��������
	Init();
	
	serverLocation.clear();
	serverLocation.push_back(48);
	serverLocation.push_back(18);
	serverLocation.push_back(29);
	serverLocation.push_back(38);
	serverLocation.push_back(12);
	serverLocation.push_back(23);


	vector<Edge>temp(edges);
	//�����������,�������ڵ�
	for (unsigned int i = 0; i < serverLocation.size(); ++i)
		addedge(serverLocation[i], numNode + 1, 1000, 0);//��������1000
		//addedge(from, to, cap, percost); 
	
	//����������
	while (SPFA(s, t, flow, cost));
	//�����ļ���
	ofstream fout("result.txt");
	fout<<spfa_count;
	fout<<endl<<endl;
	//�����ļ���

	//���������֮�����������Ϣ
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
		if(ConsumerInfo[i][1] > consumerSatisfy[i])	//1λ��������
		{
			unstai++;
			cout << i << "  require more  " << ConsumerInfo[i][1] - consumerSatisfy[i] << " flow \n";
		}
	}
	cout<<"\n\n\t *** "<<unstai<<" consumer not happy\n\n";

	//����޷�����ĵ㣬������Ϣ
	cout << "\n\t********************************\n" << endl;
	selectServer(); //ѡ�������λ��
					//���������֮�����븨����Ϣ
	unstai = 0;
	resetFlow();
	flow = 0; cost = 0;
	edges.clear();
	edges.assign(temp.begin(), temp.end());//��v2��ֵ��v1
	//�����������,�������ڵ�
	for (unsigned int i = 0; i < serverLocation.size(); ++i)
		addedge(serverLocation[i], numNode + 1, 1000, 0);//��������1000
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
		if (ConsumerInfo[i][1] > consumerSatisfy[i])	//1λ��������
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
	//������Ϣ�����ɺ�������տ���
	return cost;
}





void DFS(int u, Result & r, int & flow)
{
	if(u == t)  //���е�TΪ��㣬��ʼ��uΪԴ��
	{
		r.flow = flow;	
		cout << "dfs: ";
		int k = t;//��ǰ�ڵ�
		while(k != s)
		{
			cout << k << " ";
			r.path.push_back(k);
			edges[parent[k]].flow -= flow;//���¸�����
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
			parent[v] = G[u][i];//ָ�򸸱�
			//r.path.push_back(v);//ѹ��·������
			if( edge.flow < flow )flow = edge.flow;
			//�ݹ����
			DFS(v,r,flow);
			//r.path.pop_back();
		}
	}
}

void dfs(int u,int& flow)
{
	if(u == t)  //���е�TΪ��㣬��ʼ��uΪԴ��
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