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
	~App();
private:
	void DoFrame();
private:
	Window wnd;			//���ڶ���
	ChiliTimer timer;	//��ʱ��
	std::vector<std::unique_ptr<class Box>> boxes;  //��һ�ѷ���� ����
};

