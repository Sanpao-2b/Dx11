#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#include "GraphicsThrowMacros.h"
namespace wrl = Microsoft::WRL;

#pragma comment(lib,"d3d11.lib")//�ô������ӵ�����⣬������Ŀ���������LINK������Ҫ���һЩ�� ���ƴ��뵽�����Ŀ ���Բ�����������
#pragma comment(lib,"D3DCompiler.lib")//��������������ʱ������ɫ��������������ֻ��Ҫ��������ɫ�����غ�������


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
#ifndef NDEBUG  //����� �����Զ����ƴ���"�����豸�ͽ�����"�����еĵ��ĸ��������������ģʽ������0x2, �������ģʽ����0 
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//���ڼ��d3d��������ֵ,��Ϊ����������hrȥ���շ���ֵ�ģ����Ա���Ҫ��һ��
	HRESULT hr;
	// create device and front/back buffers, and swap chain and rendering context
/*
	HRESULT D3D11CreateDeviceAndSwapChain(
		[in, optional]  IDXGIAdapter               *pAdapter,      �����豸ʱҪʹ�õ���Ƶ��������ָ��,NULLʹ��Ĭ��������
						D3D_DRIVER_TYPE            DriverType,     ������/Ӳ������������������
						HMODULE                    Software,	   �������ѡ�������Ⱦ�������ﴫ�����դ�����ľ��
						UINT                       Flags,          ��ǩ����Ҫ
		[in, optional]  const D3D_FEATURE_LEVEL    *pFeatureLevels,ָ��D3D_FEATURE_LEVEL�����ָ�룬��������Ҫ���Դ�����Ҫ�ؼ����˳��ΪNULL����ʹ��Ĭ�����飺
						UINT                       FeatureLevels,  �����Ǹ������е�Ԫ��������
						UINT                       SDKVersion,     SDK�汾
		[in, optional]  const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,�������������ṹ��
		[out, optional] IDXGISwapChain             **ppSwapChain,  
		[out, optional] ID3D11Device               **ppDevice,
		[out, optional] D3D_FEATURE_LEVEL          *pFeatureLevel, ����һ��ָ��D3D_FEATURE_LEVEL��ָ�룬�����еĵ�һ��Ԫ�ء���NULL������㲢����Ҫȷ����Щ���ܼ���֧�ֵ����롣
		[out, optional] ID3D11DeviceContext        **ppImmediateContext
);
*/
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,//�����DEBUGģʽ����������ڻ����������꾡��������Dxgiinfomanager��ץ���� �ŵ�������
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,       
		&pDevice,     
		nullptr,      
		&pContext   
		/*
			pSwap,pDevice�⼸����������ָ���� ������&��ȡ������ָ��ĵ�ַ�� ������Ҫ�������ڲ�ά����������ָ��ĵ�ַȥ������ָ�룬
			��ʵ����ΪComptr������&�������������ȷ�����ڲ�ά�����Ǹ���ָͨ��ĵ�ַ ��ȷ��pp�����ǣ�Comptr���ȡ��ַ �����Ȼ��ͷ��Լ�ָ����Ǹ��ռ䣬Ȼ���ٷ��ؿռ�ĵ�ַ��
			�ܺ���� ��Ϊ������䣬�������ͷŵ�ԭ������ǿ��ڴ�Ķ�����������������ڴ�й©����֮����޷�ʹ���ǿ��ڴ�Ķ����ˡ�
			��������䣬����������&��û��ģ���ΪComptr��ûָ���κ����õĶ���,�ͷ��˸���į������ʱ���㲻�����ָ�룬ֻ���뵥���Ļ�ȡ��ά����ָ�����ĵ�ַ��&һ�� ֱ���ͷ��ˡ�����
			��Ҫ��ȡά�����Ǹ�ָ��ĵ�ַ��ʹ��pBackBuffer->GetAddressOf()��&pSwap��Ч����ʵ����pSwap->ReleaseAndGetAddress()һ�� ��ȡ�����ͷ�
		*/
	));
	//��ȡ�������ĺ󻺴棬����һ������ָ�����ڱ�����(�������ָ���Ŀ�ģ�ͨ��ָ��һ��Texture���󴴽�һ����ȾĿ����ͼ)
	//����ָ����ʵ�Ǹ�ģ���࣬������->�������ʹ������ָͨ��һ�����÷����������Ҫ������ָ��ĵ�ַ��pp Ҫ��Get()����
	wrl::ComPtr<ID3D11Resource> pBackBuffer;

	//��Ϊ�����õ�DXGI_SWAP_EFFECT_DISCARD���Ե�һ������������0��ֻ�ܶ�д���0�Ļ��棻���ڲ�����������"�ӿڵ�uuid����ָ���̨�������ӿڵ�ָ��
	//�����QueryInterface�����ƣ���ʵ���ǻ�ȡ��һ���ӿ�-0-
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));

	//�û�ȡ�������Դȥ���� ��ȾĿ����ͼ
	//����˺��� �׳��쳣�ˣ��������ֹ�� pBackBufferָ����ڴ棬�Ͳ��������ͷ��ˣ�������Ҫ��ָ�������һ����������ö��󳬳���Χʱ�����Զ��ͷ����ָ��
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),//��ȡָ����Ǹ�����ĵ�ַ��������pBackBuffer->Get()����Ϊ��ָ���Textureû��Get()��pBackBuffer�����Ƕ��󣬱�����"."�������Լ��ĺ���
		nullptr, //û������ȾĿ����ͼ�������ṹ�壬ʹ��Ĭ�Ϸ�ʽ��������
		&pTarget //[out] ��һ��ָ����������ȾĿ����ͼ
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
#ifndef NDEBUG //����ģʽ�� �����Set()�����������λ��
	infoManager.Set();
