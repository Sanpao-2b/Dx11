#pragma once
#include "ChiliWin.h"
#include <wrl.h>
#include <vector>
#include <dxgidebug.h>


class DxgiInfoManager
{
public:
	DxgiInfoManager(); //从库中找出DXGIGetDebugInterface()这个接口，获取到调试队列(存放输出窗口的调试信息的一个队列)，用我们定义的指针pDxgiInfoQueue指向他
	~DxgiInfoManager() = default;//释放那个调试队列

	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;

	void Set() noexcept; //Set()在每次调用D3D函数之前调用一次，记录当前信息数量是多少，并记录
	std::vector<std::string> GetMessages() const; //遍历信息队列  抓出刚刚调用的D3D函数 所产生的调试输出信息，存放到一个string容器中 并返回
private:
	unsigned long long next = 0u;

	//如果要获取输出窗口的信息，需要这个东西指向输出窗口消息队列 具体在cpp文件中可以看到
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};

