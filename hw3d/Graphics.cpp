#include "Graphics.h"
#include "dxerr.h"
#include <sstream>

#pragma comment(lib,"d3d11.lib")//�ô������ӵ�����⣬������Ŀ���������LINK������Ҫ���һЩ�� ���ƴ��뵽�����Ŀ ���Բ�����������

//����꣬��������׳��쳣�Ĵ�������

//����һ����������麯����ֵHRESULT�Ƿ����ʧ�� �����׳��쳣
#define GFX_THROW_FAILED(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, hr)
//ר�Ŵ����豸���Ƴ����쳣��
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
	sd.OutputWindow = (HWND)6969;  //����Ĵ� ����ֻ��ʾ65�г��� û�и����������
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

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
	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,//���ǸĶ�DEBUGģʽ����,��������û�� ����������ڸ��˺���ϸ�Ĵ�����Ϣ��
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));
	//��ȡ�������ĺ󻺴棬����һ����ָ�����ڱ�����(ָ��󻺴��Ŀ����Ϊ��ͨ�����Texture����һ����ȾĿ����ͼ)
	ID3D11Resource* pBackBuffer = nullptr;

	//��Ϊ�����õ�DXGI_SWAP_EFFECT_DISCARD���Ե�һ������������0��ֻ�ܶ�д���0�Ļ��棻���ڲ�����������"�ӿڵ�uuid����ָ���̨�������ӿڵ�ָ��
	//�����QueryInterface�����ƣ���ʵ���ǻ�ȡ��һ���ӿ�-0-
	GFX_THROW_FAILED(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));

	//�û�ȡ�������Դȥ���� ��ȾĿ����ͼ
	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(
		pBackBuffer, //��������Ҫһ��ID3D11Resource*�Ķ���������ǰ����봴����ô��ָ�룬�����ָ��ָ����Texture(�󻺴�),so����ȾĿ����ͼ���Ǹ������һ��Texture�󶨵ģ�
		nullptr, //û������ȾĿ����ͼ�������ṹ�壬ʹ��Ĭ�Ϸ�ʽ��������
		&pTarget //[out] ��һ��ָ����������ȾĿ����ͼ
	));

	//��ȾĿ����ͼ�Ѿ������ã����Ҵ������ˣ� ָ��󻺴��ָ�벻��Ҫ�� �ͷŵ�
	//���ʣ�Release()��������� ��ָ����Ǹ���Դ�����ô��������û������ָ��ָ���� �����ͱ��ͷ��ˣ��� ����
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
	//��������(1ͬ�������1����60֡,2-30֡��2��ǩ������Ҫ��0����
	//Present�Ƚ����⣬�����ܻ᷵���豸���Ƴ��쳣�����������쳣�������ܶ���Ϣ��������GetDeviceRemovedReason(),
	//�豸�Ƴ�����˵����Կ��γ��ˣ������Կ���������֮���
	if (FAILED(hr = pSwap->Present(1u,0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			//���Present�����׳��쳣������ȷʵ���豸�Ƴ��쳣�����������ĺ����鿴��ϸԭ�򣬲����С��ļ�λ�� HRESULT �����쳣�ಢ�׳��쳣
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
			//�������豸�Ƴ��쳣���������HRESULT�쳣��Ϣ
			GFX_THROW_FAILED(hr);
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	//�����ȾĿ����ͼ��Ҫһ����������
	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget, color);
}


// Graphics exception 

Graphics::HrException::HrException(int line, const char * file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file), //�Ը���Exception���г�ʼ���� �����Exception(line, file)���Ƕ�������Chiliexception�ĳ�ʼ��
	hr(hr)  //��ʼ���Լ��ĳ�Ա����hr
{	
}

const char * Graphics::HrException::what() const noexcept
{
	//std::dec��ʮ���ƣ���std::hex��ʮ�����ƣ���std::oct���˽��ƣ�������ֱ��ʹ��
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
	return DXGetErrorString(hr); //֮ǰ�����쳣�����Լ���ƺ���ʵ�ֵİѴ�����뷭����ַ����� ����DX�ṩ����Ӧ����
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));  //����Ҳ�� DX�ṩ���쳣����ĺ���
	return buf;
}

const char * Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
