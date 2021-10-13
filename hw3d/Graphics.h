#pragma once
#include "ChiliWin.h"
#include <d3d11.h>

class Graphics
{
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	
	//给图形类添加一些行为
	//前后缓存翻转，这个函数是其他所有函数的前提，如果不翻转后缓存到前缓存，将永远看不到内容
	void EndFrame();

	//创建一个函数去清理目标视图
	/*
		D3D11DeviceContext::ClearRenderTargetView
		将渲染目标中的所有元素设置为一个值。
		void ClearRenderTargetView(
			[in] ID3D11RenderTargetView *pRenderTargetView, //传入渲染目标视图
			[in] const FLOAT[4]        ColorRGBA            //一个4分量数组，表示填充渲染目标的颜色。
		);
	*/
	void ClearBuffer(float red, float green, float blue) noexcept
	{
		//清除渲染目标视图需要一个浮点数组
		const float color[] = {red, green, blue, 1.0f};
		pContext->ClearRenderTargetView(pTarget, color);
	}
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr; //早就创建好了- - 
};
