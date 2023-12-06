#include "Game_Play.h"
#include "Tools.h"
#include "setjmp.h"
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <direct.h>

#define isKeyPressed(nVirtKey) (GetKeyState(nVirtKey) & (1<<(sizeof(SHORT)*8-1))) != 0

using namespace std;

//ȫ�ֱ���
bool game;

int level; //�ؿ���
static int rW = GetSystemMetrics(SM_CXSCREEN); // ��Ļ��� ����
static int rH = GetSystemMetrics(SM_CYSCREEN); // ��Ļ�߶� ����
static int B = min(rW / 31, rH / 19);
static int sW = (rW - B * 31) / 2;
static int sH = (rH - B * 19) / 2;
static int Lead_B = B / 4 * 3;
static int Bullet_B = B / 4 * 3;
static int Ball_B = B / 4 * 3;
static int Delivery_B = B * 3 - B / 2;
static double X = 2.5; //
static double Y = 1.75; //
static double G = 0.02; // ����
static double V_X = X * sqrt(G * B) / sqrt(8 * Y);
static double V_Y = sqrt(2 * G * Y * B);
vector<Tip> tips;

constexpr int WaterBall_NUM = 11;

extern jmp_buf menu;
extern jmp_buf set_level;

jmp_buf play;

MyMusic play_bgm(L"resource//Music//play_bgm.mp3");
MyMusic jump(L"resource//Music//jump.mp3");
MyMusic fire(L"resource//Music//fire.mp3");
MyMusic explode(L"resource//Music//explode.mp3");
MyMusic death(L"resource//Music//death.mp3");

IMAGE ��ɫ[4];
IMAGE ����;
IMAGE ��ͨש[2];
IMAGE ʯש;
IMAGE ��ש;
IMAGE ����ש;
//IMAGE ����;
IMA ����(L"resource//Item//RedBall");
//IMAGE ����;
IMA ����(L"resource//Item//BlackBall");
IMAGE ����;
IMAGE ����;
IMAGE �ӵ�;
IMAGE �ӵ�����ͼ;
IMAGE ˮ��[WaterBall_NUM + 1];
IMAGE ˮ������ͼ[WaterBall_NUM + 1];
IMAGE ��������ʾ;
IMAGE ��������ʾ����ͼ;
IMAGE ������ʾ;
IMAGE ������ʾ����ͼ;
IMAGE ʯש����ש[5];
IMAGE ��ש��ʯש[5];
IMA ������(L"resource//Item//Delivery");

Lead lead;
Delivery delivery;
//ʮ������
CommonBrick* CommonBrick_head = new CommonBrick;
SpecialBrick* StoneBrick_head = new SpecialBrick;
SpecialBrick* IronBrick_head = new SpecialBrick;
GlassBrick* GlassBrick_head = new GlassBrick;
BlastBall* RedBall_head = new BlastBall;
BlastBall* BlackBall_head = new BlastBall;
MoveBall* PurpleBall_head = new MoveBall;
MoveBall* GreyBall_head = new MoveBall;
WaterBall* WaterBall_head = new WaterBall;
Bullet* Bullet_head = new Bullet;

Lead::Lead()
{
	x = y = 0;
	v_y = 0;
	v_x = V_X;
	v0 = V_Y;
	p = 2;
	b = 30;
	g = G;
	f = 1;
	start = 0;
	fire = true;
	cd = 200;
	w = true;
	a = true;
	s = true;
	d = true;
	deli = false;
	death = false;
	tum = false;
	//��ˮƽ�������ƶ�Ϊƽ�ƣ�����ֱ������ʹ����ʵ����ģ��
}

template <class T>
bool Delete(T head, T p)
{
	if (!head) return false;
	T tmp = head->next, pre = head;
	while (tmp)
	{
		if (tmp == p)
		{
			pre->next = tmp->next;
			delete tmp;
			return true;
		}
		pre = tmp;
		tmp = tmp->next;
	}
	return false;
}

template <class T>
bool Increase(T* head, double& x, double& y)
{
	T* tmp = new T;
	tmp->x = x;
	tmp->y = y;
	tmp->next = head->next;
	head->next = tmp;
	return true;
}

template <class T>
void print(T head)
{
	int i = 1;
	head = head->next;
	while (head)
	{
		cout << head->x << "   " << head->y << "   " << i << endl;
		head = head->next;
		i++;
	}
}

template <class T>
bool FreeList(T head)
{
	if (!head) return false;
	T p = head->next, q;
	head->next = NULL;
	while (p)
	{
		q = p;
		p = p->next;
		delete q;
	}
	return true;
}

template <class T>
void InitList(T*& head, int& x, int& y, char s)
{
	T* tmp = new T;
	tmp->x = B * x + sW;
	tmp->y = B * y + sH;
	tmp->s = s;
	tmp->next = head->next;
	head->next = tmp;
}

