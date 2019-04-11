

// huawei.cpp : Defines the entry point for the console application.
//

//#include"stdafx.h"
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<stdlib.h>
#include<vector>
#include <queue>
#include<limits>
#include<string.h> 
#include<math.h>
#include<ctime>
#include<climits>
#include<time.h>
#define MAX_ROAD 10000
#define MAX_CROSS 10000
#define MAX_CAR 100000
using namespace std;
struct Cross;
struct Car;
struct Road;

int system_Time = 0;
int answer_system_Time = numeric_limits<int>::max();
int answer_cars_Time = numeric_limits<int>::max();

struct Cross {
	static int totCross;
	int newid;//把旧id规范为0~totCross,用于求最短路 
	int id;
	Road *r[4];
	int roadId[4];
	Car *cars[10000]; int carNum;
	Cross()
	{
		this->r[0] = this->r[1] = this->r[2] = this->r[3] = NULL;
		this->carNum = 0;
	}

}; int Cross::totCross = -1; Cross cross[MAX_CROSS];
bool operator<(const Cross& a, const Cross& b) { return a.id < b.id; }

struct Road {
	static int totRoad;
	//int newid;//把旧id规范为0~totRoad
	int id, length, maxv, lanes;
	//	int startId, endId;
	struct Cross *start, *end;
	bool isDuplex;
	Road *Did, *Lid, *Rid;
	Road *rDid, *rLid, *rRid;
	vector<vector<Car*> > Cars;    //正向车辆 
	vector<vector<Car*> > rCars;    //逆向车辆 
	//Car *cars[1000]; int carNum;

	Road()
	{
		this->start = NULL; this->end = NULL; this->Did = NULL; this->Lid = NULL; this->Rid = NULL;
		this->rDid = NULL; this->rLid = NULL; this->rRid = NULL;
		//this->carNum = 0;
	}
	void init(int id, int length, int maxv, int lanes, int startId, int endId, int duplex) {
		this->id = id; this->length = length; this->maxv = maxv; this->lanes = lanes; //this->startId = startId; this->endId = endId;
		for (int j = 0; j <= Cross::totCross; ++j)
		{
			if (startId == cross[j].id)
				this->start = &cross[j];
			if (endId == cross[j].id)
				this->end = &cross[j];
		}

		if (duplex == 1) this->isDuplex = true; else this->isDuplex = false;
		for (int i = 0; i < lanes; ++i)
		{
			vector<Car*> vCar;
			Cars.push_back(vCar);
		}

		if (duplex == 1)
			for (int i = 0; i < lanes; ++i)
			{
				vector<Car*> vCar;
				rCars.push_back(vCar);
			}
	}
};
int Road::totRoad = -1; Road road[MAX_ROAD];
//bool operator<(const Road& a, const Road& b){ return a.id < b.id; }


struct Car {
	bool isFinished;  //是否到达终点站
	bool isWaiting;          //是否是等待状态 
	//vector<vector<int> > k_shrotest; //K最短路
	//vector<int> car_route;
	vector<vector<Road *> > k_myRoad;  //K最短路
	vector<int > k_time;
	vector<Road *> myRoad;
	int index;//指向当前车子到达的道路 
	vector<Road *> answerRoad;//当前最优的路线
	int answer_real_time;//当前最优的出发时间

	static int K;//k最短路径
	static int totCar;
	//int newid;//把旧id规范为0~totCar
	int id;
	int maxv;
	Cross *start, *end;//起始点和终点 
	//bool finished;   //是否已规划路线
	//Road *road[1000]; int roadNum;//路线及道路数
	//int nowRoad;//在模拟期间正在哪条道路上行驶,road[]的下标
	int dist;//在模拟期间,距当前道路下一路口距离为 dist 
	int time;//计划出发时间
	int real_time;//实际出发时间
	int end_time;//实际到达时间
	Car()
	{
		this->start = this->end = NULL;
		//	finished = false;
		isWaiting = false;
		isFinished = false;
	}
	void init(int id, int startCrossId, int endCrossId, int maxv, int time)
	{
		this->id = id; this->maxv = maxv; this->time = time;

		for (int i = 0; i <= Cross::totCross; ++i)
		{
			if (startCrossId == cross[i].id)
			{
				this->start = &cross[i];
				int carNum = ++cross[i].carNum;
				cross[i].cars[carNum] = this;

				//排序
				Car *tempCar;
				for (int j = carNum - 1; j >= 1; --j)
					if (cross[i].cars[j + 1]->id<cross[i].cars[j]->id)
					{
						tempCar = cross[i].cars[j + 1]; cross[i].cars[j + 1] = cross[i].cars[j]; cross[i].cars[j] = tempCar;
					}
					else break;

			}
			if (endCrossId == cross[i].id) this->end = &cross[i];
		}
	}
}; int Car::totCar = -1; Car car[MAX_CAR]; int Car::K = 35;   //K等于20的时候882，30，870 
bool operator<(const Car& a, const Car& b) { return a.id < b.id; }


ostream & operator<<(ostream &out, Cross &tempCross)
{
	//重载操作符，打印Cross信息 
	out << "id:" << tempCross.id << "  newId:" << tempCross.newid << "  相邻道路编号：";
	for (int i = 0; i < 4; ++i)
		if (tempCross.r[i] != NULL)
			out << (*(tempCross.r[i])).id << "  ";
		else out << -1 << " ";
		out << "  车辆数：" << tempCross.carNum;
		out << "  车的id:";
		for (int i = 1; i <= tempCross.carNum; ++i)
			out << (*(tempCross.cars[i])).id << "  ";
		return out;
}
ostream & operator<<(ostream &out, Road &tempRoad)
{
	//重载操作符，打印Cross信息 
	out << "id:" << tempRoad.id << "  " << (tempRoad.start)->id << "->" << (tempRoad.end)->id << "  是否双向：" << tempRoad.isDuplex << endl;

	out << "          " << (tempRoad.start)->id << "->" << (tempRoad.end)->id << "方向  左转：" << (tempRoad.Lid == NULL ? -1 : (tempRoad.Lid)->id)\
		<< "  直行：" << (tempRoad.Did == NULL ? -1 : (tempRoad.Did)->id) << "  右转：" << (tempRoad.Rid == NULL ? -1 : (tempRoad.Rid)->id) << endl;
	out << "          " << (tempRoad.end)->id << "->" << (tempRoad.start)->id << "方向  左转：" << (tempRoad.rLid == NULL ? -1 : (tempRoad.rLid)->id)\
		<< "  直行：" << (tempRoad.rDid == NULL ? -1 : (tempRoad.rDid)->id) << "  右转：" << (tempRoad.rRid == NULL ? -1 : (tempRoad.rRid)->id) << endl;
	//	out <<"          "<<(tempRoad.end)->id<<"->"<<(tempRoad.start)->id<<"方向  左转："<<(tempRoad.rLid)->id<<"  直行："<<(tempRoad.rDid)->id<<"  右转："<<(tempRoad.rRid)->id<<endl;
	/*out << "       车辆数：" << tempRoad.carNum;
	out << "  车的id:";
	for (int i = 1; i <= tempRoad.carNum; ++i)
	out << (*(tempRoad.cars[i])).id << "  ";*/
	return out;
}


