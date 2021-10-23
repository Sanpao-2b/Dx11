#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>
void Drawable::Draw(Graphics & gfx) const noexcept(!IS_DEBUG)
{
	// 绑定所有binds里的实例(Bindable类的子类们的实例对象)
	for (auto& b : binds) 
	{
		b->Bind(gfx);
	}
	// 绑定静态容器中存放的那些东西(DrawableBase的实例对象们中的那些东西 比如各种着色器)
	for (auto& b : GetStaticBinds())
	{
		b->Bind(gfx);
	}
	//调用gfx图形对象里的索引绘制函数，注意 索引缓存是绑定给渲染管线之后才能调用这个函数去 绘制
	gfx.DrawIndexed( pIndexBuffer->GetCount());		//后续box又在这里调用了pIndexBuffer 这个指针是空的 所以报错。我们需要别的方法找到后续box的indexbuffer
}

// 添加其他的bindable用这个
void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	//用断言函数 确保我们添加的不是索引缓存 才能继续执行 否则 终止程序 
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

// 添加索引用这个
void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept(!IS_DEBUG)
{
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = ibuf.get();		  //获得唯一指针指向的那个对象 暂存到pIndexBuffer中  以后该可绘制实体就不能再添加其他任何索引缓存了
	binds.push_back(std::move(ibuf)); //C++11特性，提高效率， 提醒编译器不用拷贝一份然后赋值给binds 直接拿走  ibuf以后不会再用那个数据了。
}