template <class T>
bool Traverse(T& head, T& tmp, double x, double y)
{
	if (!head) return false;
	tmp = head->next;
	while (tmp) {
		if (x >= tmp->x && x < tmp->x + B && y >= tmp->y && y < tmp->y + B) return true;
		tmp = tmp->next;
	}
	return false;
}

void Calculate()                            //�������ص�λ��С
{
	B = min(rW / 31, rH / 19);
	sW = (rW - B * 31) / 2;
	sH = (rH - B * 19) / 2;
	Lead_B = Bullet_B = Ball_B = B / 4 * 3;
	Delivery_B = B * 3 - B / 2;
}

void LoadResource()
{
	loadimage(&��ɫ[0], L"resource\\��ɫ(��).png", Lead_B, Lead_B);
	loadimage(&��ɫ[1], L"resource\\��ɫ(��).png", Lead_B, Lead_B);
	loadimage(&����, L"resource\\����.png", rW, rH);
	loadimage(&��ͨש[0], L"resource\\��ͨש0.png", B, B);
	loadimage(&��ͨש[1], L"resource\\��ͨש1.png", B, B);
	loadimage(&ʯש, L"resource\\ʯש.png", B, B);
	loadimage(&��ש, L"resource\\��ש.png", B, B);
	loadimage(&����ש, L"resource\\����ש.png", B, B);
	//loadimage(&����, L"resource\\����.png", B, B);
	//loadimage(&����, L"resource\\����.png", B, B);
	loadimage(&����, L"resource\\����.png", B, B);
	loadimage(&����, L"resource\\����.png", B, B);
	loadimage(&ˮ��[1], L"resource\\WaterBall\\1.png", B, B);
	loadimage(&ˮ��[2], L"resource\\WaterBall\\2.png", B, B);
	loadimage(&ˮ��[3], L"resource\\WaterBall\\3.png", B, B);
	loadimage(&ˮ��[4], L"resource\\WaterBall\\4.png", B, B);
	loadimage(&ˮ��[5], L"resource\\WaterBall\\5.png", B, B);
	loadimage(&ˮ��[6], L"resource\\WaterBall\\6.png", B, B);
	loadimage(&ˮ��[7], L"resource\\WaterBall\\7.png", B, B);
	loadimage(&ˮ��[8], L"resource\\WaterBall\\8.png", B, B);
	loadimage(&ˮ��[9], L"resource\\WaterBall\\9.png", B, B);
	loadimage(&ˮ��[10], L"resource\\WaterBall\\10.png", B, B);
	loadimage(&ˮ��[11], L"resource\\WaterBall\\11.png", B, B);
	loadimage(&ˮ������ͼ[1], L"resource\\WaterBall\\����1.png", B, B);
	loadimage(&ˮ������ͼ[2], L"resource\\WaterBall\\����2.png", B, B);
	loadimage(&ˮ������ͼ[3], L"resource\\WaterBall\\����3.png", B, B);
	loadimage(&ˮ������ͼ[4], L"resource\\WaterBall\\����4.png", B, B);
	loadimage(&ˮ������ͼ[5], L"resource\\WaterBall\\����5.png", B, B);
	loadimage(&ˮ������ͼ[6], L"resource\\WaterBall\\����6.png", B, B);
	loadimage(&ˮ������ͼ[7], L"resource\\WaterBall\\����7.png", B, B);
	loadimage(&ˮ������ͼ[8], L"resource\\WaterBall\\����8.png", B, B);
	loadimage(&ˮ������ͼ[9], L"resource\\WaterBall\\����9.png", B, B);
	loadimage(&ˮ������ͼ[10], L"resource\\WaterBall\\����10.png", B, B);
	loadimage(&ˮ������ͼ[11], L"resource\\WaterBall\\����11.png", B, B);
	loadimage(&�ӵ�, L"resource\\�ӵ�.png", Bullet_B, Bullet_B);
	loadimage(&�ӵ�����ͼ, L"resource\\�ӵ�����ͼ.png", Bullet_B, Bullet_B);
	loadimage(&��������ʾ, L"resource\\Delivery\\������ʾ.png", rW / 6, rH / 30);
	loadimage(&��������ʾ����ͼ, L"resource\\Delivery\\������ʾ����ͼ.png", rW / 6, rH / 30);
	loadimage(&������ʾ, L"resource\\������ʾ.png", rW / 6 * 4, rH / 2);
	loadimage(&������ʾ����ͼ, L"resource\\������ʾ����ͼ.png", rW / 6 * 4, rH / 2);
	loadimage(&ʯש����ש[1], L"resource\\ʯ����ʼ.png", B, B);
	loadimage(&ʯש����ש[2], L"resource\\ʯ������.png", B, B);
	loadimage(&ʯש����ש[3], L"resource\\ʯ����ĩ.png", B, B);
	loadimage(&��ש��ʯש[1], L"resource\\����ʯʼ.png", B, B);
	loadimage(&��ש��ʯש[2], L"resource\\����ʯ��.png", B, B);
	loadimage(&��ש��ʯש[3], L"resource\\����ʯĩ.png", B, B);
	����.load(B, B);
	����.load(B, B);
	������.load(125, 125);
}

