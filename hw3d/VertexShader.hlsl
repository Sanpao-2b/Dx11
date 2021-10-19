/*
	shader����һ���������������룬�������
	shader����Ҫ���һ��λ�ã��������������
	������ɫ������ں�����main ���������������޸ģ��ǵû�Ҫ��������ָ����������������������ɫ������Ϊ"Vertex Shader"
	д��󣬱�����ļ�������һ��.cso�ļ������Ǹ��������ļ��� ���������ʱҪ���ص�һ���ļ�������󶨵���Ⱦ���ߣ���ĳ������ȥ������
	1>------ ����������: ��Ŀ: hw3d, ����: Debug Win32 ------
	1>compilation object save succeeded; see F:\Project_VS2017\Repos\D3d_Pzh\hw3d\Debug\VertexShader.cso
*/

//����ṹ��ʵ��2�����������
struct VSOut
{
	float3 color: Color;
	float4 pos: SV_Position;
};

// ʹ�ó������� cbuffer�ǹؼ��� matrix��4*4 float
// û��ֵΪɶ�����ã��²⣺�����ڻ��ƺ����� �ѳ�������󶨵�������ɫ���ˣ����Կ��������transform����ֱ���Զ���ֵ������
cbuffer cBuf
{
	matrix transform;
};

//���Ƕ������ɫ��4������RGBA ������ֻ��3�� ���Ե��ĸ��ǿ��Ե�
VSOut main(float2 pos : Position, float3 color : Color)
{
	VSOut vso;
	vso.pos = mul(float4(pos.x, pos.y, 1.0f, 1.0f), transform);//ע�����ҳˣ���Ϊ������������1*4 �� 4*4 = 1*4 
	vso.color = color;   
	return vso;
}