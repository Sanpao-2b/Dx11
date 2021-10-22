#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>
void Drawable::Draw(Graphics & gfx) const noexcept
{
	for (auto& b : binds) //��������bindable���󣬰�����
	{
		b->Bind(gfx);
	}
	//����gfxͼ�ζ�������������ƺ�����ע�� ���������ǰ󶨸���Ⱦ����֮����ܵ����������ȥ ����
	gfx.DrawIndexed( pIndexBuffer->GetCount());
}

// ���������bindable�����
void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept
{
	//�ö��Ժ��� ȷ��������ӵĲ����������� ���ܼ���ִ�� ���� ��ֹ���� 
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

// ������������
void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept
{
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = ibuf.get();		  //���Ψһָ��ָ����Ǹ����� �ݴ浽pIndexBuffer��  �Ժ�ÿɻ���ʵ��Ͳ�������������κ�����������
	binds.push_back(std::move(ibuf)); //C++11���ԣ����Ч�ʣ� ���ѱ��������ÿ���һ��Ȼ��ֵ��binds ֱ������  ibuf�Ժ󲻻������Ǹ������ˡ�
}
