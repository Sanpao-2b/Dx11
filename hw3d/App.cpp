#include "App.h"
#include <sstream>
#include <iomanip>

App::App()
	:
	wnd(800, 600, "The Donkey Fart Box") //����͵�����wnd�Ĺ��캯�� ע���˴����࣬�������� ��ʾ �ȵ�
{}

int App::Go()
{	
	//��Ϣ�������ڴ����ڲ���
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		//�����˳���Ϣ��ִ��DoFrame����������Ϸ�߼���һֱѭ��ֱ�������˳���ϢΪֹ
		DoFrame();
	}
}

//ÿһ֡����
void App::DoFrame()
{
	wnd.Gfx().EndFrame(); //Gfx()���ص���ָ��Graphics�����ָ��
}


 
