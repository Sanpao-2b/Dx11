#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>
void Drawable::Draw(Graphics & gfx) const noexcept(!IS_DEBUG)
{
	// ������binds���ʵ��(Bindable��������ǵ�ʵ������)
	for (auto& b : binds) 
	{
		b->Bind(gfx);
	}
	// �󶨾�̬�����д�ŵ���Щ����(DrawableBase��ʵ���������е���Щ���� ���������ɫ��)
	for (auto& b : GetStaticBinds())
	{
		b->Bind(gfx);
	}
	//����gfxͼ�ζ�������������ƺ�����ע�� ���������ǰ󶨸���Ⱦ����֮����ܵ����������ȥ ����
	gfx.DrawIndexed( pIndexBuffer->GetCount());		//����box�������������pIndexBuffer ���ָ���ǿյ� ���Ա���������Ҫ��ķ����ҵ�����box��indexbuffer
}

// ���������bindable�����
void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	//�ö��Ժ��� ȷ��������ӵĲ����������� ���ܼ���ִ�� ���� ��ֹ���� 
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

// ������������
void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept(!IS_DEBUG)
{
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = ibuf.get();		  //���Ψһָ��ָ����Ǹ����� �ݴ浽pIndexBuffer��  �Ժ�ÿɻ���ʵ��Ͳ�������������κ�����������
	binds.push_back(std::move(ibuf)); //C++11���ԣ����Ч�ʣ� ���ѱ��������ÿ���һ��Ȼ��ֵ��binds ֱ������  ibuf�Ժ󲻻������Ǹ������ˡ�
}
