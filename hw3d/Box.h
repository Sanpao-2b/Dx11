#pragma once
#include "Drawable.h"


class Box : public Drawable
{
public:
	//mt19937�����������������   uniform_real_distribution ���������ȷֲ�
	//������캯������ʵ�������Ӷ���Σ���������ȫ�����
	Box( Graphics& gfx, 
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist);
	// ���²���
	void Update(float dt) noexcept override;
	// �Ѳ���ת����DXMATRIX��ʽ
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	float r; //�뾶
	// λ��
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	// �Ƕ����
	float theta = 0.0f;	//ϣ����ĸ��
	float phi;			//ϣ����ĸ��
	float chi;			//ϣ����ĸX
	// �ٶ� (delta/s)
	// �����ת�ٶ�
	float droll;
	float dpitch;
	float dyaw;
	// ������ת�ٶ�
	float dtheta;
	float dphi;
	float dchi;
};

