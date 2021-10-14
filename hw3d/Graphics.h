#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include <d3d11.h>
#include <vector>
#include "DxgiInfoManager.h"

class Graphics
{
public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	};

	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};

	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
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
