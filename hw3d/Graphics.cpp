#include "Graphics.h"

//�ô������ӵ�����⣬������Ŀ���������LINK������Ҫ���һЩ�� ���ƴ��뵽�����Ŀ ���Բ�����������
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
	if (pDevice != nullptr)
	{
		pDevice->Release();
	}
}

void Graphics::EndFrame()
{
	//ͬ�������֡�� 1-60֡�� 2-30֡ ��ǩ��0
	pSwap->Present(1u, 0u);
}
