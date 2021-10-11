#include "App.h"
#include <sstream>
#include <iomanip>

App::App()
	:
	wnd(800, 600, "The Donkey Fart Box") //����͵�����wnd�Ĺ��캯�� ע���˴����࣬�������� ��ʾ �ȵ�
{}

int App::Go()
{
	//�Ƴ���ǰ����Ϣ���� �ŵ���Window��ľ�̬����ProcessMessages() ���Ұ�GetMessage()������Peekmessage()�Խ�� ����Ϣʱ ��Ϸ�߼�ֹͣ��bug
	/*
		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)//gResult>0������Ϣ
		{
			TranslateMessage(&msg); //�����ж��Ƿ����WM_CHAR
			DispatchMessage(&msg);  //����Ϣ���͸� ��Ϣ������ ��Ϣ�������� �Ѷ�Ӧÿ����Ϣ���� ������ͬ����Ϊ��
		
			DoFrame();
		}
		//������GetMessage()����������
		if (gResult == -1)
		{
			throw CHWND_LAST_EXCEPT();
		}
		else
			// �����wParam�Ǵ���PostQuitMessage()���Ǹ�����
			return msg.wParam;
	*/
	
	//��Ϣ�������ڴ����ڲ���
	while (true)
	{
		//ProcessMessages()�����������optional����ֵ
		//1.ecode���յ�WM_QUIT ������Ϊecode��Ϊ������ִ��if�ڲ����
		//2.���ص��ǿգ���ecode���յ��յ�optional ��ִ���ڲ����
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		//�����˳���Ϣ��ִ��DoFrame����������Ϸ�߼���һֱѭ��ֱ�������˳���ϢΪֹ
		DoFrame();
	}
}

//ʵ��ÿһ֡�ķ����� ����������ŵ���Ϣѭ����ͷ��һ������Ϣ�ͷŽ�ϵͳ����Ϣ���У�������GetMessage()����ȡ��������
//һ��û��Ϣ�ˣ�DoFrame()�Ͳ�ִ���ˡ���������Ϸֱ��ֹͣ�ˣ�
//�����������PeekMessage() ����optional���͵ı���
void App::DoFrame()
{
	//��ȡ�Դ��ڽ�������������ʱ��
	//Peek()��һֱ���شӴ��ڽ��������ھ�����ʱ�� 
	//Ԥ��һ�����������ƶ������ͻ���±��⣬��겻���Ͳ��� ������������������������ correct��
	const float t = timer.Peek();
	std::ostringstream oss;
	oss << "Time elapsed:" << std::setprecision(1) << std::fixed << t << "s";
	wnd.SetTitle(oss.str());
}

/*
	std::setprecision(1) ������Ч����Ϊ1λ

	���ǲ�ϣ��������ǲ����룬��Ϸ�����ֱ�Ӿ�ֹ
	����취�� ����GetMessage()�����ˣ� ��PeekMessage()!!!!����ָ����Ϣ����ʽΪ PM_REMOVE����Ȼ��Ȼ�Ǵ������б���ɾ��
			  ���ǣ�PeekMessage()����û����Ϣ�����̷���
			  ����Ϣ���ط�0 û����Ϣ����0

	BOOL PeekMessage(
	  LPMSG lpMsg,
	  HWND hWnd,
	  UINT wMsgFilterMin,
	  UINT wMsgFilterMax,
	  UINT wRemoveMsg
	);
	lpMsg��		   
		[out] ָ�������Ϣ��Ϣ��MSG�ṹ��ָ�롣
	hWnd��		   
		[in] Ҫ�������Ϣ�Ĵ��ڵľ����
	wMsgFilterMin��
		[in] ָ��Ҫ������Ϣ��Χ�еĵ�һ����Ϣ��ֵ��
	wMsgFilterMax��
		[in] ָ��Ҫ������Ϣ��Χ�����һ����Ϣ��ֵ��
	wRemoveMsg��   
		[in] ָ����δ�����Ϣ���˲�������������ֵ֮һ��
			PM_NOREMOVE ����PeekMessage�������Ϣ����Ӷ�����ɾ����
			PM_REMOVE   ����Ϣ��PeekMessage�����Ӷ�����ɾ����һ�������� WM_PAINT ��Ϣ��������Ӷ�����ɾ����
	����ֵ��
		����Ϣ�����ط�0��û����Ϣ������0������������Ҫ���һ�·���ֵ����Ϊ�����������ֵû�д�����Ϣ
		���GetMessage() ����ʱ����-1



*/
 
