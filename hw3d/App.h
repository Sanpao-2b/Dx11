#pragma once
#include "Window.h"
#include "ChiliTimer.h"
class App 
{
public:
	//���캯������������ʼ��һ�����ڣ�������wnd�Ĺ��캯�� ������һ�����ڣ���ǰ����WinMainֱ�Ӵ����ģ����������ﴴ�������˶���
	App();
	//��ʼ��Ϸѭ��
	int Go();
private:
	//���ÿһ֡����Ϸ�߼�
	void DoFrame();
private:
	//Ӧ�ó���  �д���ûë���ɣ�
	Window wnd;

	//��Ӧ�ó������һ����ʱ��
	ChiliTimer timer;
};

