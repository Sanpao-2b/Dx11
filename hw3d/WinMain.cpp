/*
	2021年10月10日14:46:54
	游戏窗口基本框架
	APP包含多个窗口，APP的游戏逻辑可以调用窗口的组件"鼠标、键盘、图形"等相关接口(public属性才行)

	我们的游戏程序包含多个窗口，而一个窗口有自己的组件(鼠标、键盘、图形处理)，
	消息循环：窗口里，按键、移动鼠标 系统会自动产生消息存放到消息队列中，我们在窗口类中的自定义的消息处理函数里取出来 挨个判断是什么类型的消息 然后调用相应的组件接口 进行存放
	对这些输入事件产生行为 则要靠俺们的App中的Doframe()函数里实现 取出对应的消息 定义行为。如果不定义 则不会产生行为

	目前的窗口执行逻辑：
	=> App{}.Go();创建临时App对象调用Go()函数
	=> Go()函数体内是一个无限循环while(true)，内部用if判断ProcessMessages()的返回值(类型为optional)，
			   如果是WM_QUIT则执行if内部语句终止程序，如果是空optional，则执行DoFrame()游戏逻辑
	=> ProcessMessages()是一个窗口类提供的公有的静态成员函数 用于处理每个窗口的消息，函数体内部是Message bump 消息泵，调用WIN32接口PeekMessage()从WinAPI取出消息
	=> 如果取出消息成功，则判断是不是WM_QUIT，如果是，则返回msg.wParam, WM_QUIT消息中的wParam参数装的就是退出程序的二进制代码
	=> 如果不是WM_QUIT则正常TranslateMessage()、DispatchMessage(),把消息msg传给HandleMsg()，这是自定义的WindowProc()函数，处理消息
	=> 处理方法：给想要处理的消息搞个case，不想处理的就交给系统默认DefWindowProc()函数处理；
			   如果是按键消息，则调用键盘组件的接口，更改相应按键状态，产生相应的按键事件存如按键buffer中
			   如果是鼠标消息，则调用鼠标组件的接口，更改相应按键状态，产生对应的鼠标事件存入鼠标buffer中
	=> 后续对这些按键、鼠标的输入产生对应的行为就要交给App即我们的游戏类Doframe中去调用"鼠标、键盘、图形"提供的公共接口 取出他们的buffer中的事件，产生相应的动作 更新游戏画面之类的
*/
#include "Window.h"
#include <sstream>
#include "App.h"
int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	try
	{
		//App{}相当于创建了临时对象，虽然是临时，但是Go()会一直循环，直到返回值是退出代码
		return App{}.Go();
	}

	// 如果throw出来了一个ChiliException类型(包括子类)的一个变量，则执行
	catch (const ChiliException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);//ok按钮和感叹号
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No Details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}


