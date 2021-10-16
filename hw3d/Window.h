#pragma once
#include "ChiliWin.h"//保证这个头文件在最顶端，因为如果包含其他头文件在这个的上面，那我们在ChiliWin.h包含的那些nonono就会失效
#include "ChiliException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
//c++17 可以选择返回一个int还是一个空的optional 配合消息处理很好 optional<T> 相当于T类型变量和一个bool值的打包，其中的bool用来表示T变量是否为空
#include <optional> 
#include <memory> //智能指针
class Window
{
public:
	//并且把这个类也弄成了一个莫得卵用的类 用来生儿子的
	class Exception : public ChiliException
	{
		//继承基类构造函数
		using ChiliException::ChiliException;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;//把hr错误代码 转换成人能看的字符串。。。
	};

	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;//HRESULT窗口程序专属的错误类
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;

		HRESULT GetErrorCode() const noexcept;         //返回成员变量hr
		std::string GetErrorDescription() const noexcept;    //调用TranslateErrorCode() 并返回
	private:
		HRESULT hr;
	};

	//如果成员pGfx为空 则抛出异常 
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};

private:
	//单例模式，全程序仅允许存在一个对象，私有构造函数，便是让任何人不得创建对象
	class WindowClass 
	{
	public: 
		static const char* GetName() noexcept;   //返回窗口类名
		static HINSTANCE GetInstance() noexcept; //返回静态实例
	private:
		WindowClass() noexcept; //注册窗口类
		~WindowClass(); //注销窗口类
		WindowClass(const WindowClass&) = delete; 
		WindowClass& operator=(const WindowClass&) = delete;
	private:
		static constexpr const char* wndClassName = "Chili Direct3D Engine Window";
		static WindowClass wndClass; //定义静态对象，需要在类外初始化 cpp开头就初始化了
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const char* name); //noexcept
	~Window();//销毁窗口
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string title); //设置窗口标题
	static std::optional<int> ProcessMessages(); 
	Graphics& Gfx(); //用于返回指向图形对象的指针
	
private: 
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard kbd;  //新加的键盘
	Mouse mouse;
private:
	int width; 
	int height;
	HWND hWnd; //窗口句柄
	std::unique_ptr<Graphics> pGfx; //私有属性 需要提供函数访问 不要直接像键盘和鼠标一样搞个变量， 因为hWnd拿不到 我们要先弄个空指针，然后在窗口默认构造中去传入hWnd
};


#define CHWND_EXCEPT(hr) Window::HrException(__LINE__,__FILE__,hr)
#define CHWND_LAST_EXCEPT() Window::HrException(__LINE__,__FILE__,GetLastError())
#define CHWND_NOGFX_EXCEPT() Window::NoGfxException(__LINE__,__FILE__)
