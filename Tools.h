#pragma once
#include "graphics.h"
#include <digitalv.h>
#include <vector>

//�ṩ���ģ�����ͬʱʹ�õĹ���

wchar_t __num_to_wchar(size_t num);
wchar_t* num_to_wchar(long long num);

class MyMouse;

class MyMouse {
public:
    static MOUSEMSG mouse;
    static HCURSOR normal;
    static HCURSOR check;
    static void init_mouse(const wchar_t* Normal_Path, double Normal_Size, const wchar_t* Check_Path, double Check_Size);
    static void SetStyle(char sign);
};

//IMA���ڴ�����ͼ�Ĳ���ͼ
class IMA
{
private:
	IMAGE* top = nullptr;
	IMAGE* bottom = nullptr;
	const wchar_t* path = nullptr;
	double radium;
public:
    IMA(const wchar_t* path);
	void load(int nWidth, int nHeight);
	void put(int dstX, int dstY);
	void rotate(double degree, int dstX, int dstY);
	~IMA();
};

class MciMusic {
public:
    //�ļ�·�� ���ָ�ʽ���ΪMP3��wav��ʽѭ��������
    void open();
    //����һ��
    void play();
    //�ظ�����
    void repeat();
    //��ͣ
    void pause();
    //������С��ΧΪ  0~1000
    void set_volume(DWORD volume_value);
    MciMusic(const wchar_t* file_name);
    void close();
    void seek_to_start();
private:
    const wchar_t* file_name;
    UINT wDeviceID;
    DWORD  dwValue;
    MCI_OPEN_PARMS mci_open_parms;
    MCI_PLAY_PARMS mci_play_parms;
    MCI_SEEK_PARMS  mci_seek_parms;
    MCI_GENERIC_PARMS mci_generic_parms;
    MCI_DGV_SETAUDIO_PARMS mci_dgv_saudio_parms;
};

class MyMusic {
private:
    MciMusic music;
public:
    MyMusic(const wchar_t* file_name);
    void play_once();
    void play();
    void pause();
    void set_volume(DWORD volume_value);
    void close();
    void seek_to_start();
};

int readf_level();

void InitFullGraph(int& Win_W, int& Win_H);

void GetGraphWH(int& Win_W, int& Win_H);



