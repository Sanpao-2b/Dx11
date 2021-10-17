#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
namespace wrl = Microsoft::WRL;

#pragma comment(lib,"d3d11.lib")//用代码链接到这个库，比在项目属性里面的LINK中设置要灵活一些， 复制代码到别的项目 可以不用重新设置
#pragma comment(lib,"D3DCompiler.lib")//用它可以在运行时编译着色器，但是现在我只需要用它的着色器加载函数即可

//定义宏，让下面的抛出异常的代码更简洁
//hrcall: 包裹一个函数，检查函数返值HRESULT是否代表失败 是则抛出异常
//    hr: 直接传入俺们定义的 hr变量
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, hr)

#ifndef NDEBUG //如果是调试模式 采用这三个宏 调试模式比发布模式多出了由infoManager类抓到的额外的输出窗口的详细错误信息
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__, __FILE__, (hr), infoManager.GetMessages())  //接收错误代码，传给Exception基类的构造函数，并且传入由infoManager类抓到的输出窗口的信息
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)	//注意这里的意思是：包裹D3D函数前 先调用Set() 之后就只会获取到有关最近调用的这个函数的信息
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), infoManager.GetMessages()) //这里也同上，额外传入刚刚创建好的 InfoManager类抓出来的信息
//也是借用infoManager类中的抓窗口的方法，抓的函数不同 这里抓那些不返回HRESULT 的函数
//流程宏接收整个函数作为输入值， 直接替换成后面这一大串：调用函数前先set()记录输出窗口的当前信息数->调用传入的函数->用v获取当前输出窗口新产生的消息->if判断如果新产生了消息则说明出错->抛出异常
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException(__LINE__, __FILE__, v);}}
#else          //否则就是发布模式，采用这三个宏
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif
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
}

