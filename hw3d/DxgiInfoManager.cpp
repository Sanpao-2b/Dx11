#include "DxgiInfoManager.h"
#include "Window.h"
#include "Graphics.h"
#include <dxgidebug.h>
#include <memory>

#pragma comment(lib, "dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, hr)

DxgiInfoManager::DxgiInfoManager()
{
	//����һ������ָ���ȡĳ���ӿ� 
	/*
		 ʹ��typedef���庯��ָ������ 
		 typedef int (*PF)(int,int); //��ʱ��Ϊָ��ĳ�����ͺ����ĺ���ָ�����ͣ������Ǿ���ָ�룬�����ɶ������ָ��
		 PF pf;           
		 
	*/
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);
	
	//���� ����DXGIGetDebugInterface()�����Ŀ�
	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	//��ÿ��е�DXGIGetDebugInterface()�����Ľӿ�!! ��ȡ֮�� ���Ե��øú�����
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
	);
	if (DxgiGetDebugInterface == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	HRESULT hr;
	//�������DxgiGetDebugInterface ָ����Ǹ������� �����ǰѴ�����Ϣװ��IDXGIInfoQueue
	GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &pDxgiInfoQueue));
}

//���û����������������ǽ����ȡ���ӳ��򴴽�������Ϊֹ��ȫ�������������Ϣ�� ���������� DxgiInfoManager::GetMessages()֮ǰ���ȵ���һ��Set()��¼Ŀǰ����Ϣ������
//ֻȡ��Set()֮���������Ϣ������Ҫ�ڵ����κ�D3D����֮ǰ����һ��Set ֻ��ȡ��Ϊ�������D3D�����������������Ϣ
void DxgiInfoManager::Set() noexcept
{
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

//��ȡ����Set()֮�� ������ڲ�������Ϣ����ʵ���Ǳ�����Ϣ����
std::vector<std::string> DxgiInfoManager::GetMessages() const
{
	//���ڴ��ȡ��������Ϣ
	std::vector<std::string> messages; 
	//���õ���GetMessages()����֮���������ڶ����е���Ϣ������ DXGI_DEBUG_ALL ��ʾ��ȡ���еĲ�����D3D11 DX APP�ӿڲ�������Ϣ ��MSDN DXGI_DEBUG_ID
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

	//��֮ǰ�Ƕ���Ϣ֮���������һ����ʼ������Ϣ�������һ����Ϣ
	for (auto i = next; i < end; i++)
	{
		HRESULT hr;
		SIZE_T messageLenght;
		// ����GetMessage() ���i����Ϣ����Ϣ���� ��ֵ��messageLenght  Ҳ���Ի�ȡ��Ϣ ������������û������ָ���ȥ ����null Ŀ���� �Ȼ�ȡ���ȷ����ڴ� �ٻ�ȡ��Ϣ
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessageA(DXGI_DEBUG_ALL, i, nullptr, &messageLenght));
		
		// Ϊ��Ϣ�����ڴ�,����ʱ���˸��ֽ�ָ�� ָ��һ���ڴ� �ڴ��СΪ ������ֽ���
		auto bytes = std::make_unique<byte[]>(messageLenght);
		
		// **************������Ҫ********** 
		// ���reinterpret_cast,��bytes���ָ����ڴ��ַ ת���� DXGI_INFO_QUEUE_MESSAGE*��ʽ �����DXGI_INFO_QUEUE_MESSAGE���ͱ����ĵ�ַ
		// Ȼ����һ��DXGI_INFO_QUEUE_MESSAGE* pMessage ָ��ָ������ڴ�
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		// �ٴε���GetMessage �øոյõ����ǿ���Դ��DXGI_INFO_QUEUE_MESSAGE���ڴ��ָ�룬��ȡ��i����Ϣ������pMessage�������
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessageA(DXGI_DEBUG_ALL, i, pMessage, &messageLenght));

		// pDescription�Ǹ�����Ϣ�ĳ�Ա������Ŀ���ǻ�ȡ������Ϣ���������� Ȼ��Ž�string������
		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}