void readData(string carPath,string roadPath,string crossPath) {
	/////////////////////////////////////////////////////////////////读取Cross.txt 
	ifstream in(crossPath.c_str());
	string s;
	while (getline(in, s))
	{
		if (s.size() >= 1 && s[0] == '#') continue;
		Cross::totCross++;
		stringstream ins;
		ins << s.substr(1, s.size() - 2);


		string data;
		int id, r0, r1, r2, r3;
		int tot = -1;
		while (getline(ins, data, ','))
		{
			stringstream inNum;
			tot++;
			inNum << data;
			switch (tot)
			{
			case 0:
				inNum >> id; break;
			case 1:
				inNum >> r0; break;
			case 2:
				inNum >> r1; break;
			case 3:
				inNum >> r2; break;
			case 4:
				inNum >> r3; break;
			}
		}
		cross[Cross::totCross].id = id;
		cross[Cross::totCross].roadId[0] = r0;
		cross[Cross::totCross].roadId[1] = r1;
		cross[Cross::totCross].roadId[2] = r2;
		cross[Cross::totCross].roadId[3] = r3;

	}

	in.close();
	in.clear();

	sort(cross, cross + Cross::totCross + 1);//把cross 按 id 升序排列  


	/////////////////////////////////////////////////////////////////读取Road.txt 
	in.open(roadPath.c_str());

	while (getline(in, s))
	{
		if (s.size() >= 1 && s[0] == '#') continue;
		Road::totRoad++;
		stringstream ins;
		ins << s.substr(1, s.size() - 2);

		string data;
		int id, length, maxv, lanes, startId, endId, Duplex;
		int tot = -1;
		while (getline(ins, data, ','))
		{
			tot++;
			stringstream inNum;
			inNum << data;
			switch (tot)
			{
			case 0:
				inNum >> id; break;
			case 1:
				inNum >> length; break;
			case 2:
				inNum >> maxv; break;
			case 3:
				inNum >> lanes; break;
			case 4:
				inNum >> startId; break;
			case 5:
				inNum >> endId; break;
			case 6:
				inNum >> Duplex; break;

			}
		}
		road[Road::totRoad].init(id, length, maxv, lanes, startId, endId, Duplex);

	}
	in.close();
	in.clear();


	//更新 cross 中的 Road *r[4] 信息
	for (int i = 0; i <= Cross::totCross; ++i)
	{
		if (cross[i].roadId[0] != -1)
			for (int j = 0; j <= Road::totRoad; ++j)
				if (road[j].id == cross[i].roadId[0])
				{
					cross[i].r[0] = &road[j];
					break;
				}
		if (cross[i].roadId[1] != -1)
			for (int j = 0; j <= Road::totRoad; ++j)
				if (road[j].id == cross[i].roadId[1])
				{
					cross[i].r[1] = &road[j];
					break;
				}
		if (cross[i].roadId[2] != -1)
			for (int j = 0; j <= Road::totRoad; ++j)
				if (road[j].id == cross[i].roadId[2])
				{
					cross[i].r[2] = &road[j];
					break;
				}
		if (cross[i].roadId[3] != -1)
			for (int j = 0; j <= Road::totRoad; ++j)
				if (road[j].id == cross[i].roadId[3])
				{
					cross[i].r[3] = &road[j];
					break;
				}
	}

	//////////////////////////////////////////////////////////////////读取Car.txt 
	in.open(carPath.c_str());
	while (getline(in, s))
	{
		if (s.size() >= 1 && s[0] == '#') continue;
		//cout<<s<<endl;
		Car::totCar++;

		stringstream ins;
		ins << s.substr(1, s.size() - 2);

		string data;
		int id, start, end, maxv, time;
		int tot = -1;
		while (getline(ins, data, ','))
		{
			stringstream inNum;
			tot++;
			inNum << data;
			switch (tot)
			{
			case 0:
				inNum >> id; break;
			case 1:
				inNum >> start; break;
			case 2:
				inNum >> end; break;
			case 3:
				inNum >> maxv; break;
			case 4:
				inNum >> time; break;
			}
		}

		car[Car::totCar].init(id, start, end, maxv, time);
	}

	in.close();
	in.clear();
}

void print()
{

	cout << "交叉口数量(0开始计数)：" << Cross::totCross << endl;
	for (int i = 0; i <= Cross::totCross; ++i)
		cout << "    " << cross[i] << endl;
	cout << "道路数量(0开始计数)：" << Road::totRoad << endl;
	for (int i = 0; i <= Road::totRoad; ++i)
		cout << "    " << road[i] << endl;
}

void makeMap() {
	int i,j;
	//连接每条道路的正方向和反方向的左转、直行、右转的道路 
	for (i = 0; i <= Road::totRoad; ++i)
	{
		Cross *s = road[i].start;
		Cross *t = road[i].end;
		//正方向
		for (j = 0; j < 4; ++j)
			if ((t->r[j]) == &road[i])
				break;

		if (t->r[(j + 1) % 4] != NULL && ((t->r[(j + 1) % 4])->isDuplex || (t->r[(j + 1) % 4])->start == t))
			road[i].Lid = t->r[(j + 1) % 4];//左转 
		if (t->r[(j + 2) % 4] != NULL && ((t->r[(j + 2) % 4])->isDuplex || (t->r[(j + 2) % 4])->start == t))
			road[i].Did = t->r[(j + 2) % 4];//直行
		if (t->r[(j + 3) % 4] != NULL && ((t->r[(j + 3) % 4])->isDuplex || (t->r[(j + 3) % 4])->start == t))
			road[i].Rid = t->r[(j + 3) % 4];//右转 

		if (road[i].isDuplex == false) continue;

		//反方向
		t = s;
		for (j = 0; j < 4; ++j)
			if (t->r[j] == &road[i])
				break;
		if (t->r[(j + 1) % 4] != NULL && ((t->r[(j + 1) % 4])->isDuplex || (t->r[(j + 1) % 4])->start == t))
			road[i].rLid = t->r[(j + 1) % 4];//左转 
		if (t->r[(j + 2) % 4] != NULL && ((t->r[(j + 2) % 4])->isDuplex || (t->r[(j + 2) % 4])->start == t))
			road[i].rDid = t->r[(j + 2) % 4];//直行
		if (t->r[(j + 3) % 4] != NULL && ((t->r[(j + 3) % 4])->isDuplex || (t->r[(j + 3) % 4])->start == t))
			road[i].rRid = t->r[(j + 3) % 4];//右转       

	}
}

