#pragma once
#include <unordered_map> //����ӳ������ �ڲ�ʵ���˹�ϣ�� ���Ҽ��� ���ǽ�����Է����� ����ͨmap����ʹ�õ�ʱ��������
#include <Windows.h>


class WindowsMessageMap
{
public:
	WindowsMessageMap(); 
	std::string operator()(DWORD msg, LPARAM lp, WPARAM wp)const;
private:
	std::unordered_map<DWORD, std::string> map; 
};

