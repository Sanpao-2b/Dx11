#pragma once
#include <vector>
#include <DirectXMath.h>

template<class T>
// 这只是带有顶点数据和索引数据的一个结构体罢了，带有模板参数T，可以同这个来创建几何图元
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(std::vector<T> verts_in, std::vector<unsigned short> indices_in)
		:
		vertices(std::move(verts_in)),	//存储传入的顶点
		indices(std::move(indices_in))	//存储传入的索引
	{
		assert(vertices.size() > 2);
		assert(indices.size() % 3 == 0);
	}

	void Transform(DirectX::FXMMATRIX matrix)
	{
		for (auto& v : vertices)
		{
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos);
			DirectX::XMStoreFloat3(
				&v.pos,
				DirectX::XMVector3Transform(pos, matrix)
			);
		}
	}

public:
	std::vector<T> vertices;			 
	std::vector<unsigned short> indices;
};