
//���峣�����棬�����д�ŵ��� һ��float4���͵�����
cbuffer CBuf
{
	float4 face_colors[6];
};


//uint �޷�������
//SV_PritiveID ��ϵͳ���� ԭʼID �������Ⱦ���߸�ÿ������������ԭʼID(��0��ʼ�������θ���-1)
//��������������ID ӳ�䵽���������д�ŵ���ɫ������ ���ض�Ӧ��ɫ 
float4 main( uint tid : SV_PrimitiveID) : SV_Target
{
	return face_colors[tid / 2];// ����2����Ϊÿ���涼������������ �����ð��ŵ�����������ӳ�䵽ͬһ����ɫ 
}