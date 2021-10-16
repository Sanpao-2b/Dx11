#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include <d3d11.h>
#include <wrl.h>    //COM组件 包含了COMPointer类 比如Comptr指向一个COM对象，并且有引用计数
#include <vector>
#include "DxgiInfoManager.h"
class Graphics
{
public:
	class Exception : public ChiliException
	{
		//继承基类构造函数,编译器不会生成默认构造函数，即无法实例化此类，Exception exp;会报错  但是Exception exp(line , file)；可以
		//目的初始化基类，这样就不用额外书写子类构造函数去初始化基类的构造函数了
		//请看HrException的默认构造函数的实现
		using ChiliException::ChiliException;
	};

	class HrException : public Exception
	{
	public:
		//std::vector<std::string> infoMsgs = {} 用于接收传入构造函数的DxgiInfoManager类抓到的调试信息
		//本应是DxgiInfoManager.h之后加入的， 我copy的时候不小心先搞进来了
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {} ) noexcept;
		const char* what() const noexcept override;  //显示错误消息的哦
		
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;   //用于从DxgiInfoManager类中获取信息的

	private:
		HRESULT hr;
		std::string info;  //用于存放DxgiInfoManager类搞出来的 输出窗口的调试信息 在HrException的构造函数中
	};

	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	
	private:
		std::string reason;
	};
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;
	
	//给图形类添加一些行为
	//前后缓存翻转，这个函数是其他所有函数的前提，如果不翻转后缓存到前缓存，将永远看不到内容
	void EndFrame();

	//创建一个函数去清理目标视图
	/*
		D3D11DeviceContext::ClearRenderTargetView
		将渲染目标中的所有元素设置为一个值。
		void ClearRenderTargetView(
			[in] ID3D11RenderTargetView *pRenderTargetView, //传入渲染目标视图
			[in] const FLOAT[4]        ColorRGBA            //一个4分量数组，表示填充渲染目标的颜色。
		);
	*/
	void ClearBuffer(float red, float green, float blue) noexcept;
	//绘制三角形
	void DrawTestTriangle();
private:
#ifndef NDEBUG    //ifndef 是 if no def  没有定义 不是DEBUG 相当于是DEBUG- - 好你妈拗口
	DxgiInfoManager infoManager;
#endif //仅在调式模式下存在，  release模式不会有
	
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};

/*
	unique_ptr 去指向COM对象，不能自动释放COM对象 还必须手动释放
	并且如果需要pp去接收返回值，由于unique_ptr是封装的，获取不到内部那个指针的地址，所以无法作为pp传入
	从而我们必须用普通指针的pp去接收返回值，然后赋值给unique_ptr 这样很麻烦
	COM对象是可以归多个Comptr去指向的，并且有引用计数，而unique_ptr必须独占
	comptr还有很多好用的函数。
	最重要的是&

*/