struct Edge {
	int NodeId;//边的终止点id   边就是道路，NodeId就是cross.newid
	double length;//边的长度，也就是车子在道路上通过的时间
	Road *real_road;//这条边实际代表的道路
};
vector<Edge> Adj[MAX_CROSS];//cross的邻接表
vector<Edge> rAdj[MAX_CROSS];//逆图，用于求最短路


//构建路口的邻接表以及逆图
void make_graph()
{
	Edge edge;int i,j;
	for (i = 0; i <= Cross::totCross; ++i)
	{
		for (j = 0; j < 4; ++j)
			if (cross[i].r[j] != NULL)
			{
				Road *tempRoad = cross[i].r[j];

				//只考虑start是该路口的道路
				if (tempRoad->start == &cross[i])
				{
					int u = (tempRoad->start)->newid;
					int v = (tempRoad->end)->newid;
					edge.NodeId = v;
					edge.real_road = tempRoad;
					Adj[u].push_back(edge);

					edge.NodeId = u;
					rAdj[v].push_back(edge);

					if (tempRoad->isDuplex == true)
					{
						v = (tempRoad->start)->newid;
						u = (tempRoad->end)->newid;
						edge.NodeId = v;
						edge.real_road = tempRoad;
						Adj[u].push_back(edge);

						edge.NodeId = u;
						rAdj[v].push_back(edge);
					}

				}

			}
	}
}
//计算邻接表中边的length：根据道路的最高限速和特定车子的限速
void update_graph(const Car &tempCar)
{
	int i,j,len,v_road;
	for (i = 0; i <= Cross::totCross; ++i)
		for (j = 0; j < Adj[i].size(); ++j)
		{
			len = (Adj[i][j].real_road)->length;
			v_road = (Adj[i][j].real_road)->maxv;
			Adj[i][j].length = len * 1.0 / min(tempCar.maxv, v_road);

		}
	for (i = 0; i <= Cross::totCross; ++i)
		for (j = 0; j < rAdj[i].size(); ++j)
		{
			len = (rAdj[i][j].real_road)->length;
			v_road = (rAdj[i][j].real_road)->maxv;
			rAdj[i][j].length = len * 1.0 / min(tempCar.maxv, v_road);
		}
}


//dijkstra 算法 找出各个点到T的最短距离
bool mark[MAX_CROSS];
double dist[MAX_CROSS];
struct cmp
{
    bool operator()(int a,int b)
    {
        return dist[a]>dist[b];
    }
};

void dijkstra(int T)
{
	memset(mark, false, sizeof(mark));
	int i,j,k,u,v;
	for (i = 0; i <= Cross::totCross; i++)
		dist[i] = numeric_limits<double>::max();
	dist[T] = 0;
	priority_queue<int,vector<int>,cmp> QQ;
	QQ.push(T);
	
	while(!QQ.empty())
	{
		u=QQ.top();
        QQ.pop();
        if (mark[u]) continue;
        mark[u]=true;
        for (k = 0; k < rAdj[u].size(); k++)
		{
			v = rAdj[u][k].NodeId;
			if (!mark[v] && dist[v] > dist[u] + rAdj[u][k].length)
			{
				dist[v] = dist[u] + rAdj[u][k].length;
				QQ.push(v);
			}	
		}
	}
	
	
/*	
	int u, v; double minf;
	while (1)
	{
		u = -1, minf = numeric_limits<double>::max();
		for (int i = 0; i <= Cross::totCross; i++)
			if (!mark[i] && dist[i] < minf)
			{
				minf = dist[i];
				u = i;
			}
		if (u == -1) break;
		mark[u] = true;
		for (int k = 0; k < rAdj[u].size(); k++)
		{
			v = rAdj[u][k].NodeId;
			if (!mark[v] && dist[v] > dist[u] + rAdj[u][k].length)
				dist[v] = dist[u] + rAdj[u][k].length;
		}
	}*/
}

struct Node {
	int NodeId; double length;//cross的newId以及从原点到newId的时间
	//vector<int> route;
	vector<Road *> myRoute;
	vector<int> cross;
	Node() {};
	Node(int id, double len) :NodeId(id), length(len) { cross.push_back(id); }
	//判断路径是否包含环，需要判断是否经过同一个路口
	bool isCycle(int crossNewId)
	{
		for (int i = 0; i < cross.size(); ++i)
			if (crossNewId == cross[i])
				return true;
		return false;
	}
};
bool operator < (const Node& a, const Node& b) { return (a.length + dist[a.NodeId] > b.length + dist[b.NodeId]); }
ostream & operator<<(ostream &out, Node &tempNode)
{
	out << "时间为：" << tempNode.length << "   路线为：";
	for (int i = 0; i < (tempNode.myRoute).size(); ++i)
		out << (tempNode.myRoute)[i]->id << " ";
	return out;
}

//A*算法 计算某车辆由S点到T点的K最短路
int cnt[MAX_CROSS];//记录每个节点出现了几次，如果T节点出现了K次，则找到了K条路径
double Astar(Car& tempCar, int S, int T)
{
	if (dist[S] == numeric_limits<double>::max()) return -1;
	memset(cnt, 0, sizeof(cnt));
	priority_queue<Node> Q;
	Q.push(Node(S, 0));int i,j;
	while (!Q.empty())
	{
		Node nowNode = Q.top();
		double nowLen = nowNode.length;
		int nowId = nowNode.NodeId;
		Q.pop();
		cnt[nowId]++;
		if (nowId == T)
		{
			//vector<int> route(nowNode.route);
			//tempCar.k_shrotest.push_back(route);
			vector<Road *> tempMyRoute(nowNode.myRoute);
			tempCar.k_myRoad.push_back(tempMyRoute);
			tempCar.k_time.push_back(nowLen);
		}
		if (cnt[T] == Car::K)
		{
			return nowLen;
		}

		if (cnt[nowId] > Car::K)
			continue;
		for (i = 0; i < Adj[nowId].size(); i++)
		{
			Node newNode(Adj[nowId][i].NodeId, nowLen + Adj[nowId][i].length);
			if (nowNode.isCycle(Adj[nowId][i].NodeId)) continue;// { cout << nowNode << endl; cout << (Adj[nowId][i].real_road)->id << endl; system("pause"); continue; }    //判断是否出现回路

			for (j = 0; j < (nowNode.myRoute).size(); ++j)
				(newNode.myRoute).push_back((nowNode.myRoute)[j]);
			(newNode.myRoute).push_back(Adj[nowId][i].real_road);

			for (j = 0; j < (nowNode.cross).size(); ++j)
				(newNode.cross).push_back((nowNode.cross)[j]);

			Q.push(newNode);
		}

	}
	return -1;
}

