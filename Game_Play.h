#pragma once
#include <vector>
#include "easyx.h"
#pragma warning(disable:4996)

void Play(int level);

class Lead
{
public:
	double x;           //坐标
	double y;           //坐标
	double v_y;         //y方向速度
	double v_x;         //x方向移动速度
	double v0;          //跳跃初速度
	double g;           //重力加速度
	int p;              //p段跳
	int b;              //人物大小
	int f;              //朝向
	long start;         //记录发射子弹的起始时间
	bool fire;          //能否发射子弹的标志
	double cd;          //发射子弹的cd(毫秒)
	bool w;             //能否上升
	bool a;             //能否向左移动
	bool s;             //能否下落
	bool d;             //能否向右移动
	bool deli;          //能否进行传送
	bool death;         //是否死亡
	bool tum;           //是否跌入砖块内部
	Lead();
	bool Move();		 //左右移动
	bool Jump();	     //跳跃
	bool FireBullets();  //发射子弹
	bool FireCD();       //调整fire变量
	bool Judge();        //判断wasd四个变量
};

class Delivery
{
public:
	double x;
	double y;              //位置
	int index;
	Delivery() :index(1) {};
};

class Brick
{
public:
	char s;             //特殊标志
	double x;           //横坐标
	double y;           //纵坐标
	bool Bullet_reaction() {};
};

class CommonBrick :public Brick
{
public:
	CommonBrick* next;
	CommonBrick() :next(nullptr) {};
};

class SpecialBrick :public Brick
{
public:
	bool invert;
	bool del;
	int index;
	SpecialBrick* next;
	SpecialBrick() :invert(false), del(false), index(200), next(nullptr) {};
};

class GlassBrick :public Brick
{
public:
	bool del;
	GlassBrick* next;
	GlassBrick() :del(false), next(nullptr) {};
};

//球
class Ball
{
public:
	char s;             //特殊标志
	double x;           //坐标
	double y;           //坐标
	int hp;             //硬度
	bool del;           //删除
	Ball() :del(false) {};
};

class BlastBall :public Ball
{
public:
	bool ex;
	BlastBall* next;
	BlastBall() :ex(false), next(nullptr) {};
	bool Explode(int, int);
};

class MoveBall :public Ball
{
public:
	int t;                  //初始移动方向 t为正时向右
	double v_move;          //移动速度
	bool fly;
	MoveBall* next;
	MoveBall() :fly(false), v_move(0.5), t(1), next(nullptr) {};
	bool move();
	bool showy(SpecialBrick*, MoveBall*, char);
};

class WaterBall :public Ball
{
public:
	int index;
	WaterBall* next;
	WaterBall() :index(1), next(nullptr) {};
};

//子弹
class Bullet
{
public:
	double x;
	double y;
	int b;
	double v;
	int f;                  //朝向
	char left_upper;		//左上角属性
	char right_upper;		//右上角属性
	char left_lower;		//左下角属性 
	char right_lower;		//右下角属性
	bool del;
	Bullet* next;
	Bullet() :b(30), v(2), del(false), next(nullptr) {};
};

class Tip
{
private:
	TCHAR text[256];
	int x;
	int y;
public:
	Tip(int x, int y,TCHAR *old_text) :x(x), y(y) {
		_tcsncpy(text, old_text,256);
	}
	void draw() {
		outtextxy(x, y,text);
	}
};

template <class T>
bool Delete(T head, T p);

template <class T>
bool Increase(T* head, double& x, double& y);

template <class T>
void print(T head);

template <class T>
bool FreeList(T head);

template <class T>
void InitList(T*& head, int& x, int& y, char s);

template <class T>
bool Traverse(T& head, T& tmp, double x, double y);

std::vector<Tip>  outtext(char* addr);