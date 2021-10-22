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
	~App();
private:
	void DoFrame();
private:
	Window wnd;			//窗口对象
	ChiliTimer timer;	//计时器
	std::vector<std::unique_ptr<class Box>> boxes;  //放一堆方块的 容器
};