#endif
	//��������(1ͬ�������1����60֡,2-30֡��2��ǩ������Ҫ��0����
	//Present�Ƚ����⣬�����ܻ᷵���豸���Ƴ��쳣�����������쳣�������ܶ���Ϣ��������GetDeviceRemovedReason(),
	//�豸�Ƴ�����˵����Կ��γ��ˣ������Կ���������֮���
	if (FAILED(hr = pSwap->Present(1u,0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			//���Present�����׳��쳣������ȷʵ���豸�Ƴ��쳣�����������꣬������ʾԭ��
			//���е�ͼ������쳣�궼��ֵ���/����ģʽ��Ŷ
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
			//�������豸�Ƴ��쳣���������HRESULT�쳣��Ϣ
			throw GFX_EXCEPT(hr);
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	//�����ȾĿ����ͼ��Ҫһ����������
	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);

}

void Graphics::DrawIndexed(UINT count) noexcept(!IS_DEBUG)
{
	//StartIndexLocation����ʾ�ӵڼ���������ʼ��
	//BaseVertexLocation������ʱ��ÿһ�����������������
	//Ҫ�Ȱ���������󶨵���Ⱦ����
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
	Exception(line, file), //�Ը���Exception���г�ʼ���� �����Exception(line, file)���Ƕ�������Chiliexception�ĳ�ʼ��
	hr(hr)  //��ʼ���Լ��ĳ�Ա����hr
{	
	//ÿ����Ϣ��ռһ��; forѭ��C++11 �����ԣ���������ÿ��Ԫ��
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// ���info������Ϣ�ģ���ô��Ȼ���һ���ǿյ� ����һ���Ƴ���
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char * Graphics::HrException::what() const noexcept
{
	//std::dec��ʮ���ƣ���std::hex��ʮ�����ƣ���std::oct���˽��ƣ�������ֱ��ʹ��
	std::stringstream oss;
	oss << GetType() << std::endl
		<< "[Error code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << "(" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String]" << GetErrorString() << std::endl
		<< "[Error Description]" << GetErrorDescription() << std::endl;
	//���info������Ϣ�Ͱ�����ʾ������û�о�����
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
	return DXGetErrorString(hr); //֮ǰ�����쳣�����Լ���ƺ���ʵ�ֵİѴ�����뷭����ַ����� ����DX�ṩ����Ӧ����
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));  //����Ҳ�� DX�ṩ���쳣����ĺ���
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
	// ������ ��������ÿ��Ԫ�ؼ��ַ�������ӵ���Ա����info�У�ÿ��Ԫ�ض�ռһ��
	for ( const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// ������һ���ǿյ������
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
