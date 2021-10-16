#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include <d3d11.h>
#include <wrl.h>    //COM��� ������COMPointer�� ����Comptrָ��һ��COM���󣬲��������ü���
#include <vector>
#include "DxgiInfoManager.h"
class Graphics
{
public:
	class Exception : public ChiliException
	{
		//�̳л��๹�캯��,��������������Ĭ�Ϲ��캯�������޷�ʵ�������࣬Exception exp;�ᱨ��  ����Exception exp(line , file)������
		//Ŀ�ĳ�ʼ�����࣬�����Ͳ��ö�����д���๹�캯��ȥ��ʼ������Ĺ��캯����
		//�뿴HrException��Ĭ�Ϲ��캯����ʵ��
		using ChiliException::ChiliException;
	};

	class HrException : public Exception
	{
	public:
		//std::vector<std::string> infoMsgs = {} ���ڽ��մ��빹�캯����DxgiInfoManager��ץ���ĵ�����Ϣ
		//��Ӧ��DxgiInfoManager.h֮�����ģ� ��copy��ʱ��С���ȸ������
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {} ) noexcept;
		const char* what() const noexcept override;  //��ʾ������Ϣ��Ŷ
		
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;   //���ڴ�DxgiInfoManager���л�ȡ��Ϣ��

	private:
		HRESULT hr;
		std::string info;  //���ڴ��DxgiInfoManager�������� ������ڵĵ�����Ϣ ��HrException�Ĺ��캯����
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
	void ClearBuffer(float red, float green, float blue) noexcept;
	//����������
	void DrawTestTriangle();
private:
#ifndef NDEBUG    //ifndef �� if no def  û�ж��� ����DEBUG �൱����DEBUG- - �������ֿ�
	DxgiInfoManager infoManager;
#endif //���ڵ�ʽģʽ�´��ڣ�  releaseģʽ������
	
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};

/*
	unique_ptr ȥָ��COM���󣬲����Զ��ͷ�COM���� �������ֶ��ͷ�
	���������Ҫppȥ���շ���ֵ������unique_ptr�Ƿ�װ�ģ���ȡ�����ڲ��Ǹ�ָ��ĵ�ַ�������޷���Ϊpp����
	�Ӷ����Ǳ�������ָͨ���ppȥ���շ���ֵ��Ȼ��ֵ��unique_ptr �������鷳
	COM�����ǿ��Թ���Comptrȥָ��ģ����������ü�������unique_ptr�����ռ
	comptr���кܶ���õĺ�����
	����Ҫ����&

*/