void k_shortest()
{
	//更新 cross 的新id
	int i;
	for (i = 0; i <= Cross::totCross; ++i)
		cross[i].newid = i;

	//构建邻接表和逆图
	make_graph();
	for (i = 0; i <= Car::totCar; ++i)
	{
		//计算每辆车在道路上的行驶时间
		update_graph(car[i]);

		//利用逆图找出所有节点到T节点的最短路
		dijkstra((car[i].end)->newid);
		//A*算法 
		Astar(car[i], (car[i].start)->newid, (car[i].end)->newid);
		/*
		for (int j = 0; j < (car[i].k_myRoad).size(); ++j)
		{
		cout << "指针表示：车辆id:" << car[i].id << "   第" << j << "最短路径：";
		for (int k = 0; k < (car[i].k_myRoad)[j].size(); ++k)
		cout << ((car[i].k_myRoad)[j][k])->id << " ";
		cout << endl;

		}*/
		//	system("pause");

	}

}



//尝试行驶temproad中的tempLane车道中的第k辆车 
void runCar(Road *tempRoad, vector<Car *> &tempLane, int k) {
	if(tempLane[k]->isWaiting==false) return ;
	//cout<<"行驶"<<endl;system("pause");
	if (k == 0)
	{
		cout<<"不应该出现k=0"<<endl;system("pause");return;
	}
	Car *tempCar = tempLane[k];
	Car *frontCar = tempLane[k - 1];
	if (frontCar->isWaiting == true)   //如果前一辆车是wait 状态 
	{
		if ((tempCar->dist - frontCar->dist - 1) >= min(tempRoad->maxv, tempCar->maxv))   //如果距前一辆足够远 
		{
			tempCar->dist = tempCar->dist - min(tempRoad->maxv, tempCar->maxv);
			tempCar->isWaiting = false;
		}
		else
			tempCar->isWaiting = true;
	}
	else   //如果前一辆是 terminated状态 
	{
		int v = min(tempRoad->maxv, tempCar->maxv);
		int dist = min(v, tempCar->dist - frontCar->dist - 1);
		tempCar->dist = tempCar->dist - dist;
		tempCar->isWaiting = false;
	}


}

int getS2(Car *tempCar, Road *tempRoad){
//	cout<<"getS2"<<endl;
	int v1 = min(tempRoad->maxv, tempCar->maxv);
	if (tempCar->dist >= v1) return 0;

	int s1 = tempCar->dist;
	//if (tempCar->myRoad[tempCar->myRoad.size() - 1] == tempRoad) return 0;
	int i=tempCar->index;
//	for (i = 0; i < tempCar->myRoad.size(); ++i)
//		if (tempCar->myRoad[i] == tempRoad) break;
		
	if(i>=tempCar->myRoad.size()) {cout<<"Wrong tempCar->myRoad.size()"<<endl;system("pause");}
	Road *nextRoad = tempCar->myRoad[i + 1];
	int v2 = min(nextRoad->maxv, tempCar->maxv);
	int maxs2 = min(v2, max(v2 - s1, 0));
	return maxs2;

}

//确定 tempRoad 路上所有车道上的车辆状态
void checkStateOfOneRoad(Road *tempRoad, vector<vector<Car *> > &Cars)
{
//	cout<<"check"<<endl;
	int i,j;
	for (i = 0; i < Cars.size(); ++i)
	{
		/*从前往后遍历 i车道的车 */
		if (Cars[i].size() == 0)   //如果一辆车都没有 
			continue;
		if (Cars[i][0]->dist < min(Cars[i][0]->maxv, tempRoad->maxv))
		{
			Cars[i][0]->isWaiting = true;
		}
		else
		{
			Cars[i][0]->dist = Cars[i][0]->dist - min(Cars[i][0]->maxv, tempRoad->maxv);
			Cars[i][0]->dist = max(Cars[i][0]->dist, 0);
			Cars[i][0]->isWaiting = false;
		}

		//尝试开后面的车，并确定车的状态 
		for (j = 1; j<Cars[i].size(); ++j)
			runCar(tempRoad, Cars[i], j);
	}
}

//遍历每条道路,确定每辆车的状态，下一节点是终点站且在本时刻能到达的则行驶
void checkStateOfAllCar() {
	int i;
	for (i = 0; i <= Road::totRoad; ++i)
	{
		//遍历道路的正向车道 
		checkStateOfOneRoad(&road[i], road[i].Cars);

		if (road[i].isDuplex == false) continue;

		//遍历道路的逆向车道 
		checkStateOfOneRoad(&road[i], road[i].rCars);
	}
}

