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

//ÿһ֡����,ע������һ֡������  EndFrame���ǽ���һ֡
void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f; 
	wnd.Gfx().ClearBuffer(c, c, 1.0f);				 
	wnd.Gfx().DrawTestTriangle(
		timer.Peek(), 
		wnd.mouse.GetPosX()/400.0f - 1,  //�ӿ�����ϵ�У�X���������-1 ~ 1 ������������0~800 ����Ҫ/400 -1 �ѷ�Χ���Ƶ�(-1, 1)
		-wnd.mouse.GetPosY()/300.0f + 1); //Y��ͬ����ͬ���� �ӿ�Y���������-1 ~ 1���������������������������෴ ����Ҫȡ��
	wnd.Gfx().EndFrame(); 
} 



 
