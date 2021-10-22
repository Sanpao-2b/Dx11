#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>
void Drawable::Draw(Graphics & gfx) const noexcept
{
	for (auto& b : binds) //遍历所有bindable对象，绑定他们
	{
		b->Bind(gfx);
	}
	//调用gfx图形对象里的索引绘制函数，注意 索引缓存是绑定给渲染管线之后才能调用这个函数去 绘制
	gfx.DrawIndexed( pIndexBuffer->GetCount());
}

// 添加其他的bindable用这个
void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept
{
	//用断言函数 确保我们添加的不是索引缓存 才能继续执行 否则 终止程序 
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

// 添加索引用这个
void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept
{
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = ibuf.get();		  //获得唯一指针指向的那个对象 暂存到pIndexBuffer中  以后该可绘制实体就不能再添加其他任何索引缓存了
	binds.push_back(std::move(ibuf)); //C++11特性，提高效率， 提醒编译器不用拷贝一份然后赋值给binds 直接拿走  ibuf以后不会再用那个数据了。
}