//如果路口中的所有车辆都是终止态，则返回 true 
bool allCarInCrossAreTerminated(Cross *tempCross) {
	//正方向 
	int i,j,k;
	for (i = 0; i < 4; ++i)
		if (tempCross->r[i] != NULL && (tempCross->r[i])->end == tempCross)
		{
			Road *tempRoad = tempCross->r[i];
			for (j = 0; j < (tempRoad->Cars).size(); ++j)
			{
				for (k = 0; k < (tempRoad->Cars)[j].size(); ++k)
					if ((tempRoad->Cars)[j][k]->isWaiting == true) return false;
			}
		}
	//反方向 
	for (i = 0; i < 4; ++i)
		if (tempCross->r[i] != NULL && (tempCross->r[i])->isDuplex == true && (tempCross->r[i])->start == tempCross)
		{
			Road *tempRoad = tempCross->r[i];
			for (j = 0; j < (tempRoad->rCars).size(); ++j)
			{
				for (k = 0; k < (tempRoad->rCars)[j].size(); ++k)
					if ((tempRoad->rCars)[j][k]->isWaiting == true) return false;
			}
		}
	return true;
}
//如果tempRoad指向tempCross方向的道路的所有车辆都是终止状态，返回true 
bool AllCarOnRoadAreTerminated(Road *tempRoad, Cross *tempCross) {
	int i,j;
	if (tempRoad->end == tempCross)
	{
		for (i = 0; i < (tempRoad->Cars).size(); ++i)
			for (j = 0; j < (tempRoad->Cars)[i].size(); ++j)
				if ((tempRoad->Cars)[i][j]->isWaiting == true) return false;
	}
	else
	{
		for (int i = 0; i < (tempRoad->rCars).size(); ++i)
			for (int j = 0; j < (tempRoad->rCars)[i].size(); ++j)
				if ((tempRoad->rCars)[i][j]->isWaiting == true) return false;
	}

	return true;
}
//返回tempRoad指向tempCross方向的道路的第一优先车辆 ,以及这辆车所在车道数 
Car *getFirstCar_LaneNum(Road *tempRoad, Cross *tempCross, int &laneNum) {
	Car *firstCar = NULL;
	if (tempRoad->end == tempCross)
	{
		for (int i = 0; i < (tempRoad->Cars).size(); ++i)
			if ((tempRoad->Cars)[i].size() > 0 && (tempRoad->Cars)[i][0]->isWaiting == true)
			{
				if (firstCar == NULL)
				{
					firstCar = (tempRoad->Cars)[i][0]; laneNum = i;
				}
				else if (firstCar->dist > ((tempRoad->Cars)[i][0])->dist)
				{
					firstCar = (tempRoad->Cars)[i][0]; laneNum = i;
				}
			}
	}
	else if (tempRoad->isDuplex == true && tempRoad->start == tempCross)
	{
		for (int i = 0; i < (tempRoad->rCars).size(); ++i)
			if ((tempRoad->rCars)[i].size() > 0 && (tempRoad->rCars)[i][0]->isWaiting == true)
			{
				if (firstCar == NULL)
				{
					firstCar = (tempRoad->rCars)[i][0]; laneNum = i;
				}
				else if (firstCar->dist > ((tempRoad->rCars)[i][0])->dist)
				{
					firstCar = (tempRoad->rCars)[i][0]; laneNum = i;
				}
			}
	}
	return firstCar;
}
//tempCar当前在 道路tempRoad 上，返回下一条道路以及转向，直行0，左转1，右转2 
Road *getNextRoad_Dir(Road *tempRoad, Car *tempCar, int &dir) {
	Road *nextRoad = NULL;
	if(tempCar==NULL) 
	{
		nextRoad = NULL;dir=0;
		return nextRoad;
	}
	int i=tempCar->index;
//	for (i = 0; i < (tempCar->myRoad).size(); ++i)
//		if ((tempCar->myRoad)[i] == tempRoad) break;
	
	if(i==(tempCar->myRoad).size()-1)
	{
		nextRoad = NULL;dir=0;
		return nextRoad;
	}
	
	nextRoad = (tempCar->myRoad)[i + 1];
	if (tempRoad->Did == nextRoad) dir = 0;
	if (tempRoad->Lid == nextRoad) dir = 1;
	if (tempRoad->Rid == nextRoad) dir = 2;
	if (tempRoad->rDid == nextRoad) dir = 0;
	if (tempRoad->rLid == nextRoad) dir = 1;
	if (tempRoad->rRid == nextRoad) dir = 2;
	return nextRoad;
}


//如果tempRoad指向tempCross方向的道路的第一优先车辆firstCar与其他道路第一优先车辆冲突，返回true 
bool conflict(Car *firstCar,Road *nextRoad_firstCar,int dir_firstCar,Road *tempRoad, Cross *tempCross)
{
	for (int i = 0; i < 4; ++i)
		if (tempCross->r[i] != NULL && tempCross->r[i] != tempRoad)
		{
			Car *otherCar=NULL;int laneNum=0;Road *nextRoad_otherCar=NULL;int dir_otherCar=0;
			otherCar=getFirstCar_LaneNum(tempCross->r[i],tempCross,laneNum);
			nextRoad_otherCar=getNextRoad_Dir(tempCross->r[i],otherCar,dir_otherCar);
		//	get_FirstCar_LaneNum_NextRoad_dir(otherCar,laneNum,nextRoad_otherCar,dir_otherCar,tempCross->r[i],tempCross);
			
			if(otherCar==NULL) continue;

			//0直行，1左转，2右转 
			//1.如果两辆车都没有到终点 
			if(nextRoad_firstCar!=NULL&&nextRoad_otherCar!=NULL)
			{
				if (nextRoad_firstCar == nextRoad_otherCar && dir_firstCar > dir_otherCar) return true;
			}
			//2.如果辆车都将到达终点 
			else if(nextRoad_firstCar==NULL&&nextRoad_otherCar==NULL)
			{
				continue;  
			}
			//3.如果当前车到达终点 
			else if(nextRoad_firstCar==NULL&&nextRoad_otherCar!=NULL)
			{
				continue;  
			}
			//4.如果另外车到达终点 
			else if(nextRoad_firstCar!=NULL&&nextRoad_otherCar==NULL)
			{
				if(tempCross->r[i]->Did==nextRoad_firstCar) return true;
				if(tempCross->r[i]->rDid==nextRoad_firstCar) return true;
	
			}	
			
		}

	return false;
}

//起点为tempCross的tempRoad道路被终止状态的车堵塞了，则返回true
bool isCongestedByTerminatedCar(Cross *tempCross, Road *tempRoad) {
	if (tempRoad->start == tempCross)
	{
		for (int i = 0; i < (tempRoad->Cars).size(); ++i)
			if ((tempRoad->Cars)[i].size() > 0)
			{
				int carNum = tempRoad->Cars[i].size();
				if ((tempRoad->Cars)[i][carNum - 1]->isWaiting == true) return false;
				if ((tempRoad->Cars)[i][carNum - 1]->dist != tempRoad->length - 1) return false;
			}
			else return false;
	}
	else if (tempRoad->isDuplex == true && tempRoad->end == tempCross)
	{
		for (int i = 0; i < (tempRoad->rCars).size(); ++i)
			if ((tempRoad->rCars)[i].size() > 0)
			{
				int carNum = tempRoad->rCars[i].size();
				if ((tempRoad->rCars)[i][carNum - 1]->isWaiting == true) return false;
				if ((tempRoad->rCars)[i][carNum - 1]->dist != tempRoad->length - 1) return false;

			}
			else return false;
	}
	return true;
}

bool isBlockedByWaitingCar(int s2,Car *tempCar,Cross *tempCross, Road *tempRoad)
{
	if (tempRoad->start == tempCross)
	{
		for (int i = 0; i < (tempRoad->Cars).size(); ++i)
		
		{
			if(tempRoad->Cars[i].size()==0) return false;
			int ssize=tempRoad->Cars[i].size()-1;
			if(tempRoad->Cars[i][ssize]->isWaiting==false&&tempRoad->Cars[i][ssize]->dist<tempRoad->length-1) return false;
			if(tempRoad->Cars[i][ssize]->isWaiting==true&&tempRoad->Cars[i][ssize]->dist+s2<=tempRoad->length-1) return false;
			if(tempRoad->Cars[i][ssize]->isWaiting==true&&tempRoad->Cars[i][ssize]->dist+s2>tempRoad->length-1) return true;
		
		}
		return false;
	}
	else if (tempRoad->isDuplex == true && tempRoad->end == tempCross)
	{
		for (int i = 0; i < (tempRoad->rCars).size(); ++i)
		{
			if(tempRoad->rCars[i].size()==0) return false;
			int ssize=tempRoad->rCars[i].size()-1;
			if(tempRoad->rCars[i][ssize]->isWaiting==false&&tempRoad->rCars[i][ssize]->dist<tempRoad->length-1) return false;
			if(tempRoad->rCars[i][ssize]->isWaiting==true&&tempRoad->rCars[i][ssize]->dist+s2<=tempRoad->length-1) return false;
			if(tempRoad->rCars[i][ssize]->isWaiting==true&&tempRoad->rCars[i][ssize]->dist+s2>tempRoad->length-1) return true;
		
		}
		return false;
	}
	return true;
	


}


