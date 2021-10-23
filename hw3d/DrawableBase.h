#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"

template<class T>
class DrawableBase : public Drawable
{
public:
	// ��龲̬�����Ƿ��ʼ������ʼ��ֻ��Ҫһ�Σ�����һ��box���г�ʼ���� ������box�����ٳ�ʼ�� 
	bool IsStaticInitialized() const noexcept
	{
		return !staticBinds.empty(); 
	}
	// ����������� ������Դ(static)��bindable���� �������
	void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
		staticBinds.push_back(std::move(bind));
	}
	// �ɹ��õ�(static)�������� �������
	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		pIndexBuffer = ibuf.get();				//ֻ�ܳ�ʼ����һ��box���������棬������boxû��ִ��if�ڲ��İ��������� �����������������ָ���ǿյ�
		staticBinds.push_back(std::move(ibuf));
	}

	// ����ǽ������box ��������Ϊ�յ����� ��������ˡ���
	void SetIndexFromStatic() noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		for ( const auto& b : staticBinds)
		{
			// �����staticBinds�� �ҵ��� �Ǹ���������(��ô�ң� ��������ת����IndexBuffer*  ֻ�������������������� ��ɹ�)���͸��Ƶ�pIndexBuffer ����return
			if ( const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				pIndexBuffer = p;
				return;
			}
		}
		// ���ִ������� ˵��û�ҵ� ��ֹ���򲢱���
		assert("failed to find index buffer in static binds" && pIndexBuffer != nullptr);
	}

private://�˴�ֻ�ṩ����Ԫ�õ�
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
private:
	static std::vector<std::unique_ptr<Bindable>> staticBinds;
};

//��̬��Ա��Ҫ��������������ָ��Ĭ�϶���ʼ��nullptr
template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::staticBinds;