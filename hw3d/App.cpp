#include "App.h"
#include <sstream>
#include <iomanip>

App::App()
	:
	wnd(800, 600, "The Donkey Fart Box") //这里就调用了wnd的构造函数 注册了窗口类，创建窗口 显示 等等
{}

int App::Go()
{
	//移除以前的消息处理 放到了Window里的静态函数ProcessMessages() 并且把GetMessage()换成了Peekmessage()以解决 无消息时 游戏逻辑停止的bug
	/*
		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)//gResult>0处理消息
		{
			TranslateMessage(&msg); //翻译判断是否产生WM_CHAR
			DispatchMessage(&msg);  //把消息发送给 消息处理函数 消息处理函数会 把对应每个消息类型 产生不同的行为。
		
			DoFrame();
		}
		//检查如果GetMessage()函数出错了
		if (gResult == -1)
		{
			throw CHWND_LAST_EXCEPT();
		}
		else
			// 这里的wParam是传给PostQuitMessage()的那个数字
			return msg.wParam;
	*/
	
	//消息处理函数在窗口内部了
	while (true)
	{
		//ProcessMessages()有两种情况的optional返回值
		//1.ecode接收到WM_QUIT 并且因为ecode不为空所以执行if内部语句
		//2.返回的是空，则ecode接收到空的optional 不执行内部语句
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		//不是退出消息就执行DoFrame，这里是游戏逻辑；一直循环直到返回退出消息为止
		DoFrame();
	}
}

//实现每一帧的方法是 把这个函数放到消息循环里头，一产生消息就放进系统的消息队列，并调用GetMessage()逐条取出来处理，
//一旦没消息了，DoFrame()就不执行了。。整个游戏直接停止了？
//解决方法：用PeekMessage() 返回optional类型的变量
void App::DoFrame()
{
	//获取自窗口建立以来经过的时间
	//Peek()会一直返回从窗口建立到现在经历的时间 
	//预测一波：如果鼠标移动，他就会更新标题，鼠标不动就不会 ———————————— correct！
	const float t = timer.Peek();
	std::ostringstream oss;
	oss << "Time elapsed:" << std::setprecision(1) << std::fixed << t << "s";
	wnd.SetTitle(oss.str());
}

/*
	std::setprecision(1) 控制有效数字为1位

	俺们不希望如果我们不输入，游戏画面就直接静止
	解决办法： 不用GetMessage()函数了， 用PeekMessage()!!!!我们指定消息处理方式为 PM_REMOVE，虽然仍然是处理后从列表中删除
			  但是，PeekMessage()会在没有消息是立刻返回
			  有消息返回非0 没有消息返回0

	BOOL PeekMessage(
	  LPMSG lpMsg,
	  HWND hWnd,
	  UINT wMsgFilterMin,
	  UINT wMsgFilterMax,
	  UINT wRemoveMsg
	);
	lpMsg：		   
		[out] 指向接收消息信息的MSG结构的指针。
	hWnd：		   
		[in] 要检查其消息的窗口的句柄。
	wMsgFilterMin：
		[in] 指定要检查的消息范围中的第一条消息的值。
	wMsgFilterMax：
		[in] 指定要检查的消息范围中最后一条消息的值。
	wRemoveMsg：   
		[in] 指定如何处理消息。此参数可以是以下值之一。
			PM_NOREMOVE ：在PeekMessage处理后，消息不会从队列中删除。
			PM_REMOVE   ：消息在PeekMessage处理后从队列中删除。一个例外是 WM_PAINT 消息，它不会从队列中删除。
	返回值：
		有消息，返回非0，没有消息，返回0；所以我们需要检查一下返回值，因为这个函数返回值没有错误信息
		想必GetMessage() 出错时返回-1



*/
 