bool CreateList()		//��������
{
	char filename[100];
	sprintf_s(filename, 100,"resource\\Level\\%d.txt", level);
	char s;
	FILE* fp;
	fopen_s(&fp, filename, "r");
	if (fp == NULL)
	{
		cout << "�ؿ��ļ���ʧ��" << endl;
		exit(0);
	}

	CommonBrick* CommonBrick_tmp = CommonBrick_head;
	SpecialBrick* StoneBrick_tmp = StoneBrick_head;
	SpecialBrick* IronBrick_tmp = IronBrick_head;
	GlassBrick* GlassBrick_tmp = GlassBrick_head;
	BlastBall* RedBall_tmp = RedBall_head;
	BlastBall* BlackBall_tmp = BlackBall_head;
	MoveBall* PurpleBall_tmp = PurpleBall_head;
	MoveBall* GreyBall_tmp = GreyBall_head;
	WaterBall* WaterBall_tmp = WaterBall_head;
	Bullet* Bullet_tmp = Bullet_head;

	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 31; j++)
		{
			s = fgetc(fp);
			if (s == 'q' || s == 'w') InitList(CommonBrick_tmp, j, i, s);
			else if (s == 'e') InitList(StoneBrick_tmp, j, i, s);
			else if (s == 'r') InitList(IronBrick_tmp, j, i, s);
			else if (s == 't') InitList(GlassBrick_tmp, j, i, s);
			else if (s == 'y') InitList(RedBall_tmp, j, i, s);
			else if (s == 'u') InitList(BlackBall_tmp, j, i, s);
			else if (s == 'i') InitList(PurpleBall_tmp, j, i, s);
			else if (s == 'o') InitList(PurpleBall_tmp, j, i, s);
			else if (s == 'p') InitList(GreyBall_tmp, j, i, s);
			else if (s == 'a') InitList(GreyBall_tmp, j, i, s);
			else if (s == 's') InitList(WaterBall_tmp, j, i, s);
			else if (s == '*')
			{
				delivery.x = B * j + sW;
				delivery.y = B * i + sH;
			}
			else if (s == 'g')
			{
				Lead start;
				lead = start;
				lead.x = B * j + sW;
				lead.y = B * i + sH;
			}
		}
		fseek(fp, 2L, 1);
	}
	fclose(fp);
	return true;
}

