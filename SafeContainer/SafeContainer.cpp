// SafeContainer.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "SafeMap.h"
#include "SafeVector.h"

#include <Windows.h>

safe_map<int,int> g_map;
safe_vector<int> g_vector;

DWORD WINAPI ThreadMap1( LPVOID lpThreadParameter)
{
	int nCount=g_map.size_s();
	while (true)
	{
		safe_map<int,int>::iterator ite=g_map.begin_s();
		g_map.insert_s(++nCount,nCount);

		//map<int,int>::iterator ite=g_map.begin_s();
		for (;ite!=g_map.end_s();ite++)
		{
			printf("map:thread1:%d,%d\n",ite->first,ite->second);
		}
		Sleep(0);
	}


}

DWORD WINAPI ThreadMap2( LPVOID lpThreadParameter)
{
	while (true)
	{
		//map<int,int>::iterator ite=g_map.find_s(0);

		//for (;ite!=g_map.end_s();ite++)
		//{
		//	printf("map:thread2:%d,%d\n",ite->first,ite->second);
		//}

		safe_map<int,int>::iterator ite=g_map.begin_s();
		if (ite!=g_map.end_s())
		{
			printf("map:thread2:erase:%d,%d\n",ite->first,ite->second);
			g_map.erase_s(ite);
		}
		


		Sleep(0);
	}

}


DWORD WINAPI  ThreadVector1( LPVOID lpThreadParameter)
{
	while (true)
	{
		safe_vector<int>::iterator ite;
		for (ite=g_vector.begin_s(),ite=g_vector.begin_s();ite!=g_vector.end_s();ite++)
		{
			
			printf("vector:thread1:%d\n",*ite);
		}
		Sleep(0);
	}

}

DWORD WINAPI ThreadVector2( LPVOID lpThreadParameter)
{
	while(true)
	{
		for (safe_vector<int>::iterator ite=g_vector.begin_s();ite!=g_vector.end_s();ite++)
		{
			printf("vector:thread2:%d\n",*ite);
		}
		Sleep(0);
	}

}



int main()
{
	g_map.insert_s(0,0);
	g_map.insert_s(1,1);
	g_map.insert_s(2,2);
	g_map.insert_s(3,3);
	g_map.insert_s(4,4);
	g_map.insert_s(5,5);
	g_map.insert_s(6,6);
	g_map.insert_s(7,7);


	g_vector.push_back_s(0);
	g_vector.push_back_s(1);
	g_vector.push_back_s(2);
	g_vector.push_back_s(3);
	g_vector.push_back_s(4);
	g_vector.push_back_s(5);
	g_vector.push_back_s(6);
	g_vector.push_back_s(7);



	
	CreateThread(NULL,0,ThreadMap1,NULL,0,NULL);
	CreateThread(NULL,0,ThreadMap2,NULL,0,NULL);
	//CreateThread(NULL,0,ThreadVector1,NULL,0,NULL);
	//CreateThread(NULL,0,ThreadVector2,NULL,0,NULL);

	while (true)
	{
		Sleep(200);
	}

}



