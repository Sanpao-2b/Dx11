#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
namespace wrl = Microsoft::WRL;

#pragma comment(lib,"d3d11.lib")//�ô������ӵ�����⣬������Ŀ���������LINK������Ҫ���һЩ�� ���ƴ��뵽�����Ŀ ���Բ�����������
#pragma comment(lib,"D3DCompiler.lib")//��������������ʱ������ɫ��������������ֻ��Ҫ��������ɫ�����غ�������

//����꣬��������׳��쳣�Ĵ�������
//hrcall: ����һ����������麯����ֵHRESULT�Ƿ����ʧ�� �����׳��쳣
//    hr: ֱ�Ӵ��밳�Ƕ���� hr����
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, hr)

#ifndef NDEBUG //����ǵ���ģʽ ������������ ����ģʽ�ȷ���ģʽ�������infoManager��ץ���Ķ����������ڵ���ϸ������Ϣ
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__, __FILE__, (hr), infoManager.GetMessages())  //���մ�����룬����Exception����Ĺ��캯�������Ҵ�����infoManager��ץ����������ڵ���Ϣ
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)	//ע���������˼�ǣ�����D3D����ǰ �ȵ���Set() ֮���ֻ���ȡ���й�������õ������������Ϣ
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), infoManager.GetMessages()) //����Ҳͬ�ϣ����⴫��ոմ����õ� InfoManager��ץ��������Ϣ
//Ҳ�ǽ���infoManager���е�ץ���ڵķ�����ץ�ĺ�����ͬ ����ץ��Щ������HRESULT �ĺ���
//���̺��������������Ϊ����ֵ�� ֱ���滻�ɺ�����һ�󴮣����ú���ǰ��set()��¼������ڵĵ�ǰ��Ϣ��->���ô���ĺ���->��v��ȡ��ǰ��������²�������Ϣ->if�ж�����²�������Ϣ��˵������->�׳��쳣
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException(__LINE__, __FILE__, v);}}
#else          //������Ƿ���ģʽ��������������
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
}