bool Draw()
{
	putimage(0, 0, &����);

	CommonBrick* CommonBrick_tmp = CommonBrick_head->next;
	SpecialBrick* StoneBrick_tmp = StoneBrick_head->next;
	SpecialBrick* IronBrick_tmp = IronBrick_head->next;
	GlassBrick* GlassBrick_tmp = GlassBrick_head->next;
	BlastBall* RedBall_tmp = RedBall_head->next;
	BlastBall* BlackBall_tmp = BlackBall_head->next;
	MoveBall* PurpleBall_tmp = PurpleBall_head->next;
	MoveBall* GreyBall_tmp = GreyBall_head->next;
	WaterBall* WaterBall_tmp = WaterBall_head->next;
	Bullet* Bullet_tmp = Bullet_head->next;

	while (CommonBrick_tmp)
	{
		if (CommonBrick_tmp->s == 'q') putimage(CommonBrick_tmp->x, CommonBrick_tmp->y, &��ͨש[0]);
		if (CommonBrick_tmp->s == 'w') putimage(CommonBrick_tmp->x, CommonBrick_tmp->y, &��ͨש[1]);
		CommonBrick_tmp = CommonBrick_tmp->next;
	}
	while (StoneBrick_tmp)
	{
		if (StoneBrick_tmp->invert) putimage(StoneBrick_tmp->x, StoneBrick_tmp->y, &ʯש����ש[StoneBrick_tmp->index / 200]);
		else putimage(StoneBrick_tmp->x, StoneBrick_tmp->y, &ʯש);
		StoneBrick_tmp = StoneBrick_tmp->next;
	}
	while (IronBrick_tmp)
	{
		if (IronBrick_tmp->invert) putimage(IronBrick_tmp->x, IronBrick_tmp->y, &��ש��ʯש[IronBrick_tmp->index / 200]);
		else putimage(IronBrick_tmp->x, IronBrick_tmp->y, &��ש);
		IronBrick_tmp = IronBrick_tmp->next;
	}
	while (GlassBrick_tmp)
	{
		putimage(GlassBrick_tmp->x, GlassBrick_tmp->y, &����ש);
		GlassBrick_tmp = GlassBrick_tmp->next;
	}
	while (RedBall_tmp)
	{
		����.put(RedBall_tmp->x, RedBall_tmp->y);
		//putimage(RedBall_tmp->x, RedBall_tmp->y, &����);
		RedBall_tmp = RedBall_tmp->next;
	}
	while (BlackBall_tmp)
	{
		����.put(BlackBall_tmp->x, BlackBall_tmp->y);
		//putimage(BlackBall_tmp->x, BlackBall_tmp->y, &����);
		BlackBall_tmp = BlackBall_tmp->next;
	}
	while (PurpleBall_tmp)
	{
		putimage(PurpleBall_tmp->x, PurpleBall_tmp->y, &����);
		PurpleBall_tmp = PurpleBall_tmp->next;
	}
	while (GreyBall_tmp)
	{
		putimage(GreyBall_tmp->x, GreyBall_tmp->y, &����);
		GreyBall_tmp = GreyBall_tmp->next;
	}
	while (WaterBall_tmp)
	{
		putimage(WaterBall_tmp->x, WaterBall_tmp->y, &ˮ������ͼ[((WaterBall_tmp->index / 40) % WaterBall_NUM) + 1], SRCAND);
		putimage(WaterBall_tmp->x, WaterBall_tmp->y, &ˮ��[((WaterBall_tmp->index / 40) % WaterBall_NUM) + 1], SRCPAINT);
		WaterBall_tmp = WaterBall_tmp->next;
	}
	while (Bullet_tmp)
	{
		putimage(Bullet_tmp->x, Bullet_tmp->y, &�ӵ�����ͼ, SRCAND);
		putimage(Bullet_tmp->x, Bullet_tmp->y, &�ӵ�, SRCPAINT);
		Bullet_tmp = Bullet_tmp->next;
	}

	//������.put(delivery.x, delivery.y);
	������.rotate(0.4, delivery.x, delivery.y);

	if (lead.f == -1 && !lead.death)
		putimage(lead.x, lead.y, &��ɫ[0]);
	if (lead.f == 1 && !lead.death)
		putimage(lead.x, lead.y, &��ɫ[1]);

	if (lead.deli)
	{
		putimage(delivery.x - 55, delivery.y - 25, &��������ʾ����ͼ, SRCAND);
		putimage(delivery.x - 55, delivery.y - 25, &��������ʾ, SRCPAINT);
	}
	
	LOGFONT f_tip;                                 //�������
	gettextstyle(&f_tip);						   // ��ȡ��ǰ��������

	f_tip.lfHeight = rH / 20;			           // ��������߶�
	_tcscpy_s(f_tip.lfFaceName, _T("΢���ź�"));   // �������廪�Ĳ���
	f_tip.lfQuality = ANTIALIASED_QUALITY;		   // �������Ч��Ϊ�����  
	settextstyle(&f_tip);						   // ����������ʽ
	settextcolor(RGB(75,183,197));                 // ����������ɫ RGB(128, 42, 42)
	setbkmode(TRANSPARENT);                        // �������屳��Ϊ͸��
	for (auto& i : tips) {
		i.draw();
	}
	return true;
}

char GetPosition(double x, double y)
{
	CommonBrick* CommonBrick_tmp = NULL;
	SpecialBrick* StoneBrick_tmp = NULL;
	SpecialBrick* IronBrick_tmp = NULL;
	GlassBrick* GlassBrick_tmp = NULL;
	BlastBall* RedBall_tmp = NULL;
	BlastBall* BlackBall_tmp = NULL;
	MoveBall* PurpleBall_tmp = NULL;
	MoveBall* GreyBall_tmp = NULL;
	WaterBall* WaterBall_tmp = NULL;

	if (Traverse(CommonBrick_head, CommonBrick_tmp, x, y)) return 'q';
	if (Traverse(StoneBrick_head, StoneBrick_tmp, x, y)) return 'e';
	if (Traverse(IronBrick_head, IronBrick_tmp, x, y)) return 'r';
	if (Traverse(GlassBrick_head, GlassBrick_tmp, x, y)) return 't';
	if (Traverse(RedBall_head, RedBall_tmp, x, y)) return 'y';
	if (Traverse(BlackBall_head, BlackBall_tmp, x, y)) return 'u';
	if (Traverse(PurpleBall_head, PurpleBall_tmp, x, y)) return 'i';
	if (Traverse(GreyBall_head, GreyBall_tmp, x, y)) return 'p';
	if (Traverse(WaterBall_head, WaterBall_tmp, x, y)) return 's';

	if (x >= delivery.x && x < delivery.x + Delivery_B && y >= delivery.y && y < delivery.y + Delivery_B) return '*';

	if (x <= 0 || x >= rW || y <= 0 || y >= rH) return 'X';

	return '0';
}

