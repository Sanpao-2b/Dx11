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
			//������˳���Ϣ ֱ�ӷ���
			return *ecode;
		}
		//�����˳���Ϣ��ִ��DoFrame����������Ϸ�߼���һֱѭ��ֱ�������˳���ϢΪֹ
		DoFrame();
	}
}

//ÿһ֡����
void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f; //time.peek()�Ǵӳ������е����ڵ�ʱ�䣬c�Ǹ�0~1�仯���� ��Ҳ����ɫ�仯�ķ�Χ
	wnd.Gfx().ClearBuffer(c, c, 1.0f);				 //����ɫ�Ǳ仯������
	
	wnd.Gfx().EndFrame(); //Gfx()���ص���ָ��Graphics�����ָ��,����ͼ�����еĺ���
}



 
