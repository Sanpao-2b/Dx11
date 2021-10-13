#include "App.h"
#include <sstream>
#include <iomanip>

App::App()
	:
	wnd(800, 600, "The Donkey Fart Box") //这里就调用了wnd的构造函数 注册了窗口类，创建窗口 显示 等等
{}

int App::Go()
{	
	//消息处理函数在窗口内部了
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		//不是退出消息就执行DoFrame，这里是游戏逻辑；一直循环直到返回退出消息为止
		DoFrame();
	}
}

//每一帧调用
void App::DoFrame()
{
	wnd.Gfx().EndFrame(); //Gfx()返回的是指向Graphics对象的指针
}


 
