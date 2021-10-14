#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include <d3d11.h>
#include <vector>

class Graphics
{
public:
	class Exception : public ChiliException
	{
		//继承基类构造函数,编译器不会生成默认构造函数，即Exception exp;报错 但是Exception exp(line , file)；可以
		//目的初始化基类，这样就不用额外书写子类构造函数去初始化基类的构造函数了
		//请看HrException的默认构造函数的实现
		using ChiliException::ChiliException;
	};

	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
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
	~Graphics();
	
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
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr; //早就创建好了- - 
};
