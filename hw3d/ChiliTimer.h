#pragma once
#include <chrono>
using namespace std::chrono;
class ChiliTimer
{
	//�����ӿ� ����Ϸ�߼��õ�Ŷ
public:
	ChiliTimer();
	float Mark();
	float Peek() const;
private:
	std::chrono::steady_clock::time_point last;
};

