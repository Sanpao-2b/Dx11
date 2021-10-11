#pragma once
#include "Window.h"
#include "ChiliTimer.h"
class App 
{
public:
	//构造函数仅仅用来初始化一个窗口，调用了wnd的构造函数 创建了一个窗口，以前是在WinMain直接创建的，现在在这里创建，仅此而已
	App();
	//开始游戏循环
	int Go();
private:
	//存放每一帧的游戏逻辑
	void DoFrame();
private:
	//应用程序  有窗口没毛病吧？
	Window wnd;

	//给应用程序添加一个计时器
	ChiliTimer timer;
};

