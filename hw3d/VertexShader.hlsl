/*
	shader����һ���������������룬�������
	shader����Ҫ���һ��λ�ã��������������
	������ɫ������ں�����main ���������������޸ģ��ǵû�Ҫ��������ָ����������������������ɫ������Ϊ"Vertex Shader"
	д��󣬱�����ļ�������һ��.cso�ļ������Ǹ��������ļ��� ���������ʱҪ���ص�һ���ļ�������󶨵���Ⱦ���ߣ���ĳ������ȥ������
	1>------ ����������: ��Ŀ: hw3d, ����: Debug Win32 ------
	1>compilation object save succeeded; see F:\Project_VS2017\Repos\D3d_Pzh\hw3d\Debug\VertexShader.cso
*/

//����ṹ��ʵ��2�����������

/*
	�������ṹ���� position����ǰ�棬���к�������ɫ�������
	����ܻ���� ���������벼���ж��������壬��Ӧ��֪�������￪ʼ����ɫ���ݣ��������ᣬ��ֻ�����������Ƿ�ƥ�䣬�����ܶ�ȡ˳��
	����������VSOut ����Ҫ����ɫ����ǰ��
*/
struct VSOut
{
	float3 color: Color;
	float4 pos: SV_Position;
};

//ע�����������SV_Position������������InputLayout�Զ����
VSOut main(float2 pos : Position, float3 color : Color)
{
	VSOut vso;
	vso.pos = float4(pos.x, pos.y, 1.0f, 1.0f); //��ΪҪ����2���� ���Բ���ֱ��=pos
	vso.color = color;   //��Ϊcolor����rgb����ֵ ��vso.colorһ�� ����ֱ��=
	return vso;
	//���Ǹ����˶������ɫ�����Rasterizer����в�ֵ ���������ڲ��������ɫ
}