bool Lead::Judge()
{
	char awa, aww, dwd, dww, asa, ass, dsd, dss;//�˸����ص�λ�����ƶ�����Ծ����
	char aw, as, dw, ds;//�ĸ����ص�λ�����ж�����
	char asd, dsa;//�������ص�λ�����жϵ���

	awa = GetPosition(x - 1, y);
	aww = GetPosition(x, y - 1);
	dwd = GetPosition(x + Lead_B + 1, y);
	dww = GetPosition(x + Lead_B, y - 1);
	asa = GetPosition(x - 1, y + Lead_B);
	ass = GetPosition(x, y + Lead_B + 1);
	dsd = GetPosition(x + Lead_B + 1, y + Lead_B);
	dss = GetPosition(x + Lead_B, y + Lead_B + 1);

	aw = GetPosition(x, y);
	as = GetPosition(x + Lead_B, y);
	dw = GetPosition(x, y + Lead_B);
	ds = GetPosition(x + Lead_B, y + Lead_B);

	asd = GetPosition(x + 1, y + Lead_B);
	dsa = GetPosition(x + Lead_B - 1, y + Lead_B);

	if (dwd == 'q' || dwd == 'e' || dwd == 'r' || dwd == 't' || dwd == 'X' || dsd == 'q' || dsd == 'e' || dsd == 'r' || dsd == 't' || dsd == 'X' || lead.x + Lead_B > rW - sW)
		d = false;
	else d = true;

	if (awa == 'q' || awa == 'e' || awa == 'r' || awa == 't' || awa == 'X' || asa == 'q' || asa == 'e' || asa == 'r' || asa == 't' || asa == 'X' || lead.x < sW)
		a = false;
	else a = true;

	if (dww == 'q' || dww == 'e' || dww == 'r' || dww == 't' || aww == 'q' || aww == 'e' || aww == 'r' || aww == 't')
		w = false;
	else w = true;

	if (dss == 'q' || dss == 'e' || dss == 'r' || dss == 't' || ass == 'q' || ass == 'e' || ass == 'r' || ass == 't')
		s = false;
	else s = true;

	//�����ж�
	if (awa == '*' || asa == '*' || dwd == '*' || dsd == '*' || aww == '*' || ass == '*' || dww == '*' || dss == '*')
		deli = true;
	else deli = false;

	//�����ж�
	if (aw == 'y' || as == 'y' || dw == 'y' || ds == 'y') death = true;
	if (aw == 'u' || as == 'u' || dw == 'u' || ds == 'u') death = true;
	if (aw == 'i' || as == 'i' || dw == 'i' || ds == 'i') death = true;
	if (aw == 'p' || as == 'p' || dw == 'p' || ds == 'p') death = true;
	if (aw == 's' || as == 's' || dw == 's' || ds == 's') death = true;
	if (y >= rH) death = true;

	//�����ж�
	if (asd == 'q' || asd == 'e' || asd == 'r' || asd == 't' || dsa == 'q' || dsa == 'e' || dsa == 'r' || dsa == 't')
		tum = true;
	else tum = false;

	return true;
}

bool Lead::FireCD()
{
	if (clock() - start > cd)
		fire = true;
	return true;
}

bool Lead::FireBullets()     //����0��ʾû�з����ӵ�
{
	start = clock();
	Bullet* Bullet_tmp = new Bullet;
	Bullet_tmp->x = x;
	Bullet_tmp->y = y;
	Bullet_tmp->f = f;
	Bullet_tmp->next = Bullet_head->next;
	Bullet_head->next = Bullet_tmp;
	fire = false;
	return true;
}

bool BlastBall::Explode(int unit1, int unit2) // ����u1 = 1, u2 = 2, ����u1 = u2 = 3;
{
	SpecialBrick* StoneBrick_tmp = StoneBrick_head->next;
	BlastBall* RedBall_tmp = RedBall_head->next;
	BlastBall* BlackBall_tmp = BlackBall_head->next;

	if (!StoneBrick_tmp) return false;
	unit1 *= B;
	unit2 *= B;
	while (StoneBrick_tmp)
	{
		if (StoneBrick_tmp->x >= x - unit1 && StoneBrick_tmp->x <= x + unit1 && StoneBrick_tmp->y >= y - unit1 && StoneBrick_tmp->y <= y + unit1)
			StoneBrick_tmp->del = true;
		StoneBrick_tmp = StoneBrick_tmp->next;
	}

	while (RedBall_tmp)
	{
		if (RedBall_tmp->x >= x - unit1 && RedBall_tmp->x <= x + unit1 && RedBall_tmp->y >= y - unit1 && RedBall_tmp->y <= y + unit1)
			RedBall_tmp->del = RedBall_tmp->ex = true;
		RedBall_tmp = RedBall_tmp->next;
	}

	while (BlackBall_tmp)
	{
		if (BlackBall_tmp->x >= x - unit1 && BlackBall_tmp->x <= x + unit1 && BlackBall_tmp->y >= y - unit1 && BlackBall_tmp->y <= y + unit1)
			BlackBall_tmp->del = BlackBall_tmp->ex = true;
		BlackBall_tmp = BlackBall_tmp->next;
	}

	if (lead.x >= x - unit2 && lead.x <= x + unit2 && lead.y >= y - unit1 && lead.y <= y + unit1)
		lead.death = true;
	return true;
}


