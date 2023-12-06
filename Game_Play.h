#pragma once
#include <vector>
#include "easyx.h"
#pragma warning(disable:4996)

void Play(int level);

class Lead
{
public:
	double x;           //����
	double y;           //����
	double v_y;         //y�����ٶ�
	double v_x;         //x�����ƶ��ٶ�
	double v0;          //��Ծ���ٶ�
	double g;           //�������ٶ�
	int p;              //p����
	int b;              //�����С
	int f;              //����
	long start;         //��¼�����ӵ�����ʼʱ��
	bool fire;          //�ܷ����ӵ��ı�־
	double cd;          //�����ӵ���cd(����)
	bool w;             //�ܷ�����
	bool a;             //�ܷ������ƶ�
	bool s;             //�ܷ�����
	bool d;             //�ܷ������ƶ�
	bool deli;          //�ܷ���д���
	bool death;         //�Ƿ�����
	bool tum;           //�Ƿ����ש���ڲ�
	Lead();
	bool Move();		 //�����ƶ�
	bool Jump();	     //��Ծ
	bool FireBullets();  //�����ӵ�
	bool FireCD();       //����fire����
	bool Judge();        //�ж�wasd�ĸ�����
};

class Delivery
{
public:
	double x;
	double y;              //λ��
	int index;
	Delivery() :index(1) {};
};

class Brick
{
public:
	char s;             //�����־
	double x;           //������
	double y;           //������
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

//��
class Ball
{
public:
	char s;             //�����־
	double x;           //����
	double y;           //����
	int hp;             //Ӳ��
	bool del;           //ɾ��
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
	int t;                  //��ʼ�ƶ����� tΪ��ʱ����
	double v_move;          //�ƶ��ٶ�
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

//�ӵ�
class Bullet
{
public:
	double x;
	double y;
	int b;
	double v;
	int f;                  //����
	char left_upper;		//���Ͻ�����
	char right_upper;		//���Ͻ�����
	char left_lower;		//���½����� 
	char right_lower;		//���½�����
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