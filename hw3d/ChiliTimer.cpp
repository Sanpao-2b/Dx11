#include "ChiliTimer.h"

ChiliTimer::ChiliTimer()
{
	last = steady_clock::now(); //����ʼ���е�ʱ��
}

//�״ε��ã����س���ʼ������Mark()��������ʱ��
//�Ժ���ã����ش��ϴε���Mark��������������ʱ��
//�൱�ڼ�ʱ��������һ��Mark(�൱�ڿ�ʼ��ʱ)���ڵ���һ��Mark(����)������ʱ�䣬������ν���������һ�μ�ʱ�Ŀ�ʼ��
//����ÿ��Mark()���ã����᷵��һ��ʱ��
float ChiliTimer::Mark()
{
	const auto old = last;						  //����ǳ���ʼ����(or�ϴε���Mark())��ʱ���¼
	last = steady_clock::now();					  //last����Ϊִ�е���һ��ʱ���ʱ��
	const duration<float> frameTime = last - old; //��������(or�ϴε���Mark())����ε���Ϊֹ�����˶�õ�ʱ�� ��¼����������
	return frameTime.count();
}

//ͬ�������ϴε���(or����ʼ) ��ĿǰΪֹ������ʱ�䣬���ǲ�������Mark��ʱ��
float ChiliTimer::Peek() const
{
	return duration<float>(steady_clock::now() - last).count();
}
