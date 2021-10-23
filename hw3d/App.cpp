#include "App.h"
#include "Box.h"
#include <memory>

App::App()
	:
	wnd(800, 600, "The Donkey Fart Box") //这里就调用了wnd的构造函数 注册了窗口类，创建窗口 显示 等等
{
	//随机数
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 1.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.08f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	
	//创建一堆盒子
	for (auto i = 0; i < 180; i++)
	{
		boxes.push_back(std::make_unique<Box>(wnd.Gfx(), rng, adist, ddist, odist, rdist));
	};

	// 根据视椎体 生成投影矩阵存在gfx中
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveFovLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

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

App::~App()
{
}

//每一帧调用,注意这是一帧的内容  EndFrame就是结束一帧
void App::DoFrame()
{
	auto dt = timer.Mark(); //自己看内部实现，如果每帧调用Mark()函数，则返回值就是每一帧之间的间隔时间 nice
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);//每帧都要执行清屏操作 不然看个寂寞
	for (auto& b : boxes)
	{
		b->Update(dt);		//每帧都改变 每一个盒子对象的 变换属性为新的随机值
		b->Draw(wnd.Gfx()); //Draw函数里 对binds容器中的 可绑定的东西 进行批量绑定
	}
	wnd.Gfx().EndFrame();
}



 
