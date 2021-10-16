#include "Window.h"
#include <sstream>
#include "resource.h"

//这是在初始化wndClass静态实例对象 调用了默认构造函数？
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr)) 
{
	//创建窗口类的结构体变量，定义其所有的属性
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance(); 
	wc.hIcon = static_cast<HICON>(LoadImage(hInst,MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));

	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName(); 
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));;

	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance()); //这里会管理单例模式中的对象析构问题吧
}

const char * Window::WindowClass::GetName() noexcept
{
	return wndClassName; 
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;  
}

//Window构造函数 创建窗口
Window::Window(int width, int height, const char * name) //移除了noexcept 否则无法抛出异常
	://忘了存放传入的宽和高了这里补充一下
	width(width),
	height(height)
{
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	
	//这里纠正错误，去掉FAILED
	//FAILED用法：判断括号里的数是否<0 若<0则false 注意0不算哦！！！！！！
	//FAILED是用来检查HRESULT的，但是并不是所有窗口函数都返回HRESULT 有的是返回整数 要注意辨别
	//AdjustWindowRect 返回的是BOOL 如果出错返回0
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{	
		throw CHWND_LAST_EXCEPT(); //俺们检查调整窗口是否失败，如果失败 抛
	}

	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,CW_USEDEFAULT,wr.right - wr.left, wr.bottom - wr.top, 
		nullptr,nullptr,WindowClass::GetInstance(),this
		);


	if (hWnd == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}
	
	//显示窗口
	ShowWindow(hWnd, SW_SHOWDEFAULT); 
	//对图形类对象进行初始化赋值
	pGfx = std::make_unique<Graphics>(hWnd);
}


Window::~Window()
{
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	//当消息队列中有消息时，移除并取出最前面的消息
	//返回值为真，则表示取到消息了
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		//检查是不是退出消息，记住，所有的GetMessage()函数的返回值都会直接告诉是不是返回值 即返回-1就是退出，而PeekMessage()不会告知
		//所以手动的取出msg中的成员message 判断是不是退出消息
		if (msg.message == WM_QUIT)
		{
			return (int)msg.wParam;
		}
	
		TranslateMessage(&msg); //翻译判断是否产生WM_CHAR
		DispatchMessage(&msg);  //把消息发送给 消息处理函数 消息处理函数会 把对应每个消息类型 产生不同的行为。
	}	
	return {}; //返回空的optional
}

Graphics& Window::Gfx()
{
	if (!pGfx)
	{
		throw CHWND_NOGFX_EXCEPT(); 
	}
	return *pGfx;
}

//消息处理函数
LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE) 
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);  
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

//真正的消息处理函数
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE: 
		PostQuitMessage(0); 
		return 0; 
	case WM_KILLFOCUS:
		kbd.ClearState(); 
		break;
/***************************** 按键消息 直接调用接口 封装得很nice*******************************/
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled())
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
		/***************************** 按键消息 直接调用接口 封装得很nice*******************************/
	
		/***************************** 鼠标消息 直接调用接口 封装得很nice*******************************/
	case WM_MOUSEMOVE://必须加花括号括起来，否则pt会重定义错误
	{	
		const POINTS pt = MAKEPOINTS(lParam);
		//if inside client region 
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			//更新坐标，把Move事件存到buffer中
			mouse.OnMouseMove(pt.x, pt.y);

			//msg产生后，鼠标在窗口内，但是这个时刻的isInWindow变量如果是false 
			//说明是鼠标之前在client region之外，刚刚进入client region
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);     //WINAPI函数，这个函数作用是 即便鼠标离开窗口，也可以继续接收MouseMove 消息
				mouse.OnMouseEnter(); //记录鼠标Enter client region 事件
			}
		}
		else
		{
				/*
					注意 "&"位与  "|"位或
					MK_LBUTTON | MK_RBUTTON 结果是代表左右键的两个位上的值为1 其他位全是0
					再跟wParam按位与，wParam其他位上的数值不管1 0 全部变成0，然后左右按键位上 只要存在一个是1 按位与的结果即为真
					这行代码第一次执行的时候，即鼠标刚从client region出来时， 如果是拖拽的状态，即鼠标是按下状态 ，则执行if内语句
					如果是拖拽状态，继续响应鼠标的移动事件，虽然没在client region 但是 由于之前进入的时候设置过SetCapture(hWnd)捕捉鼠标，
					只要不释放，就一直产生WM_MOUSEMOVE消息 一直执行这几次逻辑判断 到这里 符合条件 给事件buffer里添加事件
				*/
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			//否则 如果第一次离开窗口 不是拖拽状态，或者是在窗口某个时刻不拖拽了 松开了，
			//那么这次的WM_KEYDOWN的执行逻辑到这里 符合了,释放掉鼠标的域外抓捕，之后就不产生WM_MOUSEMOVE消息了，并且给事件队列加一个鼠标离开的事件
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}

	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam); //这是一个宏从wParam中提取出delta
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
		/***************************** 鼠标消息 直接调用接口 封装得很nice*******************************/
	}


	return DefWindowProc(hWnd, msg, wParam, lParam);
}


//异常处理类
Window::HrException::HrException(int line, const char * file, HRESULT hr) noexcept
	:
	Exception(line, file) ,
	hr(hr)
{}

const char * Window::HrException::what() const noexcept
{
	std::stringstream oss;
	oss << GetType() << std::endl
		<< "[Error code]" << GetErrorCode() << std::endl
		<< "[Description]" << GetErrorDescription() << std::endl
		<< GetOriginString(); 
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char * Window::HrException::GetType() const noexcept
{
	return "Chili Window Exception";
}

//静态函数
std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr, 
		hr, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf),
		0, 
		nullptr
	);

	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	std::string errorString = pMsgBuf; //如果不是0则成功 把缓冲区的字符串拷贝出去
	LocalFree(pMsgBuf); //释放掉系统分配的存放字符串的那块内存
	return errorString;
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hr);
}

const char * Window::NoGfxException::GetType() const noexcept
{
	return "Chili Window Exception [No Graphics]";
}
