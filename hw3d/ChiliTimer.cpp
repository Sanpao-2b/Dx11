#include "ChiliTimer.h"

ChiliTimer::ChiliTimer()
{
	last = steady_clock::now(); //程序开始运行的时间
}

//首次调用：返回程序开始到调用Mark()所经历的时间
//以后调用：返回从上次调用Mark到现在所经历的时间
//相当于计时器，调用一次Mark(相当于开始计时)，在调用一次Mark(结束)，返回时间，并且这次结束又是下一次计时的开始，
//所以每次Mark()调用，都会返回一个时间
float ChiliTimer::Mark()
{
	const auto old = last;						  //这个是程序开始运行(or上次调用Mark())的时间记录
	last = steady_clock::now();					  //last更新为执行到这一行时候的时刻
	const duration<float> frameTime = last - old; //程序运行(or上次调用Mark())到这次调用为止经历了多久的时间 记录下来并返回
	return frameTime.count();
}

//同样返回上次调用(or程序开始) 到目前为止经历的时间，但是不会重置Mark计时器
float ChiliTimer::Peek() const
{
	return duration<float>(steady_clock::now() - last).count();
}
