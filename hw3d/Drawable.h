#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	virtual void Update(float dt) noexcept = 0;
	void AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept(!IS_DEBUG);
	virtual ~Drawable() = default;
private:
	// Drawable类需要访问子类中的StaticBinds 所以搞个友元 定义一个虚函数 最后会返回一个子类中的那个静态binds,此函数就是静态和动态连接的桥梁
	virtual const std::vector<std::unique_ptr<Bindable>> & GetStaticBinds() const noexcept = 0;
	
private:
	// 这个指针存在的目的是，有些bindable可能是索引，我们需要为他们绘制索引，
	// 注意每一个Drawable实例 只能存在一个索引缓存，具体看AddIndexBuffer代码 只允许绑定一个 
	const class IndexBuffer* pIndexBuffer = nullptr;
	// 唯一指针 因为要用到多态；存放的是Bindable类型的指针 Bindable是个基类型，可以是缓存、输入布局、着色器、等等东西
	// 之后再循环遍历这个binds 调用这个里面存放的那些指针指向的Bindable内部的binds函数(多态) 进行批量绑定
	// binds里面有大量的待绑定的东西，不知道谁是谁 只知道 他们最后都要运行各自类中的bind()
	std::vector<std::unique_ptr<Bindable>> binds;  
};