void Graphics::DrawTestTriangle()
{
	HRESULT hr;

	//1.Input Assembler输入装配器阶段：指定三角形定点位置，输入顶点，装配成三角形输出给Vertex shader
	//pContext->可以提供一大堆函数，看前缀可以知道对应渲染管线的什么阶段。 比如IA开头的都是输入装配器阶段
	namespace wrl = Microsoft::WRL;
	
	// CreateBuffer()传入创建一个指针当pp
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;

	// CreateBuffer()需要传入子资源数据 即缓冲区具体存放的什么玩意儿。
	struct Vertex
	{
		float x;
		float y;
	};

	// 存放3个点,注意必须顺时针，D3D用顺时针和逆时针区分正反面，反面是不显示的
	const Vertex vertices[] =
	{
		{ 0.0f, 0.5f },
		{ 0.5f, -0.5f },
		{ -0.5f, -0.5f },
	};
	/*
	typedef struct D3D11_SUBRESOURCE_DATA
	{
	const void *pSysMem;        //指向初始化数据
	UINT       SysMemPitch;		//只跟纹理有关 别的类型不用管
	UINT       SysMemSlicePitch;//只跟纹理有关 不管
	} D3D11_SUBRESOURCE_DATA;
	*/
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;

	// CreateBuffer()需要缓冲区数组描述符
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(vertices);			//UINT			缓冲区的大小(以字节为单位)
	bd.Usage = D3D11_USAGE_DEFAULT;				//D3D11_USAGE	确定预期如何读取和写入缓冲区,通常是 D3D11_USAGE_DEFAULT GPU可读写
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//UINT			表示这缓存将要成为什么样的缓存，顶点缓存、索引缓存、常量缓存等等 
	bd.CPUAccessFlags = 0u;						//UINT			CPU访问标志  是否允许CPU访问 0的不许
	bd.MiscFlags = 0u;							//UINT			杂项标志 或 0(如果未使用)。
	bd.StructureByteStride = sizeof(Vertex);	//UNIT			每个顶点的大小

	// 因为IASetVertexBuffers() 需要传入ID3D11Buffer数组 所以创建
	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	

	// 将顶点缓冲区数组绑定到渲染管线的 输入装配器阶段
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	/*void IASetVertexBuffers(
		[in]           UINT         StartSlot,    用于绑定的第一个输入槽，数组额外的缓冲区会被隐式绑定到后面的输入草
		[in]           UINT         NumBuffers,	  数组中的顶点缓存的数量
		[in, optional] ID3D11Buffer * const *ppVertexBuffers, 指向顶点缓冲区数组的指针
		[in, optional] const UINT   *pStrides,	    步幅值，就是一个顶点数据的大小，单位是字节byte
		[in, optional] const UINT   *pOffsets		偏移量，！注意顶点缓存不只是装顶点数据 还有颜色、法线，这里我们只有顶点数据则偏移0
	);*/
	pContext->IASetVertexBuffers(
		0,1u,
		pVertexBuffer.GetAddressOf(), //这里出错了 他需要一个pp 但不是用于填充，它内部是取出pp指向的对象，所以不能直接取地址 这样会释放掉被指向的缓存
		&stride,		//步幅 每组数据的大小
		&offset			//偏移量某种数据在一组数据里的起始位置，每个数组元素目前只有一种数据：顶点 所以不用偏移
		);

	// ————创建顶点着色器
	wrl::ComPtr<ID3D11VertexShader> pVertexShader; //老套路 传它的pp填充
	wrl::ComPtr<ID3DBlob> pBlob;
	// 这个函数可以读取任何二进制文件，注意这个函数只能用wild string 所以前面价格L 进行转换
	// !还有个大问题，这个函数加载文件的目录跟HLSL编译后输出的目录是不同的，懒得输入一大串目录，所以要改HLSL文件编译后的 执行文件输出目录,
	// 把OutDir 改成ProjectDir即不要输出到外目录，输出到项目目录
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	// 形参：1.着色器的二进制文件的指针(void*类型的 不能用&pBlob 前面说过 会释放掉指向的内存) 2.二进制文件长度(读到哪里结束) 3.暂时不用以后再说 4.pp
	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	// ————绑定顶点着色器 
	/* 再次提醒，不要传入直接传入智能指针，要调用Get()才能获取到正确的 内部维护的那个指针*/
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

	// ————Input(vertex) Layout(2d position only)
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	// 描述符,下面这是描述符数组，即每个元素都是一个InputLayout的描述符 要看有啥成员要接受输入值可以自己创建一个变量 用.挨个赋值也行
	/*
		typedef struct D3D11_INPUT_ELEMENT_DESC {
		  LPCSTR                     SemanticName;			//**语义名，必须跟VertexShader中形参:后的名称相同
		  UINT                       SemanticIndex;			//**索引号，因为shader中可能有很多语义参数，所以会有0 1 2....索引
		  DXGI_FORMAT                Format;				//**重要，告诉我们输入的数据是什么，如下面宏即 2个float元素 R32G32可以不指颜色，可以指点。看MSDN DXGI_FORMAT
		  UINT                       InputSlot;				//总是0
		  UINT                       AlignedByteOffset;		//某种数据的第一个字节位 相比于那组数据的第一个字节位的偏移量
		  D3D11_INPUT_CLASSIFICATION InputSlotClass;        //后面这俩是用于渲染实例对象，即一组数据渲染很多个实例 看MSDN D3D11_INPUT_CLASSIFICATION
		  UINT                       InstanceDataStepRate;
		} D3D11_INPUT_ELEMENT_DESC;
	*/
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	//创建输入布局函数 见msdn 
	/*
		HRESULT CreateInputLayout(
		  [in]  const D3D11_INPUT_ELEMENT_DESC  *pInputElementDescs,  //一组数据描述符
		  [in]  UINT               NumElements,						  //输入的数组中描述符的数量
		  [in]  const void         *pShaderBytecodeWithInputSignature,//指向已编译过的shader的指针，指向字节码 我们已经绑定了shader啊？ 这里作用是检查描述符和着色器是不是匹配的
		  [in]  SIZE_T             BytecodeLength,					  //字节码长度
		  [out] ID3D11InputLayout  **ppInputLayout					  //pp，填充我们滴InputLayout对象
		);
	*/
	GFX_THROW_INFO(pDevice->CreateInputLayout(
		ied, (UINT)std::size(ied),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pInputLayout
	));

	// ————创建像素着色器
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob)); //重用pBlob因为原来指向的空间不需要了 可以释放掉 没问题
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
	// ————绑定像素着色器
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

	// ————绑定渲染目标 Render Target 
	/*如果不绑定这个东西 那D3D不知道像素着色器应该输出到哪里去 OM Oput Merge 阶段
	 加了s 复数 则肯定需要一个数组的指针了 长得像pp,我们已经创建过 渲染目标了pTarget 当做只有一个元素的数组即可,
		！！！！注意不能&pTarget,这样会释放掉指向的对象,用GetAddressOf()来获取内部那个指针的地址 pp。
	 没用到深度缓存 nullptr即可*/
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);
	
	// ————Primitive Topology 原始拓扑
	/* 描述IA阶段传入的点之间的拓扑关系，即确定图元  
	 比如输入0 1 2 3 4 5 点。线：01 23 45三条线。 三角形：012 345。 三角形带:012 123 234 345 */
	pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	// ————配置视口 Viewport
	/* 因为把buffer上的东西映射到屏幕可以有很多方法，比如映射过去只占屏幕的左上角一块区域， 这样屏幕的其他区域可以用别的buffer去映射，从而实现分屏 或者是映射HUD
	 不需要绑定到渲染管线，只需要创建一个结构体，然后用函数指定它即可*/
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0; //左上角的位置
	vp.TopLeftY = 0;
	// RS——Rasterize Stage  注意有s 说明需要一个数组，我们用取地址指向这个地址，就像是一个只有一个元素的数组
	pContext->RSSetViewports(1u, &vp);


	//绘制三角形的函数,只有在真正渲染命令时才会有输出错误的信息，所以包裹这个即可
	GFX_THROW_INFO_ONLY(pContext->Draw((UINT)sizeof(vertices), 0u));//形参：1.顶点数量 2.起始位置
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
