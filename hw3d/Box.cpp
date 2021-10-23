#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Sphere.h"

Box::Box(Graphics & gfx, 
	std::mt19937 & rng,
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist)
	:
	r(rdist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dphi(odist(rng)),
	dtheta(odist(rng)),
	dchi(odist(rng)),
	chi(adist(rng)),
	theta(adist(rng)),
	phi(adist(rng))
{
	namespace dx = DirectX;

	// 每个box实例都检查一遍staticBinds是已经初始化过，第一个box执行时 没有初始化 执行if内部代码， 后续box都不会再执行了
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
		};

		auto model = Sphere::Make<Vertex>();								// 创建了一个球体模型
		model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 1.2f));  			// 对球进行缩放变换，变成椭圆
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices)); // 创建顶点缓存

		// 放到staticBinds容器中 绑定
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
		// pvs存放顶点着色器cso文件的指针
		auto pvs = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
		// 暂时存放着色器字节码 用于创建Inputlayout 否则调用std::move后 psv装的东西就莫得了
		auto pvsbc = pvs->GetBytecode();

		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));

		// 把索引缓存放进binds容器中 准备绑定
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));


		struct ConstantBuffer2
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[6];
		};

		const ConstantBuffer2 cb2 =
		{
			{
				{ 1.0f,0.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,1.0f,1.0f },
			}
		};
		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBuffer2>>(gfx, cb2));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};

		//创建唯一指针的时候就调用了InputLayout的构造函数创建了输入布局，然后放进binds容器中
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else // 后续box 执行else 把他们box对象内部的从母类继承的pIndexBuffer给填充上
	{
		SetIndexFromStatic();
	}

	// 每个box的变换矩阵 常数缓存 是不同的
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

//传入delta time 更新这些变换位置 或者旋转角度 每帧
void Box::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

//返回一个变换矩阵， 这个矩阵是世界坐标的变换矩阵 还没经过投影 投影矩阵会在绑定前一刻 加进去
DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	//注意 roll pitch yaw每帧都在变化 然后
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) * //绕box的中心旋转
			DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *			 //让box离开中心点
			DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) * //绕世界中心旋转
			DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);		 //box远离我们的摄像机一点
}
