#pragma once
#include "ChiliWin.h"//保证这个头文件在最顶端，因为如果包含其他头文件在这个的上面，那我们在ChiliWin.h包含的那些nonono就会失效
#include "ChiliException.h"
#include "Keyboard.h"
#include "Mouse.h"

//c++17 可以选择返回一个int还是一个空的optional 配合消息处理很好 optional<T> 相当于T类型变量和一个bool值的打包，其中的bool用来表示T变量是否为空
#include <optional> 
class Window
{
	//异常处理成员类
public:
	class Exception : public ChiliException
	{
	public:    
		Exception(int line, const char* file, HRESULT hr) noexcept;//HRESULT窗口程序专属的错误类
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;//把hr错误代码 转换成人能看的字符串。。。
		HRESULT GetErrorCode() const noexcept;                     //返回成员变量hr
		std::string GetErrorString() const noexcept;               //调用TranslateErrorCode() 并返回
	private:
		HRESULT hr;
	};

private:
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
		static WindowClass wndClass; //运行前就创建了
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const char* name); //noexcept
	~Window();//销毁窗口
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string title); //设置窗口标题

	//————————————————————————————————————————————————————————————————————————//
	//从App类中的Go()函数中 搬到这里来了 
	//静态 因为这个函数要处理所有窗口的消息，the one
	static std::optional<int> ProcessMessages(); 
	//————————————————————————————————————————————————————————————————————————//
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
};


#define CHWND_EXCEPT(hr) Window::Exception(__LINE__,__FILE__,hr)
#define CHWND_LAST_EXCEPT() Window::Exception(__LINE__,__FILE__,GetLastError())
