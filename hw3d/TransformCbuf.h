#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

//TransformCbuf 变换常数缓存类，内部有一个 VertexConstantBuffer 存放矩阵
//注意注意注意：： 尽管他是一个VertexConstantBuffer 但是我们没继承这个顶点常数缓存 而是继承的bindable 
class TransformCbuf : public Bindable
{
public:
	//形参获取Drawble类对象，存起来，调用他的方法
	TransformCbuf(Graphics& gfx, const Drawable& parent);
	void Bind(Graphics& gfx) noexcept override;  //调用bind 会先获取Drawable对象 中的矩阵 经过投影变换后 得到最终的 用于顶点着色器的vsbuf
private:
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> pVcbuf;
	const Drawable& parent;  //不是母类也可以算作是个母类所以用parent，保存一个对drawable类的引用
};
