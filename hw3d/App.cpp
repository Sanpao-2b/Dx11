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
			//如果是退出消息 直接返回
			return *ecode;
		}
		//不是退出消息就执行DoFrame，这里是游戏逻辑；一直循环直到返回退出消息为止
		DoFrame();
	}
}

//每一帧调用
void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f; //time.peek()是从程序运行到现在的时间，c是个0~1变化的数 这也是颜色变化的范围
	wnd.Gfx().ClearBuffer(c, c, 1.0f);				 //红绿色是变化的数字
	
	wnd.Gfx().EndFrame(); //Gfx()返回的是指向Graphics对象的指针,调用图形类中的函数
}



 
