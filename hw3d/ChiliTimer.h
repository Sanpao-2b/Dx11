#pragma once
#include <chrono>
using namespace std::chrono;
class ChiliTimer
{
	//公共接口 给游戏逻辑用的哦
public:
	ChiliTimer();
	float Mark();
	float Peek() const;
private:
	std::chrono::steady_clock::time_point last;
};

