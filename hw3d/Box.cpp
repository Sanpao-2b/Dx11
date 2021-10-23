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

	// ÿ��boxʵ�������һ��staticBinds���Ѿ���ʼ��������һ��boxִ��ʱ û�г�ʼ�� ִ��if�ڲ����룬 ����box��������ִ����
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
		};

		auto model = Sphere::Make<Vertex>();								// ������һ������ģ��
		model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 1.2f));  			// ����������ű任�������Բ
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices)); // �������㻺��

		// �ŵ�staticBinds������ ��
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
		// pvs��Ŷ�����ɫ��cso�ļ���ָ��
		auto pvs = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
		// ��ʱ�����ɫ���ֽ��� ���ڴ���Inputlayout �������std::move�� psvװ�Ķ�����Ī����
		auto pvsbc = pvs->GetBytecode();

		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));

		// ����������Ž�binds������ ׼����
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

		//����Ψһָ���ʱ��͵�����InputLayout�Ĺ��캯�����������벼�֣�Ȼ��Ž�binds������
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else // ����box ִ��else ������box�����ڲ��Ĵ�ĸ��̳е�pIndexBuffer�������
	{
		SetIndexFromStatic();
	}

	// ÿ��box�ı任���� �������� �ǲ�ͬ��
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
