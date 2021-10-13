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
	
	//��ͼ�������һЩ��Ϊ
	//ǰ�󻺴淭ת������������������к�����ǰ�ᣬ�������ת�󻺴浽ǰ���棬����Զ����������
	void EndFrame();

	//����һ������ȥ����Ŀ����ͼ
	/*
		D3D11DeviceContext::ClearRenderTargetView
		����ȾĿ���е�����Ԫ������Ϊһ��ֵ��
		void ClearRenderTargetView(
			[in] ID3D11RenderTargetView *pRenderTargetView, //������ȾĿ����ͼ
			[in] const FLOAT[4]        ColorRGBA            //һ��4�������飬��ʾ�����ȾĿ�����ɫ��
		);
	*/
	void ClearBuffer(float red, float green, float blue) noexcept
	{
		//�����ȾĿ����ͼ��Ҫһ����������
		const float color[] = {red, green, blue, 1.0f};
		pContext->ClearRenderTargetView(pTarget, color);
	}
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr; //��ʹ�������- - 
};
