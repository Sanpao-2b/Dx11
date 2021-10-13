#include "Graphics.h"

//用代码链接到这个库，比在项目属性里面的LINK中设置要灵活一些， 复制代码到别的项目 可以不用重新设置
#pragma comment(lib,"d3d11.lib")

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
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
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

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
	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	);
	//获取交换链的后缓存，创建一个空指针用于保存它(指向后缓存的目的是为了通过这个Texture创建一个渲染目标视图)
	ID3D11Resource* pBackBuffer = nullptr;

	//因为我们用的DXGI_SWAP_EFFECT_DISCARD所以第一个参数必须是0即只能读写编号0的缓存；用于操纵这个缓存的"接口的uuid“；指向后台缓冲区接口的指针
	//这里跟QueryInterface很类似，其实就是获取了一个接口-0-
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));

	//用获取的这个资源去创建 渲染目标视图
	pDevice->CreateRenderTargetView(
		pBackBuffer, //很明显需要一个ID3D11Resource*的东西，所以前面必须创建这么个指针，而这个指针指向了Texture(后缓存),so“渲染目标视图”是跟具体的一个Texture绑定的？
		nullptr, //没传入渲染目标视图的描述结构体，使用默认方式创建即可
		&pTarget //[out] 用一个指针变量填充渲染目标视图
	);

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
	//同步间隔：帧数 1-60帧， 2-30帧 标签：0
	pSwap->Present(1u, 0u);
}
