#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics & gfx, const Drawable & parent)
	:
	parent(parent)
{
	if (!pVcbuf) //检查常量缓冲区是否已经分配，如果没分配 那就分配一下
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx); //传入gfx给它是调用VertexConstantBuffer类的母类中的 第二个构造函数，创建buffer
	}
}

void TransformCbuf::Bind(Graphics & gfx) noexcept
{
	//绑定到渲染管线之前，做个投影
	pVcbuf->Update(gfx, 
		DirectX::XMMatrixTranspose( //转置
			parent.GetTransformXM() * gfx.GetProjection()));//这里面先获取Drawable子类中的变换矩阵和gfx中的投影矩阵 相乘后得到被VertexShader使用的变换矩阵
	pVcbuf->Bind(gfx);  //直接绑定到渲染管线
}
std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCbuf::pVcbuf;