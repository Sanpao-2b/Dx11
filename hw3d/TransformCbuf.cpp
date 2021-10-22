#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics & gfx, const Drawable & parent)
	:
	vcbuf(gfx),
	parent(parent)
{}

void TransformCbuf::Bind(Graphics & gfx) noexcept
{
	//绑定到渲染管线之前，做个投影
	vcbuf.Update(gfx, 
		DirectX::XMMatrixTranspose( //转置
			parent.GetTransformXM() * gfx.GetProjection()));//这里面先获取Drawable子类中的变换矩阵和gfx中的投影矩阵 相乘后得到被VertexShader使用的变换矩阵
	vcbuf.Bind(gfx);  //直接绑定到渲染管线
}