//移动currentRoad上第laneNum车道上的车辆tempCar 
bool runToNextRoad(Car *tempCar, int laneNum, Road *currentRoad, vector<vector<Car *> > &preCars, Road *nextRoad, vector<vector<Car *> > &nextCars){
	int i = 0;
	int carNumOnLane;
	for (i = 0; i < nextCars.size(); ++i)
	{
		carNumOnLane = nextCars[i].size();
		//如果当前车道上1.没有车，2.最后一辆车为waiting状态，3.最后一辆车为terminated状态且不在道路最后位置
		if (carNumOnLane == 0
			|| nextCars[i][carNumOnLane - 1]->isWaiting == true
			|| (nextCars[i][carNumOnLane - 1]->isWaiting == false && (nextCars[i][carNumOnLane - 1]->dist < nextRoad->length - 1)))
			break;
	}
	if(i>=nextCars.size()){cout<<"Wrong"<<endl;}

	if (carNumOnLane == 0)
	{
		int s2 = getS2(tempCar, currentRoad);
		tempCar->dist = max(nextRoad->length - s2, 0);
		nextCars[i].push_back(tempCar);
		tempCar->isWaiting = false;
		tempCar->index++;
		preCars[laneNum].erase(preCars[laneNum].begin());
		return true;
	}
	else if (nextCars[i][carNumOnLane - 1]->isWaiting == true)
	{
		int s2 = getS2(tempCar, currentRoad);
		if (nextRoad->length - nextCars[i][carNumOnLane - 1]->dist - 1<s2)  return false;
		tempCar->dist = nextRoad->length - s2;
		nextCars[i].push_back(tempCar);
		tempCar->isWaiting = false;
		tempCar->index++;
		preCars[laneNum].erase(preCars[laneNum].begin());
		return true;
	}
	else if (nextCars[i][carNumOnLane - 1]->isWaiting == false && (nextCars[i][carNumOnLane - 1]->dist < nextRoad->length - 1))
	{
		int s2 = getS2(tempCar, currentRoad);
		s2 = min(s2, nextRoad->length - 1 - nextCars[i][carNumOnLane - 1]->dist);
		tempCar->dist = nextRoad->length - s2;
		nextCars[i].push_back(tempCar);
		tempCar->isWaiting = false;
		tempCar->index++;
		preCars[laneNum].erase(preCars[laneNum].begin());
		return true;
	}
}



//尝试把在车道数laneNum的这辆车开往下一路段
bool runCarToNextRoad(Car *tempCar, Road *currentRoad, int laneNum, Road *nextRoad)
{
	//当前道路为正方向，下一条道路为正方向
	if (currentRoad->end == nextRoad->start)
	{
		return runToNextRoad(tempCar, laneNum, currentRoad, currentRoad->Cars, nextRoad, nextRoad->Cars);
	}
	//当前道路为正方向，下一条道路为反方向
	else if (currentRoad->end == nextRoad->end&&nextRoad->isDuplex == true)
	{
		return runToNextRoad(tempCar, laneNum, currentRoad, currentRoad->Cars, nextRoad, nextRoad->rCars);
	}
	//当前道路为反方向，下一条道路为正方向
	else if (currentRoad->isDuplex == true && currentRoad->start == nextRoad->start)
	{
		return runToNextRoad(tempCar, laneNum, currentRoad, currentRoad->rCars, nextRoad, nextRoad->Cars);
	}
	//当前道路为反方向，下一条道路为反方向
	else if (currentRoad->isDuplex == true && nextRoad->isDuplex == true && currentRoad->start == nextRoad->end)
	{
		return runToNextRoad(tempCar, laneNum, currentRoad, currentRoad->rCars, nextRoad, nextRoad->rCars);
	}
	else
	{
		cout << "出错了！" << endl; return false;
	}
}

bool allCarInNetworkAreTerminated(){
	for (int i = 0; i <= Cross::totCross; ++i)
		if (!allCarInCrossAreTerminated(&cross[i])) return false;
	return true;
}



bool allCarAreFinished(){
	for (int i = 0; i <= Car::totCar; ++i)
		if (car[i].isFinished == false) return false;
	return true;
}


//尝试移动tempCross上的tempCar车辆进入nextRoad道路的nextCars方向
bool canRunCarFromCross(Car *tempCar, Cross *tempCross, Road *nextRoad, vector<vector<Car *> > &nextCars){
	int i = 0;
	int carNumOnLane;
	for (i = 0; i < nextCars.size(); ++i)
	{
		carNumOnLane = nextCars[i].size();
		//如果当前车道上1.没有车，2.最后一辆车为waiting状态，3.最后一辆车为terminated状态且不在道路最后位置
		if (carNumOnLane == 0
			|| nextCars[i][carNumOnLane - 1]->isWaiting == true
			|| (nextCars[i][carNumOnLane - 1]->isWaiting == false && (nextCars[i][carNumOnLane - 1]->dist < nextRoad->length - 1)))
			break;
	}

	if (i >= nextCars.size()) return false;//说明下一道路的位置被占满了

	if (carNumOnLane == 0)
	{
		int s2 = min(tempCar->maxv, nextRoad->maxv);
		tempCar->dist = nextRoad->length - s2;
		nextCars[i].push_back(tempCar);
		tempCar->isWaiting = false;
		tempCar->index++;
		return true;
	}
	else if (nextCars[i][carNumOnLane - 1]->isWaiting == true)
	{
		cout << "出现错误，车辆从路口出发不应该有车是waiting状态！" << endl;
		return false;
	}
	else if (nextCars[i][carNumOnLane - 1]->isWaiting == false && (nextCars[i][carNumOnLane - 1]->dist < nextRoad->length - 1))
	{
		int s2 = min(tempCar->maxv, nextRoad->maxv);
		s2 = min(s2, nextRoad->length - 1 - nextCars[i][carNumOnLane - 1]->dist);
		tempCar->dist = nextRoad->length - s2;
		nextCars[i].push_back(tempCar);
		tempCar->isWaiting = false;
		tempCar->index++;
		return true;
	}
}

