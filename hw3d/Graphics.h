#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include <d3d11.h>
#include <wrl.h>    //COM组件 包含了COMPointer类 比如Comptr指向一个COM对象，并且有引用计数
#include <vector>
#include "DxgiInfoManager.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>

class Graphics
{
	// ———————————————————— 基类是友元，他的儿子们并不会自动成为友元！————————————————————
	// 那他儿子们想访问这个类中的私有成员怎么办？
	// 友元Bindable可以访问Graphics，那么我们在Bindable中提供函数返回这里的成员属性指针，而子类们可以访问Bindable的函数得到这些指针
	friend class Bindable;
public:
	class Exception : public ChiliException
	{
		//继承基类构造函数,编译器不会生成默认构造函数，即无法实例化此类，Exception exp;会报错  但是Exception exp(line , file)；可以
		//目的初始化基类，这样就不用额外书写子类构造函数去初始化基类的构造函数了
		//请看HrException的默认构造函数的实现
		using ChiliException::ChiliException;
	};
	//抓HRESULT
	class HrException : public Exception
	{
	public:
		//std::vector<std::string> infoMsgs = {} 用于接收传入构造函数的DxgiInfoManager类抓到的调试信息
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {} ) noexcept;
		const char* what() const noexcept override;  //显示错误消息的哦
		
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;   //用于从DxgiInfoManager类中获取信息的

	private:
		HRESULT hr;     //存储HRESULT 因为是用这个组件去设计宏包裹那些返回HRESULT的函数 
		std::string info;  //用于存放DxgiInfoManager类搞出来的 输出窗口的调试信息 在HrException的构造函数中
	};
	//返回值为void 那就抓输出窗的 Infomation
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;   //用于那些没有HRESULT返回值的函数,用info存放输出窗口抓到的信息
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
	
	//前后缓存翻转，这个函数是其他所有函数的前提，如果不翻转后缓存到前缓存，将永远看不到内容
	void EndFrame();
	//创建一个函数去清理目标视图
	void ClearBuffer(float red, float green, float blue) noexcept;
	void DrawIndexed(UINT count);
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;

private:
	DirectX::XMMATRIX projection;
#ifndef NDEBUG   
	DxgiInfoManager infoManager;
#endif 
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};

