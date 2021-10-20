#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>
void Drawable::Draw(Graphics & gfx) const noexcept(!IS_DEBUG)
{
	for (auto& b : binds) //遍历所有bindable对象，绑定他们
	{
		b->Bind(gfx);
	}
	//传入索引的数量 画出来
	gfx.DrawIndexed( pIndexBuffer->GetCount());
}

void Drawable::Update(float dt) noexcept
{
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept
{
	//用断言函数 确保我们添加的不是索引缓存 即如果bind 装的不是索引缓存 才能继续执行 否则 终止程序
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept
{
	//确保不会重复添加索引缓存
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = ibuf.get();
	binds.push_back(std::move(ibuf));
}
