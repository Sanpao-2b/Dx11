#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#include "GraphicsThrowMacros.h"
namespace wrl = Microsoft::WRL;

#pragma comment(lib,"d3d11.lib")//用代码链接到这个库，比在项目属性里面的LINK中设置要灵活一些， 复制代码到别的项目 可以不用重新设置
#pragma comment(lib,"D3DCompiler.lib")//用它可以在运行时编译着色器，但是现在我只需要用它的着色器加载函数即可


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
	sd.OutputWindow = hWnd;  
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG  //或等于 用于自动控制传入"创建设备和交换链"函数中的第四个参数，如果调试模式下则传入0x2, 如果发布模式则是0 
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,//如果是DEBUG模式，则输出窗口会产生错误的详尽描述，用Dxgiinfomanager类抓出来 放到弹窗上
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,       
		&pDevice,     
		nullptr,      
		&pContext   
		/*
			pSwap,pDevice这几个都是智能指针了 看起来&是取的智能指针的地址啊 我们需要的是它内部维护的真正的指针的地址去填充这个指针，
			其实是因为Comptr重载了&运算符，可以正确返回内部维护的那个普通指针的地址 正确的pp。但是！Comptr如果取地址 他首先会释放自己指向的那个空间，然后再返回空间的地址，
			很合理吧 因为我想填充，所以先释放掉原来存的那块内存的东西，这样不会造成内存泄漏啊，之后就无法使用那块内存的东西了。
			我们想填充，这里这样用&是没错的，因为Comptr还没指向任何有用的东西,释放了个寂寞。但有时候你不想填充指针，只是想单纯的获取我维护的指针的真的地址，&一用 直接释放了。。。
			真要获取维护的那个指针的地址请使用pBackBuffer->GetAddressOf()，&pSwap的效果其实就是pSwap->ReleaseAndGetAddress()一样 获取并且释放
		*/
	));
	//获取交换链的后缓存，创建一个智能指针用于保存它(创建这个指针的目的：通过指向一个Texture对象创建一个渲染目标视图)
	//智能指针其实是个模板类，重载了->运算符，使他和普通指针一样的用法，但是如果要获得这个指针的地址即pp 要用Get()函数
	wrl::ComPtr<ID3D11Resource> pBackBuffer;

	//因为我们用的DXGI_SWAP_EFFECT_DISCARD所以第一个参数必须是0即只能读写编号0的缓存；用于操纵这个缓存的"接口的uuid“；指向后台缓冲区接口的指针
	//这里跟QueryInterface很类似，其实就是获取了一个接口-0-
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));

	//用获取的这个资源去创建 渲染目标视图
	//如果此函数 抛出异常了，程序就终止了 pBackBuffer指向的内存，就不能正常释放了，我们需要把指针包裹在一个对象里，当该对象超出范围时，会自动释放这个指针
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),//获取指向的那个对象的地址，不能用pBackBuffer->Get()，因为被指向的Texture没有Get()，pBackBuffer本质是对象，必须用"."，调用自己的函数
		nullptr, //没传入渲染目标视图的描述结构体，使用默认方式创建即可
		&pTarget //[out] 用一个指针变量填充渲染目标视图
	));

	// create depth stensil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;
	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));

	// bind depth state
	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	// create depth stensil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = 800u;
	descDepth.Height = 600u;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	// create view of depth stensil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(pDevice->CreateDepthStencilView(
		pDepthStencil.Get(), &descDSV, &pDSV
	));

	// bind depth stensil view to OM
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800.0f;
	vp.Height = 600.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);
}


void Graphics::EndFrame()
{

	HRESULT hr;
#ifndef NDEBUG //调试模式下 把这个Set()函数放在这个位置
	infoManager.Set();
#endif
	//参数解释(1同步间隔：1代表60帧,2-30帧。2标签：不需要则0。。
	//Present比较特殊，他可能会返回设备已移除异常，这是特殊异常，包含很多信息，必须用GetDeviceRemovedReason(),
	//设备移除不是说你把显卡拔出了，而是显卡驱动崩溃之类的
	if (FAILED(hr = pSwap->Present(1u,0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			//如果Present函数抛出异常，并且确实是设备移除异常，则调用下面宏，弹窗显示原因
			//所有的图形类的异常宏都会分调试/发布模式了哦
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
			//否则不是设备移除异常，正常输出HRESULT异常信息
			throw GFX_EXCEPT(hr);
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	//清除渲染目标视图需要一个浮点数组
	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);

}

void Graphics::DrawIndexed(UINT count) noexcept(!IS_DEBUG)
{
	//StartIndexLocation：表示从第几个索引开始画
	//BaseVertexLocation：画的时候，每一个索引都加这个数字
	//要先把索引缓存绑定到渲染管线
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}


// Graphics exception 

Graphics::HrException::HrException(int line, const char * file, HRESULT hr,  std::vector<std::string> infoMsgs  ) noexcept
	:
	Exception(line, file), //对父类Exception进行初始化， 父类的Exception(line, file)又是对他父类Chiliexception的初始化
	hr(hr)  //初始化自己的成员变量hr
{	
	//每个消息都占一行; for循环C++11 新特性，遍历容器每个元素
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// 如果info是有信息的，那么必然最后一行是空的 把这一行移除掉
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char * Graphics::HrException::what() const noexcept
{
	//std::dec（十进制）、std::hex（十六进制）、std::oct（八进制）都可以直接使用
	std::stringstream oss;
	oss << GetType() << std::endl
		<< "[Error code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << "(" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String]" << GetErrorString() << std::endl
		<< "[Error Description]" << GetErrorDescription() << std::endl;
	//如果info中有消息就把他显示出来，没有就算了
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
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

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char * Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char * file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// 新特性 遍历容器每个元素即字符串，添加到成员变量info中，每个元素独占一行
	for ( const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// 如果最后一行是空的则清除
	if (!info.empty())
	{
		info.pop_back(); 
	}
}

const char * Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char * Graphics::InfoException::GetType() const noexcept
{
	return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
