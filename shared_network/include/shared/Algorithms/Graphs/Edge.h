#pragma once
struct Edge
{
	Edge(int vertexA,int vertexB, int weight): VertexA(vertexA), VertexB(vertexB), Weight(weight) {}
	int VertexA;
	int VertexB;
	int Weight;

};

inline bool operator==(const Edge& lhs, const Edge& rhs)
{
	return lhs.VertexA == rhs.VertexA && lhs.VertexB == rhs.VertexB && lhs.Weight == rhs.Weight;
}