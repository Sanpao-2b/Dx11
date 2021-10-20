#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics & gfx, const Drawable & parent)
	:
	vcbuf(gfx),
	parent(parent)
{}

void TransformCbuf::Bind(Graphics & gfx) noexcept
{
	//绑定到渲染管线之前，先更新一下，因为可能Drawable那边的小儿子们还要改变换啥的
	vcbuf.Update(gfx, 
		DirectX::XMMatrixTranspose(
			parent.GetTransformXM() * gfx.GetProjection()));//这里面先获取Drawable子类中的变换矩阵和图像类中的投影矩阵 相乘后得到被VertexShader使用的变换矩阵
	vcbuf.Bind(gfx);  //直接绑定到渲染管线
}
