#include "App.h"
#include "Box.h"
#include <memory>

App::App()
	:
	wnd(800, 600, "The Donkey Fart Box") //����͵�����wnd�Ĺ��캯�� ע���˴����࣬�������� ��ʾ �ȵ�
{
	//�����
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 1.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.08f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	
	//����һ�Ѻ���
	for (auto i = 0; i < 180; i++)
	{
		boxes.push_back(std::make_unique<Box>(wnd.Gfx(), rng, adist, ddist, odist, rdist));
	};

	// ������׵�� ����ͶӰ�������gfx��
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveFovLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

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

App::~App()
{
}

//ÿһ֡����,ע������һ֡������  EndFrame���ǽ���һ֡
void App::DoFrame()
{
	auto dt = timer.Mark(); //�Լ����ڲ�ʵ�֣����ÿ֡����Mark()�������򷵻�ֵ����ÿһ֮֡��ļ��ʱ�� nice
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);//ÿ֡��Ҫִ���������� ��Ȼ������į
	for (auto& b : boxes)
	{
		b->Update(dt);		//ÿ֡���ı� ÿһ�����Ӷ���� �任����Ϊ�µ����ֵ
		b->Draw(wnd.Gfx()); //Draw������ ��binds�����е� �ɰ󶨵Ķ��� ����������
	}
	wnd.Gfx().EndFrame();
}



 
