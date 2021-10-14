#include "Graphics.h"
#include "dxerr.h"
#include <sstream>

#pragma comment(lib,"d3d11.lib")//用代码链接到这个库，比在项目属性里面的LINK中设置要灵活一些， 复制代码到别的项目 可以不用重新设置

//定义宏，让下面的抛出异常的代码更简洁

//包裹一个函数，检查函数返值HRESULT是否代表失败 是则抛出异常
#define GFX_THROW_FAILED(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, hr)
//专门处理设备被移除的异常，
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, hr)

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {}; //Swapchain Description
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = (HWND)6969;  //故意改错 报错只显示65行出错 没有更具体的描述
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	//用于检查d3d函数返回值,因为宏里面是用hr去接收返回值的，所以必须要有一个
	HRESULT hr;
	// create device and front/back buffers, and swap chain and rendering context
/*
	HRESULT D3D11CreateDeviceAndSwapChain(
		[in, optional]  IDXGIAdapter               *pAdapter,      创建设备时要使用的视频适配器的指针,NULL使用默认适配器
						D3D_DRIVER_TYPE            DriverType,     传入软/硬驱动程序类型来创建
						HMODULE                    Software,	   如果上面选择软件渲染，则这里传入软光栅化器的句柄
						UINT                       Flags,          标签不需要
		[in, optional]  const D3D_FEATURE_LEVEL    *pFeatureLevels,指向D3D_FEATURE_LEVEL数组的指针，它决定了要尝试创建的要素级别的顺序。为NULL，则使用默认数组：
						UINT                       FeatureLevels,  上面那个数组中的元素数量。
						UINT                       SDKVersion,     SDK版本
		[in, optional]  const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,交换链的描述结构体
		[out, optional] IDXGISwapChain             **ppSwapChain,  
		[out, optional] ID3D11Device               **ppDevice,
		[out, optional] D3D_FEATURE_LEVEL          *pFeatureLevel, 返回一个指向D3D_FEATURE_LEVEL的指针，数组中的第一个元素。若NULL，如果你并不需要确定哪些功能级别支持的输入。
		[out, optional] ID3D11DeviceContext        **ppImmediateContext
);
*/
	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,//我们改动DEBUG模式试试,弹出窗口没变 但是输出窗口给了很详细的错误信息，
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));
	//获取交换链的后缓存，创建一个空指针用于保存它(指向后缓存的目的是为了通过这个Texture创建一个渲染目标视图)
	ID3D11Resource* pBackBuffer = nullptr;

	//因为我们用的DXGI_SWAP_EFFECT_DISCARD所以第一个参数必须是0即只能读写编号0的缓存；用于操纵这个缓存的"接口的uuid“；指向后台缓冲区接口的指针
	//这里跟QueryInterface很类似，其实就是获取了一个接口-0-
	GFX_THROW_FAILED(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));

	//用获取的这个资源去创建 渲染目标视图
	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(
		pBackBuffer, //很明显需要一个ID3D11Resource*的东西，所以前面必须创建这么个指针，而这个指针指向了Texture(后缓存),so“渲染目标视图”是跟具体的一个Texture绑定的？
		nullptr, //没传入渲染目标视图的描述结构体，使用默认方式创建即可
		&pTarget //[out] 用一个指针变量填充渲染目标视图
	));

	//渲染目标视图已经创建好，并且存起来了， 指向后缓存的指针不需要了 释放掉
	//疑问，Release()方法会减少 被指向的那个资源的引用次数，如果没有其他指针指向它 则它就被释放了？？ 不懂
	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	if (pContext != nullptr)
	{
		pContext->Release();
	}
	if (pSwap != nullptr)
	{
		pSwap->Release();
	}
	if (pTarget != nullptr)
	{
		pTarget->Release();
	}
	if (pDevice != nullptr)
	{
		pDevice->Release();
	}
}

void Graphics::EndFrame()
{

	HRESULT hr;
	//参数解释(1同步间隔：1代表60帧,2-30帧。2标签：不需要则0。。
	//Present比较特殊，他可能会返回设备已移除异常，这是特殊异常，包含很多信息，必须用GetDeviceRemovedReason(),
	//设备移除不是说你把显卡拔出了，而是显卡驱动崩溃之类的
	if (FAILED(hr = pSwap->Present(1u,0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			//如果Present函数抛出异常，并且确实是设备移除异常，则调用下面的函数查看详细原因，并把行、文件位置 HRESULT 传入异常类并抛出异常
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
			//否则不是设备移除异常，正常输出HRESULT异常信息
			GFX_THROW_FAILED(hr);
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	//清除渲染目标视图需要一个浮点数组
	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget, color);
}


// Graphics exception 

Graphics::HrException::HrException(int line, const char * file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file), //对父类Exception进行初始化， 父类的Exception(line, file)又是对他父类Chiliexception的初始化
	hr(hr)  //初始化自己的成员变量hr
{	
}

const char * Graphics::HrException::what() const noexcept
{
	//std::dec（十进制）、std::hex（十六进制）、std::oct（八进制）都可以直接使用
	std::stringstream oss;
	oss << GetType() << std::endl
		<< "[Error code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << "(" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String]" << GetErrorString() << std::endl
		<< "[Error Description]" << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char * Graphics::HrException::GetType() const noexcept
{
	return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr); //之前窗口异常类中自己设计函数实现的把错误代码翻译成字符串， 这里DX提供了相应函数
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));  //这里也是 DX提供了异常处理的函数
	return buf;
}

const char * Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
