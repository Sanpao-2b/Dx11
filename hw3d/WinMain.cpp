/*
	2021��10��10��14:46:54
	��Ϸ���ڻ������
	APP����������ڣ�APP����Ϸ�߼����Ե��ô��ڵ����"��ꡢ���̡�ͼ��"����ؽӿ�(public���Բ���)

	���ǵ���Ϸ�������������ڣ���һ���������Լ������(��ꡢ���̡�ͼ�δ���)��
	��Ϣѭ����������������ƶ���� ϵͳ���Զ�������Ϣ��ŵ���Ϣ�����У������ڴ������е��Զ������Ϣ��������ȡ���� �����ж���ʲô���͵���Ϣ Ȼ�������Ӧ������ӿ� ���д��
	����Щ�����¼�������Ϊ ��Ҫ�����ǵ�App�е�Doframe()������ʵ�� ȡ����Ӧ����Ϣ ������Ϊ����������� �򲻻������Ϊ

	Ŀǰ�Ĵ���ִ���߼���
	=> App{}.Go();������ʱApp�������Go()����
	=> Go()����������һ������ѭ��while(true)���ڲ���if�ж�ProcessMessages()�ķ���ֵ(����Ϊoptional)��
			   �����WM_QUIT��ִ��if�ڲ������ֹ��������ǿ�optional����ִ��DoFrame()��Ϸ�߼�
	=> ProcessMessages()��һ���������ṩ�Ĺ��еľ�̬��Ա���� ���ڴ���ÿ�����ڵ���Ϣ���������ڲ���Message bump ��Ϣ�ã�����WIN32�ӿ�PeekMessage()��WinAPIȡ����Ϣ
	=> ���ȡ����Ϣ�ɹ������ж��ǲ���WM_QUIT������ǣ��򷵻�msg.wParam, WM_QUIT��Ϣ�е�wParam����װ�ľ����˳�����Ķ����ƴ���
	=> �������WM_QUIT������TranslateMessage()��DispatchMessage(),����Ϣmsg����HandleMsg()�������Զ����WindowProc()������������Ϣ
	=> ������������Ҫ�������Ϣ���case�����봦��ľͽ���ϵͳĬ��DefWindowProc()��������
			   ����ǰ�����Ϣ������ü�������Ľӿڣ�������Ӧ����״̬��������Ӧ�İ����¼����簴��buffer��
			   ����������Ϣ��������������Ľӿڣ�������Ӧ����״̬��������Ӧ������¼��������buffer��
	=> ��������Щ�������������������Ӧ����Ϊ��Ҫ����App�����ǵ���Ϸ��Doframe��ȥ����"��ꡢ���̡�ͼ��"�ṩ�Ĺ����ӿ� ȡ�����ǵ�buffer�е��¼���������Ӧ�Ķ��� ������Ϸ����֮���
*/
#include "Window.h"
#include <sstream>
#include "App.h"
int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	try
	{
		//App{}�൱�ڴ�������ʱ������Ȼ����ʱ������Go()��һֱѭ����ֱ������ֵ���˳�����
		return App{}.Go();
	}

	// ���throw������һ��ChiliException����(��������)��һ����������ִ��
	catch (const ChiliException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);//ok��ť�͸�̾��
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No Details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}


