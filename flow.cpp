// flow.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "minFlow.h"


int main()
{
	system("mode con cols=100 lines=10000");//改变宽高
	cout << "The total flow cost is: "<<MincotMaxflow() << endl;
	//selectServer();
	//handleInput();
	//int count = 0;
	//int count0 = 0;
	//int count1 = 0;
	//int count2 = 0;
	//int count3 = 0;
	//int count4 = 0;
	//int count5 = 0;
	//for (int i = 0; i < numNode; ++i)
	//{
	//	if (node_consumer[i] != -1)
	//		nearConsumer[i].push_back(i);
	//	searchNear(i, i, 0);
	//	for (int j = 0; j < N; ++j)nodeVisited[j] = false;//允许有重合邻居
	//	//cout<<nearConsumer[i].size()<<endl;
	//	if(nearConsumer[i].size() == 0)count0++;
	//	else if(nearConsumer[i].size() == 1)count1++;
	//	else if(nearConsumer[i].size() == 2)count2++;
	//	else if(nearConsumer[i].size() == 3)count3++;
	//	else if(nearConsumer[i].size() == 4)count4++;
	//	else count5++;
	//}
	//cout<<count0<<endl;
	//cout<<count1<<endl;
	//cout<<count2<<endl;
	//cout<<count3<<endl;
	//cout<<count4<<endl;
	//cout<<count5<<endl;
	return 0;
}
