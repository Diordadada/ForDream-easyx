#pragma once
#include <windows.h>

#include <iostream>

void Setting();

struct bn* create(int x, int y, int w, int h, const wchar_t* pt, int a);

struct bnj* create(int x, int y, int w, int h, int t, wchar_t* pt);

struct bnj* create1(int x, int y, int w, int h, const wchar_t* pt);    //��ʼ��������ť  ���곤������ 

struct bn1* create(int x, int y, int s, const wchar_t* pt);

struct bn     //��ť
{
	int x;      // ����  ����
	int y;
	int w;
	int h;
	int t;
	COLORREF co[5];        //��ɫ
	wchar_t* pt;                //����
};

struct bn1     //�ְ�ť
{
	int x;      // ���� ��С
	int y;
	int s;
	COLORREF co;        //��ɫ
	wchar_t* pt;                //����
};

struct bnj     //������ť
{
	int x;      // ����  ����
	int y;
	int w;
	int h;
	COLORREF co[5];        //��ɫ
	wchar_t pt;                //����
	wchar_t* p;                //����
};

void draww(struct bn1* bp);

void drawbn(struct bn* bp, int  zh, int zw, int zl);

void drawj(struct bnj* bp, int  zh, int zw, int zl);

void drawj1(struct bnj* bp, int  zh, int zw, int zl);      //��������ť

int mbn1(struct bn* bp, MOUSEMSG m);

int mbn(struct bnj* bp, MOUSEMSG m);

int dlickb(struct bnj* bp, MOUSEMSG m);

int dlick(struct bn* bp, MOUSEMSG m);
