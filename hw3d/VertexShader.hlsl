// û��ֵΪɶ�����ã��²⣺�����ڻ��ƺ����� �ѳ�������󶨵�������ɫ���ˣ����Կ��������transform����ֱ���Զ���ֵ������
cbuffer cBuf
{
	matrix transform;
};

//�����ٽ�����ɫ�����ˣ����㲻��Ҫ��ɫ����������ˣ��������ɫ���� ����������ɫ����
float4 main(float3 pos : Position) : SV_Position
{
	return mul(float4(pos, 1.0f), transform);
}