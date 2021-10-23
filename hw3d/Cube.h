#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>

// 所有可绘制的实体类 都只是创建了带索引的三角形集
class Cube
{
public:
	template<class V>	//make()返回值是三角形集
	static IndexedTriangleList<V> Make()
	{
		namespace dx = DirectX;

		constexpr float side = 1.0f / 2.0f;

		std::vector<dx::XMFLOAT3> vertices;			//顶点容器
		vertices.emplace_back(-side, -side, -side);	// 0
		vertices.emplace_back(side, -side, -side);	// 1
		vertices.emplace_back(-side, side, -side);	// 2
		vertices.emplace_back(side, side, -side);	// 3
		vertices.emplace_back(-side, -side, side);	// 4
		vertices.emplace_back(side, -side, side);	// 5
		vertices.emplace_back(-side, side, side);	// 6
		vertices.emplace_back(side, side, side);	// 7

		std::vector<V> verts(vertices.size());		//创建一个容器，元素个数=顶点数
		for (size_t i = 0; i < vertices.size(); i++)
		{
			verts[i].pos = vertices[i];   //.pos哪里来的
		}
		return{ //返回 顶点数组和索引数组
			std::move(verts),{
				0,2,1, 2,3,1,   
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			}
		};
	}
};