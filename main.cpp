//MCMF模板：
#include <stdio.h>
#include <queue>
#include <cstring>
#include <iostream>
#include <vector>
#include <time.h>
using namespace std;

#define N 1000
#define INF 100000000

struct Edge
{
    int from,to,cap,flow,cost;
    Edge(int u,int v,int ca,int f,int co):from(u),to(v),cap(ca),flow(f),cost(co) {};
};

class MCMF
{
public:
    MCMF() {}
    ~MCMF() {}


public:

    int n,m,s,t;
    vector<Edge> edges;
    vector<int> G[N];
    int inq[N];//是否在队列中
    int d[N];//距离
    int p[N];//上一条弧
    int a[N];//可改进量

    void init(int n)//初始化
    {
        this->n=n;
        for(int i=0; i<n; i++)
            G[i].clear();
        edges.clear();
    }

    void addedge(int from,int to,int cap,int cost)//加边
    {
        //添加上行链路边
        // from,to,cap,flow,cost;
        edges.push_back(Edge(from,to,cap,0,cost));
        edges.push_back(Edge(to,from,0,0,-cost));
        int m = edges.size();
        G[from].push_back(m-2); //从from出发的有向边 编号
        G[to].push_back(m-1);   //从to出发的有向边 编号

        //添加下行链路边
        /*
        edges.push_back(Edge(to,from,cap,0,cost));
        edges.push_back(Edge(from,to,0,0,-cost));
        m = edges.size();
        G[from].push_back(m-2); //从from出发的有向边 编号
        G[to].push_back(m-1);   //从to出发的有向边 编号
        */
    }

    bool SPFA(int s,int t,int &flow,int &cost)//寻找最小费用的增广路，使用引用同时修改原flow,cost
    {
        for(int i=0; i<n; i++)
            d[i]=INF;
        memset(inq,0,sizeof(inq));  //初始化为0
        d[s]=0;
        inq[s]=1;
        p[s]=0;
        a[s]=INF;
        queue<int> Q;
        Q.push(s);
        while(!Q.empty())
        {
            int u=Q.front();
            Q.pop();
            inq[u]--;
            for(int i=0; i<G[u].size(); i++)
            {
                Edge& e=edges[G[u][i]];
                if(e.cap>e.flow && d[e.to]>d[u]+e.cost)//满足可增广且可变短
                {
                    d[e.to]=d[u]+e.cost;
                    p[e.to]=G[u][i];
                    a[e.to]=min(a[u],e.cap-e.flow);
                    if(!inq[e.to])
                    {
                        inq[e.to]++;
                        Q.push(e.to);
                    }
                }
            }
        }
        if(d[t]==INF) return false;//汇点不可达则退出
        flow+=a[t];
        cost+=d[t]*a[t];
        int u=t;
        while(u!=s)//更新正向边和反向边
        {
            edges[p[u]].flow+=a[t];
            //řcout<<edges[p[u]].flow;
            edges[p[u]^1].flow-=a[t];
            u=edges[p[u]].from;
        }
        return true;
    }

    int MincotMaxflow()
    {
        int flow=0,cost=0;
        freopen("input","r",stdin);
        cin>>n>>m;
        cin>>s>>t;
        init(n);
        int from, to, cap, percost;
        for(int i=0;i<m;++i)
        {
            cin>>from>>to>>cap>>percost;
            addedge( from, to, cap, percost);
        }


        while(SPFA(s,t,flow,cost));
        int u = t;
        while(u!=s)//更新正向边和反向边
        {
            cout<<edges[p[u]].from<<" -- ";
            u=edges[p[u]].from;
        }cout<<endl<<flow<<endl;
        return cost;
    }
};


int main()
{
    MCMF mcmf;
    cout<<mcmf.MincotMaxflow()<<endl;


    return 0;
}
