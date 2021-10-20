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

//每一帧调用,注意这是一帧的内容  EndFrame就是结束一帧
void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f; 
	wnd.Gfx().ClearBuffer(c, c, 1.0f);				 
	wnd.Gfx().DrawTestTriangle(
		timer.Peek(), 
		wnd.mouse.GetPosX()/400.0f - 1,  //视口坐标系中，X轴从左往右-1 ~ 1 而鼠标类从左到右0~800 所以要/400 -1 把范围限制到(-1, 1)
		-wnd.mouse.GetPosY()/300.0f + 1); //Y轴同理，不同的是 视口Y轴从下往上-1 ~ 1，这与鼠标类中鼠标往下是增加相反 所以要取反
	wnd.Gfx().EndFrame(); 
} 



 
