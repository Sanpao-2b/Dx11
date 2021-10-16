#include "DxgiInfoManager.h"
#include "Window.h"
#include "Graphics.h"
#include <dxgidebug.h>
#include <memory>

#pragma comment(lib, "dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, hr)

DxgiInfoManager::DxgiInfoManager()
{
	//定义一个函数指针获取某个接口 
	/*
		 使用typedef定义函数指针类型 
		 typedef int (*PF)(int,int); //此时，为指向某种类型函数的函数指针类型，而不是具体指针，用它可定义具体指针
		 PF pf;           
		 
	*/
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);
	
	//加载 包含DXGIGetDebugInterface()函数的库
	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	//获得库中的DXGIGetDebugInterface()函数的接口!! 获取之后 可以调用该函数了
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
	);
	if (DxgiGetDebugInterface == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	HRESULT hr;
	//这里调用DxgiGetDebugInterface 指向的那个函数， 差不多就是把错误信息装进IDXGIInfoQueue
	GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &pDxgiInfoQueue));
}

//如果没有这个函数，那我们将会获取到从程序创建到现在为止的全部的输出窗口消息， 所以我们在 DxgiInfoManager::GetMessages()之前，先调用一次Set()记录目前的消息数量，
//只取出Set()之后产生的消息，所以要在调用任何D3D函数之前调用一下Set 只获取因为调用这个D3D函数而产生的输出信息
void DxgiInfoManager::Set() noexcept
{
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

//获取调用Set()之后 输出窗口产生的消息，其实就是遍历消息队列
std::vector<std::string> DxgiInfoManager::GetMessages() const
{
	//用于存放取出来的信息
	std::vector<std::string> messages; 
	//设置调用GetMessages()函数之后的输出窗口队列中的信息数量， DXGI_DEBUG_ALL 表示获取所有的不论是D3D11 DX APP接口产生的信息 见MSDN DXGI_DEBUG_ID
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

	//从之前那堆消息之后产生的那一个开始，到消息队列最后一个消息
	for (auto i = next; i < end; i++)
	{
		HRESULT hr;
		SIZE_T messageLenght;
		// 调用GetMessage() 获得i个消息的消息长度 赋值给messageLenght  也可以获取消息 但是这里我们没传接收指针进去 传的null 目的是 先获取长度分配内存 再获取消息
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessageA(DXGI_DEBUG_ALL, i, nullptr, &messageLenght));
		
		// 为消息分配内存,先临时搞了个字节指针 指向一段内存 内存大小为 传入的字节数
		auto bytes = std::make_unique<byte[]>(messageLenght);
		
		// **************这里重要********** 
		// 理解reinterpret_cast,把bytes这个指向的内存地址 转换成 DXGI_INFO_QUEUE_MESSAGE*格式 即存放DXGI_INFO_QUEUE_MESSAGE类型变量的地址
		// 然后用一个DXGI_INFO_QUEUE_MESSAGE* pMessage 指针指向这块内存
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		// 再次调用GetMessage 用刚刚得到的那块可以存放DXGI_INFO_QUEUE_MESSAGE的内存的指针，获取第i个消息，传入pMessage用来填充
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessageA(DXGI_DEBUG_ALL, i, pMessage, &messageLenght));

		// pDescription是该条消息的成员函数，目的是获取该条消息的描述部分 然后放进string容器里
		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}


