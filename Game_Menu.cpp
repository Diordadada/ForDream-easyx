#include "Game_Menu.h"
#include "graphics.h"
#include "Tools.h"
#include "time.h"

static int Win_W = 0;
static int Win_H = 0;

#define font_Magnitude 5 //����仯������
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)  //������ⰴ���ĵ���¼�

MyMusic menu_bgm(L"resource\\Music\\menu_bgm.mp3");

LOGFONT f_title;                                 //�������

int Menu() {
	InitFullGraph(Win_W, Win_H);

	IMAGE Menu; // ����ͼƬ
	IMAGE Title1, Title2; // ��������Ϸ��
	MyMouse::init_mouse(L"resource\\Mouse\\normal.cur", 1.0, L"resource\\Mouse\\check.cur", 1.0);
	MyMouse::SetStyle('n');

	menu_bgm.seek_to_start();
	menu_bgm.play();

	loadimage(&Menu, L"resource\\Background\\menu_bg.png", Win_W, Win_H);   // ���ر���ͼƬ
	putimage(0, 0, &Menu);

	int title_width = Win_W / 2;
	int title_height = Win_H / 4;      // ������Ϸ���Ŀ�Ⱥ͸߶�

	int title_xpos = (Win_W - title_width) / 2;
	int title_ypos = Win_H / 5;        // ������Ϸ��λ��

	loadimage(&Title1, L"resource\\Item\\title.png", title_width, title_height); // ������������Ϸ��
	loadimage(&Title2, L"resource\\Item\\title_mask.png", title_width, title_height);
	putimage(title_xpos, title_ypos, &Title2, SRCAND);
	putimage(title_xpos, title_ypos, &Title1, SRCPAINT);

	
	gettextstyle(&f_title);						   // ��ȡ��ǰ��������

	f_title.lfHeight = Win_H / 20;			   // ��������߶�
	_tcscpy_s(f_title.lfFaceName, _T("���Ĳ���"));   // �������廪�Ĳ���
	f_title.lfQuality = ANTIALIASED_QUALITY;		   // �������Ч��Ϊ�����  
	settextstyle(&f_title);						   // ����������ʽ
	settextcolor(RGB(128, 42, 42));            // ����������ɫ
	setbkmode(TRANSPARENT);                    // �������屳��Ϊ͸��


	TCHAR s[3][10] = { _T("��ʼ��Ϸ"),_T("��Ϸ����"),_T("�˳���Ϸ") };// ѡ������

	int option_height = Win_H / 15;                              // ����ѡ���ĸ߶�

	int option_xpos = Win_W / 2, option_ypos = Win_H / 2;    // ����ѡ��λ��

	int r[3][4];                                                      // ѡ��Ķ�ά����

	for (int i = 0; i < 3; ++i)                                       // ��ʼ��ѡ���λ��
	{
		r[i][0] = option_xpos - textwidth(s[i]) / 2;
		r[i][1] = option_ypos + (textheight(s[i]) + option_height) * i;
		r[i][2] = r[i][0] + textwidth(s[i]);
		r[i][3] = r[i][1] + textheight(s[i]);
	}

	for (int i = 0; i < 3; ++i)
		outtextxy(r[i][0], r[i][1], s[i]);                           // ��ӡѡ��

	MOUSEMSG m;                                                      // ���ָ��
	int event = 0, tmp = 0;                                          // ����¼�����
	while (true)
	{
		m = GetMouseMsg();                                           // ��ȡ�����Ϣ
		tmp = button_judge(m.x, m.y, r);
		switch (m.uMsg)
		{
		case WM_MOUSEMOVE:                                           // ����ƶ�
			setfillstyle(BS_DIBPATTERN, NULL, &Menu);               // ���ñ���ͼƬ���
			if (tmp && event != tmp)
			{
				if (event) font_reset(event - 1, r, s);
				event = tmp;                                     // ��¼��ǰ����¼�
				font_special(event - 1, r, s);
			}
			else if (!tmp && event) {
				font_reset(event - 1, r, s);
				event = 0;
			}
			break;
		case WM_LBUTTONDOWN:                                          // ����������
			if (tmp) return tmp;
			break;
		}
		flushmessage(EM_MOUSE);                                      // ��������Ϣ������
	}
	menu_bgm.close();
	return 0;
}

int button_judge(int x, int y, int r[][4])   // ���λ���ж�
{
	for (int i = 0; i < 3; ++i)
		if (x > r[i][0] && x<r[i][2] && y>r[i][1] && y < r[i][3]) return i + 1;
	return 0;
}

void font_special(int op, int r[][4], TCHAR s[][10])  //���嶯̬��Ч
{
	int lfgap;
	BeginBatchDraw();
	clearrectangle(r[op][0], r[op][1], r[op][2], r[op][3]);
	solidrectangle(r[op][0], r[op][1], r[op][2], r[op][3]);
	for (int i = 1; i <= font_Magnitude; ++i)
	{
		++f_title.lfHeight, lfgap = i;
		settextstyle(&f_title);
		int rect_xpos = r[op][0] - textwidth(s[op]) / f_title.lfHeight / 2 * lfgap;    // ���嶯̬ѡ��λ��
		int rect_ypos = r[op][1] - lfgap / 2;
		outtextxy(rect_xpos, rect_ypos, s[op]);
		FlushBatchDraw();
		Sleep(20);
		if (i != font_Magnitude) {
			clearrectangle(rect_xpos, rect_ypos, rect_xpos + textwidth(s[op]), rect_ypos + textheight(s[op]));
			solidrectangle(rect_xpos, rect_ypos, rect_xpos + textwidth(s[op]), rect_ypos + textheight(s[op]));
		}
	}
	EndBatchDraw();
}

void font_reset(int op, int r[][4], TCHAR s[][10])    //����ѡ���С
{
	int rect_xpos = r[op][0] - textwidth(s[op]) / f_title.lfHeight / 2 * font_Magnitude;
	int rect_ypos = r[op][1] - (font_Magnitude + 1) / 2;                //ȷ������ǰѡ��λ��
	f_title.lfHeight -= font_Magnitude;                                 //���������С
	BeginBatchDraw();
	clearrectangle(rect_xpos, rect_ypos, rect_xpos + textwidth(s[op]), rect_ypos + textheight(s[op]));
	solidrectangle(rect_xpos, rect_ypos, rect_xpos + textwidth(s[op]), rect_ypos + textheight(s[op]));
	settextstyle(&f_title);
	outtextxy(r[op][0], r[op][1], s[op]);
	FlushBatchDraw();
	EndBatchDraw();
}