#include "Window.h"
#include <sstream>
#include "resource.h"

//�����ڳ�ʼ��wndClass��̬ʵ������ ������Ĭ�Ϲ��캯����
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr)) 
{
	//����������Ľṹ����������������е�����
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance(); 
	wc.hIcon = static_cast<HICON>(LoadImage(hInst,MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));

	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName(); 
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));;

	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance()); //����������ģʽ�еĶ������������
}

const char * Window::WindowClass::GetName() noexcept
{
	return wndClassName; 
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;  
}

//Window���캯�� ��������
Window::Window(int width, int height, const char * name) //�Ƴ���noexcept �����޷��׳��쳣
	://���˴�Ŵ���Ŀ�͸������ﲹ��һ��
	width(width),
	height(height)
{
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	
	//�����������ȥ��FAILED
	//FAILED�÷����ж�����������Ƿ�<0 ��<0��false ע��0����Ŷ������������
	//FAILED���������HRESULT�ģ����ǲ��������д��ں���������HRESULT �е��Ƿ������� Ҫע����
	//AdjustWindowRect ���ص���BOOL ���������0
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{	
		throw CHWND_LAST_EXCEPT(); //���Ǽ����������Ƿ�ʧ�ܣ����ʧ�� ��
	}

	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,CW_USEDEFAULT,wr.right - wr.left, wr.bottom - wr.top, 
		nullptr,nullptr,WindowClass::GetInstance(),this
		);


	if (hWnd == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}
	
	//��ʾ����
	ShowWindow(hWnd, SW_SHOWDEFAULT); 
	//��ͼ���������г�ʼ����ֵ
	pGfx = std::make_unique<Graphics>(hWnd);
}


Window::~Window()
{
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	//����Ϣ����������Ϣʱ���Ƴ���ȡ����ǰ�����Ϣ
	//����ֵΪ�棬���ʾȡ����Ϣ��
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		//����ǲ����˳���Ϣ����ס�����е�GetMessage()�����ķ���ֵ����ֱ�Ӹ����ǲ��Ƿ���ֵ ������-1�����˳�����PeekMessage()�����֪
		//�����ֶ���ȡ��msg�еĳ�Աmessage �ж��ǲ����˳���Ϣ
		if (msg.message == WM_QUIT)
		{
			return (int)msg.wParam;
		}
	
		TranslateMessage(&msg); //�����ж��Ƿ����WM_CHAR
		DispatchMessage(&msg);  //����Ϣ���͸� ��Ϣ������ ��Ϣ�������� �Ѷ�Ӧÿ����Ϣ���� ������ͬ����Ϊ��
	}	
	return {}; //���ؿյ�optional
}

Graphics& Window::Gfx()
{
	if (!pGfx)
	{
		throw CHWND_NOGFX_EXCEPT(); 
	}
	return *pGfx;
}

//��Ϣ������
LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE) 
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);  
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

//��������Ϣ������
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE: 
		PostQuitMessage(0); 
		return 0; 
	case WM_KILLFOCUS:
		kbd.ClearState(); 
		break;
/***************************** ������Ϣ ֱ�ӵ��ýӿ� ��װ�ú�nice*******************************/
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled())
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
		/***************************** ������Ϣ ֱ�ӵ��ýӿ� ��װ�ú�nice*******************************/
	
		/***************************** �����Ϣ ֱ�ӵ��ýӿ� ��װ�ú�nice*******************************/
	case WM_MOUSEMOVE://����ӻ�����������������pt���ض������
	{	
		const POINTS pt = MAKEPOINTS(lParam);
		//if inside client region 
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			//�������꣬��Move�¼��浽buffer��
			mouse.OnMouseMove(pt.x, pt.y);

			//msg����������ڴ����ڣ��������ʱ�̵�isInWindow���������false 
			//˵�������֮ǰ��client region֮�⣬�ոս���client region
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);     //WINAPI������������������� ��������뿪���ڣ�Ҳ���Լ�������MouseMove ��Ϣ
				mouse.OnMouseEnter(); //��¼���Enter client region �¼�
			}
		}
		else
		{
				/*
					ע�� "&"λ��  "|"λ��
					MK_LBUTTON | MK_RBUTTON ����Ǵ������Ҽ�������λ�ϵ�ֵΪ1 ����λȫ��0
					�ٸ�wParam��λ�룬wParam����λ�ϵ���ֵ����1 0 ȫ�����0��Ȼ�����Ұ���λ�� ֻҪ����һ����1 ��λ��Ľ����Ϊ��
					���д����һ��ִ�е�ʱ�򣬼����մ�client region����ʱ�� �������ק��״̬��������ǰ���״̬ ����ִ��if�����
					�������ק״̬��������Ӧ�����ƶ��¼�����Ȼû��client region ���� ����֮ǰ�����ʱ�����ù�SetCapture(hWnd)��׽��꣬
					ֻҪ���ͷţ���һֱ����WM_MOUSEMOVE��Ϣ һֱִ���⼸���߼��ж� ������ �������� ���¼�buffer������¼�
				*/
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			//���� �����һ���뿪���� ������ק״̬���������ڴ���ĳ��ʱ�̲���ק�� �ɿ��ˣ�
			//��ô��ε�WM_KEYDOWN��ִ���߼������� ������,�ͷŵ���������ץ����֮��Ͳ�����WM_MOUSEMOVE��Ϣ�ˣ����Ҹ��¼����м�һ������뿪���¼�
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}

	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam); //����һ�����wParam����ȡ��delta
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
		/***************************** �����Ϣ ֱ�ӵ��ýӿ� ��װ�ú�nice*******************************/
	}


	return DefWindowProc(hWnd, msg, wParam, lParam);
}


//�쳣������
Window::HrException::HrException(int line, const char * file, HRESULT hr) noexcept
	:
	Exception(line, file) ,
	hr(hr)
{}

const char * Window::HrException::what() const noexcept
{
	std::stringstream oss;
	oss << GetType() << std::endl
		<< "[Error code]" << GetErrorCode() << std::endl
		<< "[Description]" << GetErrorDescription() << std::endl
		<< GetOriginString(); 
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char * Window::HrException::GetType() const noexcept
{
	return "Chili Window Exception";
}

//��̬����
std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr, 
		hr, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf),
		0, 
		nullptr
	);

	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	std::string errorString = pMsgBuf; //�������0��ɹ� �ѻ��������ַ���������ȥ
	LocalFree(pMsgBuf); //�ͷŵ�ϵͳ����Ĵ���ַ������ǿ��ڴ�
	return errorString;
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hr);
}

const char * Window::NoGfxException::GetType() const noexcept
{
	return "Chili Window Exception [No Graphics]";
}
