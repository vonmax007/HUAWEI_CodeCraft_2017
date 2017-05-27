// costFlow.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "minFlow.h"


int main()
{
	system("mode con cols=100 lines=10000");//改变宽高
	cout << "The total flow cost is: "<<MincotMaxflow() << endl;
	//selectServer();
	return 0;
}
