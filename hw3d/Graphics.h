#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include <d3d11.h>
#include <wrl.h>    //COM��� ������COMPointer�� ����Comptrָ��һ��COM���󣬲��������ü���
#include <vector>
#include "DxgiInfoManager.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>

class Graphics
{
	// ���������������������������������������� ��������Ԫ�����Ķ����ǲ������Զ���Ϊ��Ԫ������������������������������������������
	// ���������������������е�˽�г�Ա��ô�죿
	// ��ԪBindable���Է���Graphics����ô������Bindable���ṩ������������ĳ�Ա����ָ�룬�������ǿ��Է���Bindable�ĺ����õ���Щָ��
	friend class Bindable;
public:
	class Exception : public ChiliException
	{
		//�̳л��๹�캯��,��������������Ĭ�Ϲ��캯�������޷�ʵ�������࣬Exception exp;�ᱨ��  ����Exception exp(line , file)������
		//Ŀ�ĳ�ʼ�����࣬�����Ͳ��ö�����д���๹�캯��ȥ��ʼ������Ĺ��캯����
		//�뿴HrException��Ĭ�Ϲ��캯����ʵ��
		using ChiliException::ChiliException;
	};
	//ץHRESULT
	class HrException : public Exception
	{
	public:
		//std::vector<std::string> infoMsgs = {} ���ڽ��մ��빹�캯����DxgiInfoManager��ץ���ĵ�����Ϣ
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {} ) noexcept;
		const char* what() const noexcept override;  //��ʾ������Ϣ��Ŷ
		
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;   //���ڴ�DxgiInfoManager���л�ȡ��Ϣ��

	private:
		HRESULT hr;     //�洢HRESULT ��Ϊ����������ȥ��ƺ������Щ����HRESULT�ĺ��� 
		std::string info;  //���ڴ��DxgiInfoManager�������� ������ڵĵ�����Ϣ ��HrException�Ĺ��캯����
	};
	//����ֵΪvoid �Ǿ�ץ������� Infomation
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;   //������Щû��HRESULT����ֵ�ĺ���,��info����������ץ������Ϣ
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
	~Graphics() = default;
	
	//ǰ�󻺴淭ת������������������к�����ǰ�ᣬ�������ת�󻺴浽ǰ���棬����Զ����������
	void EndFrame();
	//����һ������ȥ����Ŀ����ͼ
	void ClearBuffer(float red, float green, float blue) noexcept;
	void DrawIndexed(UINT count);
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;

private:
	DirectX::XMMATRIX projection;
#ifndef NDEBUG   
	DxgiInfoManager infoManager;
#endif 
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};

