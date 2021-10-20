#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>
class TransformCbuf : public Bindable
{
public:
	//形参获取Drawble类对象，存起来，调用他的方法
	TransformCbuf(Graphics& gfx, const Drawable& parent);
	void Bind(Graphics& gfx) noexcept override;  //调用bind 会先获取Drawable对象 中的矩阵 经过投影变换后 得到最终的 用于顶点着色器的vsbuf
private:
	VertexConstantBuffer<DirectX::XMMATRIX> vcbuf; //顶点着色器会用到这个常数缓存 内部自带bind函数可以绑定到渲染渲染管线
	const Drawable& parent;  //不是母类也可以算作是个母类所以用parent
};

