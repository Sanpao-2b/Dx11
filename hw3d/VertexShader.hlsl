/*
	shader����һ���������������룬�������
	shader����Ҫ���һ��λ�ã��������������
	������ɫ������ں�����main ���������������޸ģ��ǵû�Ҫ��������ָ����������������������ɫ������Ϊ"Vertex Shader"
	д��󣬱�����ļ�������һ��.cso�ļ������Ǹ��������ļ��� ���������ʱҪ���ص�һ���ļ�������󶨵���Ⱦ���ߣ���ĳ������ȥ������
	1>------ ����������: ��Ŀ: hw3d, ����: Debug Win32 ------
	1>compilation object save succeeded; see F:\Project_VS2017\Repos\D3d_Pzh\hw3d\Debug\VertexShader.cso
*/

//������������ͨ������ͬ��Ҫ��һ���������ǡ�������
//ע��������SV_Position�ں������� SV��ϵͳ��������д
float4 main(float2 pos : Position) : SV_Position
{
	return float4(pos.x, pos.y, 0.0f, 1.0f);
}