void Graphics::DrawTestTriangle()
{
	HRESULT hr;

	//1.Input Assembler����װ�����׶Σ�ָ�������ζ���λ�ã����붥�㣬װ��������������Vertex shader
	//pContext->�����ṩһ��Ѻ�������ǰ׺����֪����Ӧ��Ⱦ���ߵ�ʲô�׶Ρ� ����IA��ͷ�Ķ�������װ�����׶�
	namespace wrl = Microsoft::WRL;
	
	// CreateBuffer()���봴��һ��ָ�뵱pp
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;

	// CreateBuffer()��Ҫ��������Դ���� �������������ŵ�ʲô�������
	struct Vertex
	{
		float x;
		float y;
	};

	// ���3����,ע�����˳ʱ�룬D3D��˳ʱ�����ʱ�����������棬�����ǲ���ʾ��
	const Vertex vertices[] =
	{
		{ 0.0f, 0.5f },
		{ 0.5f, -0.5f },
		{ -0.5f, -0.5f },
	};
	/*
	typedef struct D3D11_SUBRESOURCE_DATA
	{
	const void *pSysMem;        //ָ���ʼ������
	UINT       SysMemPitch;		//ֻ�������й� ������Ͳ��ù�
	UINT       SysMemSlicePitch;//ֻ�������й� ����
	} D3D11_SUBRESOURCE_DATA;
	*/
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;

	// CreateBuffer()��Ҫ����������������
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(vertices);			//UINT			�������Ĵ�С(���ֽ�Ϊ��λ)
	bd.Usage = D3D11_USAGE_DEFAULT;				//D3D11_USAGE	ȷ��Ԥ����ζ�ȡ��д�뻺����,ͨ���� D3D11_USAGE_DEFAULT GPU�ɶ�д
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//UINT			��ʾ�⻺�潫Ҫ��Ϊʲô���Ļ��棬���㻺�桢�������桢��������ȵ� 
	bd.CPUAccessFlags = 0u;						//UINT			CPU���ʱ�־  �Ƿ�����CPU���� 0�Ĳ���
	bd.MiscFlags = 0u;							//UINT			�����־ �� 0(���δʹ��)��
	bd.StructureByteStride = sizeof(Vertex);	//UNIT			ÿ������Ĵ�С

	// ��ΪIASetVertexBuffers() ��Ҫ����ID3D11Buffer���� ���Դ���
	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	

	// �����㻺��������󶨵���Ⱦ���ߵ� ����װ�����׶�
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	/*void IASetVertexBuffers(
		[in]           UINT         StartSlot,    ���ڰ󶨵ĵ�һ������ۣ��������Ļ������ᱻ��ʽ�󶨵�����������
		[in]           UINT         NumBuffers,	  �����еĶ��㻺�������
		[in, optional] ID3D11Buffer * const *ppVertexBuffers, ָ�򶥵㻺���������ָ��
		[in, optional] const UINT   *pStrides,	    ����ֵ������һ���������ݵĴ�С����λ���ֽ�byte
		[in, optional] const UINT   *pOffsets		ƫ��������ע�ⶥ�㻺�治ֻ��װ�������� ������ɫ�����ߣ���������ֻ�ж���������ƫ��0
	);*/
	pContext->IASetVertexBuffers(
		0,1u,
		pVertexBuffer.GetAddressOf(), //��������� ����Ҫһ��pp ������������䣬���ڲ���ȡ��ppָ��Ķ������Բ���ֱ��ȡ��ַ �������ͷŵ���ָ��Ļ���
		&stride,		//���� ÿ�����ݵĴ�С
		&offset			//ƫ����ĳ��������һ�����������ʼλ�ã�ÿ������Ԫ��Ŀǰֻ��һ�����ݣ����� ���Բ���ƫ��
		);

	// ������������������ɫ��
	wrl::ComPtr<ID3D11VertexShader> pVertexShader; //����· ������pp���
	wrl::ComPtr<ID3DBlob> pBlob;
	// ����������Զ�ȡ�κζ������ļ���ע���������ֻ����wild string ����ǰ��۸�L ����ת��
	// !���и������⣬������������ļ���Ŀ¼��HLSL����������Ŀ¼�ǲ�ͬ�ģ���������һ��Ŀ¼������Ҫ��HLSL�ļ������� ִ���ļ����Ŀ¼,
	// ��OutDir �ĳ�ProjectDir����Ҫ�������Ŀ¼���������ĿĿ¼
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	// �βΣ�1.��ɫ���Ķ������ļ���ָ��(void*���͵� ������&pBlob ǰ��˵�� ���ͷŵ�ָ����ڴ�) 2.�������ļ�����(�����������) 3.��ʱ�����Ժ���˵ 4.pp
	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	// ���������󶨶�����ɫ�� 
	/* �ٴ����ѣ���Ҫ����ֱ�Ӵ�������ָ�룬Ҫ����Get()���ܻ�ȡ����ȷ�� �ڲ�ά�����Ǹ�ָ��*/
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

	// ��������Input(vertex) Layout(2d position only)
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	// ������,�����������������飬��ÿ��Ԫ�ض���һ��InputLayout�������� Ҫ����ɶ��ԱҪ��������ֵ�����Լ�����һ������ ��.������ֵҲ��
	/*
		typedef struct D3D11_INPUT_ELEMENT_DESC {
		  LPCSTR                     SemanticName;			//**�������������VertexShader���β�:���������ͬ
		  UINT                       SemanticIndex;			//**�����ţ���Ϊshader�п����кܶ�������������Ի���0 1 2....����
		  DXGI_FORMAT                Format;				//**��Ҫ���������������������ʲô��������꼴 2��floatԪ�� R32G32���Բ�ָ��ɫ������ָ�㡣��MSDN DXGI_FORMAT
		  UINT                       InputSlot;				//����0
		  UINT                       AlignedByteOffset;		//ĳ�����ݵĵ�һ���ֽ�λ ������������ݵĵ�һ���ֽ�λ��ƫ����
		  D3D11_INPUT_CLASSIFICATION InputSlotClass;        //����������������Ⱦʵ�����󣬼�һ��������Ⱦ�ܶ��ʵ�� ��MSDN D3D11_INPUT_CLASSIFICATION
		  UINT                       InstanceDataStepRate;
		} D3D11_INPUT_ELEMENT_DESC;
	*/
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	//�������벼�ֺ��� ��msdn 
	/*
		HRESULT CreateInputLayout(
		  [in]  const D3D11_INPUT_ELEMENT_DESC  *pInputElementDescs,  //һ������������
		  [in]  UINT               NumElements,						  //�����������������������
		  [in]  const void         *pShaderBytecodeWithInputSignature,//ָ���ѱ������shader��ָ�룬ָ���ֽ��� �����Ѿ�����shader���� ���������Ǽ������������ɫ���ǲ���ƥ���
		  [in]  SIZE_T             BytecodeLength,					  //�ֽ��볤��
		  [out] ID3D11InputLayout  **ppInputLayout					  //pp��������ǵ�InputLayout����
		);
	*/
	GFX_THROW_INFO(pDevice->CreateInputLayout(
		ied, (UINT)std::size(ied),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pInputLayout
	));

	// ������������������ɫ��
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob)); //����pBlob��Ϊԭ��ָ��Ŀռ䲻��Ҫ�� �����ͷŵ� û����
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
	// ����������������ɫ��
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

	// ������������ȾĿ�� Render Target 
	/*�������������� ��D3D��֪��������ɫ��Ӧ�����������ȥ OM Oput Merge �׶�
	 ����s ���� ��϶���Ҫһ�������ָ���� ������pp,�����Ѿ������� ��ȾĿ����pTarget ����ֻ��һ��Ԫ�ص����鼴��,
		��������ע�ⲻ��&pTarget,�������ͷŵ�ָ��Ķ���,��GetAddressOf()����ȡ�ڲ��Ǹ�ָ��ĵ�ַ pp��
	 û�õ���Ȼ��� nullptr����*/
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);
	
	// ��������Primitive Topology ԭʼ����
	/* ����IA�׶δ���ĵ�֮������˹�ϵ����ȷ��ͼԪ  
	 ��������0 1 2 3 4 5 �㡣�ߣ�01 23 45�����ߡ� �����Σ�012 345�� �����δ�:012 123 234 345 */
	pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	// �������������ӿ� Viewport
	/* ��Ϊ��buffer�ϵĶ���ӳ�䵽��Ļ�����кܶ෽��������ӳ���ȥֻռ��Ļ�����Ͻ�һ������ ������Ļ��������������ñ��bufferȥӳ�䣬�Ӷ�ʵ�ַ��� ������ӳ��HUD
	 ����Ҫ�󶨵���Ⱦ���ߣ�ֻ��Ҫ����һ���ṹ�壬Ȼ���ú���ָ��������*/
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0; //���Ͻǵ�λ��
	vp.TopLeftY = 0;
	// RS����Rasterize Stage  ע����s ˵����Ҫһ�����飬������ȡ��ַָ�������ַ��������һ��ֻ��һ��Ԫ�ص�����
	pContext->RSSetViewports(1u, &vp);


	//���������εĺ���,ֻ����������Ⱦ����ʱ�Ż�������������Ϣ�����԰����������
	GFX_THROW_INFO_ONLY(pContext->Draw((UINT)sizeof(vertices), 0u));//�βΣ�1.�������� 2.��ʼλ��
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