void runCarsFromCrosses(int system_Time){
	int i,j,k;
	for (i = 0; i <= Cross::totCross; ++i)
		for ( j = 1; j <= cross[i].carNum; ++j)
		{
		//	cout<<cross[i].cars[j]->id<<"   ";
			if ((cross[i].cars[j])->real_time == system_Time)
				for (k = 0; k < 4; ++k)
					if (cross[i].r[k] == (cross[i].cars[j])->myRoad[0])
					{
						if (cross[i].r[k]->start == &cross[i])
						{
							if (!canRunCarFromCross(cross[i].cars[j], &cross[i], cross[i].r[k], cross[i].r[k]->Cars))
							{
								cross[i].cars[j]->real_time++;
							
							}
						}
						else if (cross[i].r[k]->isDuplex == true && cross[i].r[k]->end == &cross[i])
						{
							if (!canRunCarFromCross(cross[i].cars[j], &cross[i], cross[i].r[k], cross[i].r[k]->rCars))
							{
								cross[i].cars[j]->real_time++;
							}
						}
					}
		
		}
			
}

bool ISempty(vector<vector<Car *> > Cars){
	for (int i = 0; i < Cars.size(); ++i)
		if (Cars[i].size() != 0) return false;
	return true;
}

void printAllCar(){
	cout << system_Time << "              时刻" << endl;
	for (int i = 0; i <= Road::totRoad; ++i)
	{
		if (!ISempty(road[i].Cars))cout << road[i].start->id << " ---->  " << road[i].end->id << endl;
		for (int j = 0; j < road[i].Cars.size(); ++j)
		{
			for (int k = 0; k < road[i].Cars[j].size(); ++k)
				cout << "                         " << road[i].Cars[j][k]->id << "    车道：" << j << "  距离" << road[i].Cars[j][k]->dist << endl;

		}

		if (road[i].isDuplex == false) continue;
		if (!ISempty(road[i].rCars))cout << road[i].end->id << " ---->  " << road[i].start->id << endl;
		for (int j = 0; j < road[i].rCars.size(); ++j)
		{
			for (int k = 0; k < road[i].rCars[j].size(); ++k)
				cout << "                         " << road[i].rCars[j][k]->id << "    车道：" << j << "  距离" << road[i].rCars[j][k]->dist << endl;

		}

	}


}
void runThisLane(Road *tempRoad,vector<Car *> &tempLane){
	if(tempLane.size()==0) return ;
	if(tempLane[0]->isWaiting==true&&tempLane[0]->dist>=min(tempLane[0]->maxv, tempRoad->maxv))
	{
		tempLane[0]->dist=tempLane[0]->dist - min(tempLane[0]->maxv, tempRoad->maxv);
		tempLane[0]->isWaiting=false;
		for(int k=1;k<tempLane.size();++k)
		if(tempLane[k]->isWaiting==true)
		runCar(tempRoad,tempLane,k);
		else break;
	}

}

bool runnable()
{
	int i,j,k;
	int cars_Time = 0;
	bool condition_System_Changed = false;
	bool condition_Cross_Changed = false;
	for (system_Time = 0;; ++system_Time)
	{
		if(system_Time>answer_system_Time) return false;
		if (allCarAreFinished())
		{
			cout << "系统调度时间为:" << system_Time << endl;
			for (i = 0; i <= Car::totCar; ++i)
			{
				cars_Time += car[i].end_time - car[i].time;
			}
			cout << "总的车辆调度时间为：" << cars_Time << endl;

			if (system_Time < answer_system_Time || (system_Time == answer_system_Time&&cars_Time < answer_cars_Time))
			{
				answer_system_Time = system_Time; answer_cars_Time = cars_Time;
				for (i = 0; i <= Car::totCar; ++i)
				{
					car[i].answerRoad = car[i].myRoad;
					car[i].answer_real_time = car[i].real_time;
				}
			}

			return true;
		}
	
		for (i = 0; i <= Car::totCar; ++i)
			car[i].isWaiting=true; 
		//		cout << "开始确认状态" << endl;
		checkStateOfAllCar();
		
		while (!allCarInNetworkAreTerminated()) {

			condition_System_Changed = false;

			for (i = 0; i <= Cross::totCross; ++i)
			{
				//	cout << "开始调度" << cross[i].id << "路口" << endl;
				//把该路口的道路按id升序 
				Road *r[4] = { cross[i].r[0], cross[i].r[1], cross[i].r[2], cross[i].r[3] }, *tempRoad;
				for (j = 0; j < 4; ++j)
				{
					for (k = j + 1; k < 4; ++k)
						if (r[k] == NULL || (r[j] != NULL && r[k]->id < r[j]->id))
						{
							tempRoad = r[j]; r[j] = r[k]; r[k] = tempRoad;
						}
				}
				while (!allCarInCrossAreTerminated(&cross[i]))
				{
					condition_Cross_Changed = false; //记录该路口车辆状态是否有改变 

					for (j = 0; j < 4; ++j)
						//判断道路是否指向该路口 
						if (r[j] != NULL && (r[j]->end == &cross[i] || (r[j]->isDuplex == true && r[j]->start == &cross[i])))
						{

							while (!AllCarOnRoadAreTerminated(r[j], &cross[i]))
							{
								Car *firstCar=NULL;int laneNum=0;Road *nextRoad=NULL;int dir=0;
								
								//	cout<<"开始找最优车辆"<<endl;
								firstCar=getFirstCar_LaneNum(r[j],&cross[i],laneNum);
								nextRoad=getNextRoad_Dir(r[j],firstCar,dir);
								
								if (!conflict(firstCar,nextRoad,dir,r[j],&cross[i]))
								{
									
									//1.如果该优先车辆的终点在路口 
									if(nextRoad==NULL)
									{
										//因为需要判断第一优先车辆后面的车是否到达终点，所以用runThisLane函数 
										condition_Cross_Changed = true;
										if(firstCar->dist<min(firstCar->maxv,r[j]->maxv))
										{
											firstCar->dist=-1;
											firstCar->isWaiting=false;
											firstCar->isFinished=true;
											firstCar->end_time=system_Time;//+1;
											firstCar->index=-1;
											
											if(r[j]->end==&cross[i])
												r[j]->Cars[laneNum].erase(r[j]->Cars[laneNum].begin());
											else
												r[j]->rCars[laneNum].erase(r[j]->rCars[laneNum].begin()); 
										}
										else {cout<<"不可能出现这种情况"<<endl;system("pause");} 
										
										if(r[j]->end==&cross[i]) runThisLane(r[j],r[j]->Cars[laneNum]);
										else if(r[j]->start==&cross[i]) runThisLane(r[j],r[j]->rCars[laneNum]);
										
										continue;  //返回 
									}
									
									
									
									
									
									int s2=getS2(firstCar, r[j]);
									
							//		if(isBlockedByWaitingCar(s2,firstCar,&cross[i], nextRoad)) break;
									//2.如果第一优先的车辆将要行驶的道路被终止态车辆堵塞了\或者s2==0，把该车道中的所有车辆都确定为终止态 
									if (s2<=0||isCongestedByTerminatedCar(&cross[i], nextRoad))
									{
										//不需要判断第一优先车辆后面的车是否到达终点
										condition_Cross_Changed = true;

										if (r[j]->end == &cross[i])
										{
											r[j]->Cars[laneNum][0]->dist= r[j]->Cars[laneNum][0]->dist-min(r[j]->Cars[laneNum][0]->maxv,r[j]->maxv);
											r[j]->Cars[laneNum][0]->dist=max(r[j]->Cars[laneNum][0]->dist,0);
											r[j]->Cars[laneNum][0]->isWaiting=false;
											
											for(int k=1;k<r[j]->Cars[laneNum].size();++k)
											if(r[j]->Cars[laneNum][k]->isWaiting!=false)
											runCar(r[j],r[j]->Cars[laneNum],k);
											else break;
										}
										else
										{
											r[j]->rCars[laneNum][0]->dist= r[j]->rCars[laneNum][0]->dist-min(r[j]->rCars[laneNum][0]->maxv,r[j]->maxv);
											r[j]->rCars[laneNum][0]->dist=max(r[j]->rCars[laneNum][0]->dist,0);
											r[j]->rCars[laneNum][0]->isWaiting=false;
											
											for(int k=1;k<r[j]->rCars[laneNum].size();++k)
											if(r[j]->rCars[laneNum][k]->isWaiting!=false)
											runCar(r[j],r[j]->rCars[laneNum],k);
											else break;
										}
										continue;
									}
									
									//3.如果优先车辆可以开往下一道路
									if (runCarToNextRoad(firstCar, r[j], laneNum, nextRoad))
									{
										//因为需要判断第一优先车辆后面的车是否到达终点，所以用runThisLane函数 
										condition_Cross_Changed = true;
										if (r[j]->end == &cross[i])  runThisLane(r[j],r[j]->Cars[laneNum]);
										else runThisLane(r[j],r[j]->rCars[laneNum]);

									}
									//4.优先车辆不能开往下一道路，即还是waiting状态
									else break;

								}
								//有冲突的话就换下一条道路
								else break;
							}
						}
					           //    cout<<"结束遍历每条路"<<endl;
					if (condition_Cross_Changed == false) break;
				}
				condition_System_Changed = condition_System_Changed || condition_Cross_Changed;

			}

			if (condition_System_Changed == false) return false;

		}

		//	cout << "开始开路口中的车" << endl;
		runCarsFromCrosses(system_Time);
		//	system("pause");
      /*  
			cout<<system_Time<<"时刻从路口开车结束"<<endl;
			printAllCar();
			cout << endl;
			system("pause");
*/
	}


}



