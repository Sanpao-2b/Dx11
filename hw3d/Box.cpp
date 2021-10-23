#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"

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
	// ��������
	struct Vertex
	{
		struct  
		{
			float x;
			float y;
			float z;
		}pos;
	};
	// ����
	const std::vector<Vertex> vertices =
	{
		{ -1.0f,-1.0f,-1.0f },
		{ 1.0f,-1.0f,-1.0f },
		{ -1.0f,1.0f,-1.0f },
		{ 1.0f,1.0f,-1.0f },
		{ -1.0f,-1.0f,1.0f },
		{ 1.0f,-1.0f,1.0f },
		{ -1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f },
	};
	// ��VertexBuffer�����ʱ������gfx��vertices ������˶��㻺��Ĵ�����Ȼ���ٵ���addbind �Ѹû���ŵ�binds����ָ�������� �ȴ��󶨵���Ⱦ����
	AddBind(std::make_unique<VertexBuffer>(gfx, vertices));
	// pvs��Ŷ�����ɫ��cso�ļ���ָ��
	auto pvs = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
	// ��ʱ�����ɫ���ֽ��� ���ڴ���Inputlayout �������std::move�� psvװ�Ķ�����Ī����
	auto pvsbc = pvs->GetBytecode();

	AddBind(std::move(pvs));

	AddBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));

	// ��������
	const std::vector<unsigned short> indices =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};

	// ����������Ž�binds������ ׼����
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));


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
	AddBind(std::make_unique<PixelConstantBuffer<ConstantBuffer2>>(gfx, cb2));


	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	//����Ψһָ���ʱ��͵�����InputLayout�Ĺ��캯�����������벼�֣�Ȼ��Ž�binds������
	AddBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

	AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

//����delta time ������Щ�任λ�� ������ת�Ƕ� ÿ֡
void Box::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

//����һ���任���� �����������������ı任���� ��û����ͶӰ ͶӰ������ڰ�ǰһ�� �ӽ�ȥ
DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	//ע�� roll pitch yawÿ֡���ڱ仯 Ȼ��
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) * //��box��������ת
			DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *			 //��box�뿪���ĵ�
			DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) * //������������ת
			DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);		 //boxԶ�����ǵ������һ��
}