bool NaturalLaw()
{
	CommonBrick* CommonBrick_tmp = CommonBrick_head->next;
	SpecialBrick* StoneBrick_tmp = StoneBrick_head->next;
	SpecialBrick* IronBrick_tmp = IronBrick_head->next;
	GlassBrick* GlassBrick_tmp = GlassBrick_head->next;
	BlastBall* RedBall_tmp = RedBall_head->next;
	BlastBall* BlackBall_tmp = BlackBall_head->next;
	MoveBall* PurpleBall_tmp = PurpleBall_head->next;
	MoveBall* GreyBall_tmp = GreyBall_head->next;
	WaterBall* WaterBall_tmp = WaterBall_head->next;
	Bullet* Bullet_tmp = Bullet_head->next;

	//����
	lead.v_y = lead.v_y - lead.g;

	if (!lead.s)
	{
		if (lead.v_y < 0)
		{
			lead.v_y = 0;
			lead.p = 2;
		}
	}

	if (!lead.w)
	{
		lead.v_y = 0;
		lead.y++;//�����Ϸ�ǽ��
	}

	//�ϱ߽�����
	if (lead.y <= sH)
	{
		lead.v_y = 0;
		lead.y++;
	}

	lead.y = lead.y - lead.v_y;

	if (lead.tum)//�Զ�����
		lead.y--;

	if (lead.death)
	{
		death.play_once();
		putimage(200, 200, &������ʾ����ͼ, SRCAND);
		putimage(200, 200, &������ʾ, SRCPAINT);
		FlushBatchDraw();
		while (1)
		{
			ExMessage msg;
			peekmessage(&msg, 2 );
			if (msg.message == WM_KEYDOWN)
			{
				if (msg.vkcode == 'r' || msg.vkcode == 'R')
				{
					longjmp(play, 1);
				}
			}
			if (_kbhit()) // Esc�����⴦��������ֲ��ȶ�����
			{
				int k = _getch();
				if (k == 27) // ESC��
				{
					game = false;
				}
			}
		}
	}

	//�ӵ�
	while (Bullet_tmp)
	{
		if (Bullet_tmp->del == true) {
			Delete(Bullet_head, Bullet_tmp);
			break;
		}
		else Bullet_tmp->x += Bullet_tmp->v * Bullet_tmp->f;
		Bullet_tmp = Bullet_tmp->next;
	}

	//����
	while (PurpleBall_tmp)
	{
		if (PurpleBall_tmp->fly) {
			MoveBall* FloatBall = PurpleBall_tmp;
			PurpleBall_tmp = PurpleBall_tmp->next;
			FloatBall->showy(IronBrick_head, PurpleBall_head, 'r');
			continue;
		}
		else PurpleBall_tmp->move();
		PurpleBall_tmp = PurpleBall_tmp->next;
	}

	//����
	while (GreyBall_tmp)
	{
		if (GreyBall_tmp->fly) {
			MoveBall* FloatBall = GreyBall_tmp;
			GreyBall_tmp = GreyBall_tmp->next;
			FloatBall->showy(StoneBrick_head, GreyBall_head, 'e');
			continue;
		}
		else GreyBall_tmp->move();
		GreyBall_tmp = GreyBall_tmp->next;
	}

	//��ש
	while (IronBrick_tmp)
	{
		if (IronBrick_tmp->invert) {
			IronBrick_tmp->index += 4;
			if (IronBrick_tmp->index >= 800) {
				Increase(StoneBrick_head, IronBrick_tmp->x, IronBrick_tmp->y);
				SpecialBrick* cur = IronBrick_tmp;
				IronBrick_tmp = IronBrick_tmp->next;
				Delete(IronBrick_head, cur);
				continue;
			}
		}
		IronBrick_tmp = IronBrick_tmp->next;
	}

	//ʯש
	while (StoneBrick_tmp)
	{
		if (StoneBrick_tmp->invert)
		{
			StoneBrick_tmp->index += 5;
			if (StoneBrick_tmp->index >= 800)
			{
				Increase(IronBrick_head, StoneBrick_tmp->x, StoneBrick_tmp->y);
				SpecialBrick* cur = StoneBrick_tmp;
				StoneBrick_tmp = StoneBrick_tmp->next;
				Delete(StoneBrick_head, cur);
				continue;
			}
		}
		if (StoneBrick_tmp->del) {
			SpecialBrick* cur = StoneBrick_tmp;
			StoneBrick_tmp = StoneBrick_tmp->next;
			Delete(StoneBrick_head, cur);
			continue;
		}
		StoneBrick_tmp = StoneBrick_tmp->next;
	}

	//����
	while (RedBall_tmp)
	{
		if (RedBall_tmp->ex) RedBall_tmp->Explode(1, 2);
		if (RedBall_tmp->del)
		{
			explode.play_once();
			Delete(RedBall_head, RedBall_tmp);
			break;
		}
		RedBall_tmp = RedBall_tmp->next;
	}

	//����
	while (BlackBall_tmp)
	{
		if (BlackBall_tmp->ex) BlackBall_tmp->Explode(3, 3);
		if (BlackBall_tmp->del)
		{
			explode.play_once();
			Delete(BlackBall_head, BlackBall_tmp);
			break;
		}
		BlackBall_tmp = BlackBall_tmp->next;
	}

	//����ש
	while (GlassBrick_tmp)
	{
		if (GlassBrick_tmp->del)
		{
			Delete(GlassBrick_head, GlassBrick_tmp);
			break;
		}
		GlassBrick_tmp = GlassBrick_tmp->next;
	}

	//ˮ��
	while (WaterBall_tmp)
	{
		WaterBall_tmp->index++;
		WaterBall_tmp = WaterBall_tmp->next;
	}

	//������
	delivery.index++;

	return true;

}