//int main(int argc, _TCHAR* argv[])
int main(int argc, char *argv[])
{
	cout << "Begin" << std::endl;
	
	if(argc < 5){
		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
		exit(1);
	}
	string carPath(argv[1]);
	string roadPath(argv[2]);
	string crossPath(argv[3]);
	string answerPath(argv[4]);
	
	clock_t startTime, endTime;
	startTime = clock();

	//读取road.txt，cross.txt,car.txt的数据 
	readData(carPath,roadPath,crossPath);
	
	cout << "车的数量：" << Car::totCar << endl;
	cout << "道路的数量：" << Road::totRoad << endl;
	cout << "路口的数量：" << Cross::totCross << endl;
	//把每条道路的Did,Lid,Rid,rDid,rLid,rRid分别指向对应的道路 
	makeMap();
	//寻找每辆车的k最短路径 
	k_shortest();
	endTime = clock();
	cout << "路径规划已完成,耗时"<< (endTime - startTime) / CLOCKS_PER_SEC << "秒" << endl;
	
	//	system("pause");

	int l=1,r=10000;
	int range,i,j,k;
	int dd;
	srand(time(0));
	while(true)
	{
		range = (l+r)/2;
	//	range=1;
		cout<<"range="<<range<<endl;
		k=0;
		while(k<20)
		{
			
			cout<<"    启动第"<<k<<"次查找"<< endl;
			endTime = clock();
			//如果运行了250s,就结束运行
			if ((endTime - startTime) / CLOCKS_PER_SEC > 60*14)  break;
			//if(range<=answer_system_Time) range++;

			for (i = 0; i <= Car::totCar; ++i)
			{
				car[i].isFinished = false;
				car[i].isWaiting = true;
				car[i].index=-1; 
				
				int totRoute = car[i].k_myRoad.size();
				
				while (true)
				{
					dd=rand() % totRoute;
					car[i].myRoad = car[i].k_myRoad[dd];
					car[i].real_time = car[i].time+rand()%range;
					if (car[i].real_time+car[i].k_time[dd] < answer_system_Time) break;
				}
		
			
				
			//	car[i].myRoad = car[i].k_myRoad[0];
			}
			for (i = 0; i <= Road::totRoad; ++i)
			{
				for (j = 0; j<road[i].Cars.size(); ++j)
					road[i].Cars[j].clear();
				if (road[i].isDuplex == true)
				for (j = 0; j<road[i].rCars.size(); ++j)
					road[i].rCars[j].clear();
			}
			
			
			
			
			if (runnable())
			{
				cout << "找到一条合法道路" << endl;//system("pause");
				r=range;
				break;
			}
			else
			{
				cout << "    警告---------路径规划方案已堵死" <<endl;//system("pause");
			}
			
			k++;
			
		}
		if ((endTime - startTime) / CLOCKS_PER_SEC > 60*14)  break;
		if(k>=20)
		l=range;

	}
	
	ofstream out(answerPath.c_str());
	for (i = 0; i <= Car::totCar; ++i)
	{
		out << "(" << car[i].id << "," << car[i].answer_real_time;
		for (j = 0; j < car[i].answerRoad.size(); ++j)
			out << "," << (car[i].answerRoad[j])->id;
		out << ")" << endl;
	}
	out.close();
	cout << "系统调度时间：" << answer_system_Time-1 << endl;
	cout << "车辆运行时间" << answer_cars_Time << endl;
	cout << "运行时间" << (endTime - startTime) / CLOCKS_PER_SEC << "秒" << endl;

	return 0;
}


