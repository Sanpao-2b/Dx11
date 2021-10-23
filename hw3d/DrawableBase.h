#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"

template<class T>
class DrawableBase : public Drawable
{
public:
	// 检查静态数据是否初始化，初始化只需要一次，即第一个box进行初始化后， 后续的box无需再初始化 
	bool IsStaticInitialized() const noexcept
	{
		return !staticBinds.empty(); 
	}
	// 非索引缓存的 共用资源(static)的bindable对象 调用这个
	void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
		staticBinds.push_back(std::move(bind));
	}
	// 可共用的(static)索引缓存 调用这个
	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		pIndexBuffer = ibuf.get();				//只能初始化第一个box，后续的box没有执行if内部的绑定索引缓存 所以他们索引缓存的指针是空的
		staticBinds.push_back(std::move(ibuf));
	}
	void SetIndexfromStatic() noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		for ( const auto& b : staticBinds)
		{
			if ( const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				pIndexBuffer = p;
				return;
			}
		}
		assert("failed to find index buffer in static binds" && pIndexBuffer != nullptr);
	}

private://此处只提供给友元用的
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
private:
	static std::vector<std::unique_ptr<Bindable>> staticBinds;
};

//静态成员需要类外声明，智能指针默认都初始化nullptr
template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::staticBinds;