bool FreeAllList()
{
	FreeList(CommonBrick_head);
	FreeList(StoneBrick_head);
	FreeList(IronBrick_head);
	FreeList(GlassBrick_head);
	FreeList(RedBall_head);
	FreeList(BlackBall_head);
	FreeList(PurpleBall_head);
	FreeList(GreyBall_head);
	FreeList(WaterBall_head);
	FreeList(Bullet_head);
	return true;
}

bool Lead::Move()
{
	if (isKeyPressed(68) && d)
	{
		if (isKeyPressed(65) && a)
		{
			x = x - v_x;
			f = -1;
		}
		x = x + v_x;
		f = 1;
	}
	else if (isKeyPressed(65) && a)
	{
		if (isKeyPressed(68) && d)
		{
			x = x + v_x;
			f = 1;
		}
		x = x - v_x;
		f = -1;
	}
	return true;
}

bool Lead::Jump()
{
	y--;
	v_y = v0;
	p--;
	return true;
}

bool MoveBall::showy(SpecialBrick* special_head, MoveBall* moveball_head, char c)
{
	char aw, dw;
	y -= 0.5;
	aw = GetPosition(x, y - 1);
	dw = GetPosition(x + B - 1, y - 1);
	if ((aw == '0' && dw == '0') || (aw == 's' && dw == 's')) return false;
	else {
		if (aw == c || dw == c) {
			SpecialBrick* topl = NULL, * topr = NULL;
			if (Traverse(special_head, topl, x, y - 1)) topl->invert = true;
			if (Traverse(special_head, topr, x + B - 1, y - 1)) topr->invert = true;
		}
		Delete(moveball_head, this);
	}
	return true;
}

bool MoveBall::move()
{
	char awa, asa, dwd, dsd;
	x += v_move * t;
	double a = x, a1 = y + B + 0.1, d = x + B;
	awa = GetPosition(a, y);
	dwd = GetPosition(d, y);
	asa = GetPosition(a, a1);
	dsd = GetPosition(d, a1);
	if ((awa != '0' && awa != '*' && awa != 'i' && awa != 's' && awa != 'p'))
	{
		t = -t;
		return false;
	}
	else if ((dwd != '0' && dwd != '*' && dwd != 'i' && dwd != 's' && dwd != 'p'))
	{
		t = -t;
		return false;
	}
	else if (asa == '0' || dsd == '0')
	{
		t = -t;
		return false;
	}
	return true;

}

bool Bullet_Judge()
{
	CommonBrick* CommonBrick_tmp = NULL;
	SpecialBrick* StoneBrick_tmp = NULL;
	SpecialBrick* IronBrick_tmp = NULL;
	GlassBrick* GlassBrick_tmp = NULL;
	BlastBall* RedBall_tmp = NULL;
	BlastBall* BlackBall_tmp = NULL;
	MoveBall* PurpleBall_tmp = NULL;
	MoveBall* GreyBall_tmp = NULL;
	WaterBall* WaterBall_tmp = NULL;
	Bullet* Bullet_tmp = Bullet_head->next;

	int dx, dy[2] = { 0,1 };

	while (Bullet_tmp)
	{
		dx = Bullet_tmp->f;
		if (dx < 0) dx++;
		for (int i = 0; i < 2; ++i)
		{
			char sym = '0';
			double x = Bullet_tmp->x + dx * Bullet_B;     //�����ʾ�ӵ�����
			double y = Bullet_tmp->y + dy[i] * Bullet_B;
			//ȷ���ӵ�״̬
			if (Traverse(CommonBrick_head, CommonBrick_tmp, x, y)) sym = 'q';
			else if (Traverse(StoneBrick_head, StoneBrick_tmp, x, y)) sym = 'e';
			else if (Traverse(IronBrick_head, IronBrick_tmp, x, y)) sym = 'r';
			else if (Traverse(GlassBrick_head, GlassBrick_tmp, x, y)) sym = 't';
			else if (Traverse(RedBall_head, RedBall_tmp, x, y)) sym = 'y';
			else if (Traverse(BlackBall_head, BlackBall_tmp, x, y)) sym = 'u';
			else if (Traverse(PurpleBall_head, PurpleBall_tmp, x, y)) sym = 'i';
			else if (Traverse(GreyBall_head, GreyBall_tmp, x, y)) sym = 'p';
			else if (Traverse(WaterBall_head, WaterBall_tmp, x, y)) sym = 's';
			//�ж�
			if (sym == '0');
			else if (sym == 'q' || sym == 'w' || sym == 'e' || sym == 'r') Bullet_tmp->del = true;
			else if (sym == 't') GlassBrick_tmp->del = Bullet_tmp->del = true;
			else if (sym == 'y') RedBall_tmp->del = RedBall_tmp->ex = Bullet_tmp->del = true;
			else if (sym == 'u') BlackBall_tmp->del = BlackBall_tmp->ex = Bullet_tmp->del = true;
			else if (sym == 'i' || sym == 'o') PurpleBall_tmp->fly = Bullet_tmp->del = true;
			else if (sym == 'p' || sym == 'a') GreyBall_tmp->fly = Bullet_tmp->del = true;
			else if (sym == 's');
		}
		Bullet_tmp = Bullet_tmp->next;
	}
	return true;
}

