#pragma once
#include "ChiliWin.h"//��֤���ͷ�ļ�����ˣ���Ϊ�����������ͷ�ļ�����������棬��������ChiliWin.h��������Щnonono�ͻ�ʧЧ
#include "ChiliException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
//c++17 ����ѡ�񷵻�һ��int����һ���յ�optional �����Ϣ����ܺ� optional<T> �൱��T���ͱ�����һ��boolֵ�Ĵ�������е�bool������ʾT�����Ƿ�Ϊ��
#include <optional> 
#include <memory> //����ָ��
class Window
{
	//�쳣�����Ա��
public:
	class Exception : public ChiliException
	{
	public:    
		//��ͼ����Ĺ��캯���Աȿ� ���������๹�캯���г�ʼ������
		Exception(int line, const char* file, HRESULT hr) noexcept;//HRESULT���ڳ���ר���Ĵ�����
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;//��hr������� ת�������ܿ����ַ���������
		HRESULT GetErrorCode() const noexcept;                     //���س�Ա����hr
		std::string GetErrorString() const noexcept;               //����TranslateErrorCode() ������
	private:
		HRESULT hr;
	};

private:
	class WindowClass 
	{
	public: 
		static const char* GetName() noexcept;   //���ش�������
		static HINSTANCE GetInstance() noexcept; //���ؾ�̬ʵ��
	private:
		WindowClass() noexcept; //ע�ᴰ����
		~WindowClass(); //ע��������
		WindowClass(const WindowClass&) = delete; 
		WindowClass& operator=(const WindowClass&) = delete;
	private:
		static constexpr const char* wndClassName = "Chili Direct3D Engine Window";
		static WindowClass wndClass; //����ǰ�ʹ�����
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const char* name); //noexcept
	~Window();//���ٴ���
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string title); //���ô��ڱ���
	static std::optional<int> ProcessMessages(); 
	Graphics& Gfx(); //���ڷ���ָ��ͼ�ζ����ָ��
	
private: 
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard kbd;  //�¼ӵļ���
	Mouse mouse;
private:
	int width; 
	int height;
	HWND hWnd; //���ھ��
	std::unique_ptr<Graphics> pGfx; //˽������ ��Ҫ�ṩ�������� ��Ҫֱ������̺����һ����������� ��ΪhWnd�ò��� ����Ҫ��Ū����ָ�룬Ȼ���ڴ���Ĭ�Ϲ�����ȥ����hWnd
};


#define CHWND_EXCEPT(hr) Window::Exception(__LINE__,__FILE__,hr)
#define CHWND_LAST_EXCEPT() Window::Exception(__LINE__,__FILE__,GetLastError())