bool Key_Detection()//�������
{
	ExMessage msg;
	peekmessage(&msg, 2);

	if (msg.message == WM_KEYDOWN)
	{
		if (msg.vkcode == 'R' || msg.vkcode == 'r')
			longjmp(play, 1);
		else if ((msg.vkcode == 'J' || msg.vkcode == 'j') && lead.fire)
		{
			fire.play_once();
			lead.FireBullets();
		}
		else if ((msg.vkcode == 'K' || msg.vkcode == 'k' || msg.vkcode == ' ') && lead.p)
		{
			jump.play_once();
			lead.Jump();
		}
		else if ((msg.vkcode == 'L' || msg.vkcode == 'l') && lead.deli)
		{
			level++;
			longjmp(play, 1);
		}
	}
	if (_kbhit()) // Esc�����⴦��������ֲ��ȶ�����
	{
		int k = _getch();
		if (k == 27)//ESC��
		{
			game = false;
		}
	}
	return true;
}

void down_shift_once()
{
	static int key = 0;
	key++;
	if (key == 1)
	{
		keybd_event(VK_SHIFT, 0, 0, 0);
		keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
	}
}

std::vector<Tip> outtext(int level) {
	TCHAR k[1024];
	char _buf[1024];
	wchar_t buf[1024]={L'\0'};
	wchar_t text[1024];
	char addr[1024];
	sprintf_s(addr, 1024, "resource\\Level\\%d.txt", level);
	std::vector<Tip> tips;
	int x, y;
	FILE* file;
	fopen_s(&file, addr, "r");
	if (file == NULL) {
		printf("file open error!");
		exit(1);
	}
	fread(_buf, 1, 1024, file);
	MultiByteToWideChar(CP_ACP, 0, _buf,-1,buf,1024); // 
	buf[1023] = L'\0';
	for (unsigned int i = 0; buf[i] != L'&'; i++) {
		if (buf[i] == L'#') {
			unsigned int j = ++i;
			while (buf[i++] != L'#');
			wcsncpy(text, &buf[j], i - j - 1);
			text[i - j - 1] = L'\0';
			x = _wtoi(&buf[i]);
			while (buf[i++] != L'#');
			y = _wtoi(&buf[i]);
			tips.push_back(Tip(x, y, text));
			while (1) {
				while (buf[i++] !=L'#') {
					if (buf[i] == L'&')
					{
						fclose(file);
						return std::move(tips);
					}	
				}
				unsigned int j = i;
				while (buf[i++] != L'#');
				wcsncpy(text, &buf[j], i - j - 1);
				text[i - j - 1] = L'\0';
			
				x = _wtoi(&buf[i]);
				while (buf[i++] != L'#');
				y = _wtoi(&buf[i]);
			
				tips.push_back(Tip(x, y, text));
			}
		}
	}
	fclose(file);
	return std::move(tips);
}

void Play(int x) {
	game = true; //�ж���Ϸ�Ƿ����

	down_shift_once();
	MyMouse::SetStyle('n');
	play_bgm.seek_to_start();
	play_bgm.play();
	level = x;
	LoadResource();//������Դ

	setjmp(play);//��ʼ��־

	FreeAllList();

	//��������
	CreateList();

	tips = outtext(level);

	clock_t st, ed;
	double spt, fps = 0.003, tim;	//0.004	ԽСԽ��

	while (1)
	{
		st = clock();

		//����־������ֵ
		lead.FireCD();
		lead.Judge();
		lead.Move();

		Bullet_Judge();

		//���ݱ�־��������
		NaturalLaw();

		Key_Detection();

		BeginBatchDraw();
		cleardevice();
		Draw();
		FlushBatchDraw();

		if (!game)
		{
			EndBatchDraw();
			FreeAllList();
			play_bgm.pause();
			longjmp(set_level, 1);
			break;
		}

		ed = clock();
		spt = double(ed - st) / CLOCKS_PER_SEC;
		tim = (fps - spt) * 1000;
		if (tim > 0) Sleep(tim);
	}
}