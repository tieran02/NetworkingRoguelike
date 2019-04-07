#pragma once
#include <vector>
#include "Vertex.h"
#include <unordered_map>
#include "Edge.h"
#include <limits>

template<class T>
class Graph
{
public:
	Graph(){}
	~Graph(){};
	void AddVertex(int id, T data);
	void AddEdge(int vertexA, int vertexB, int weight = 0);
	bool ContainsEdge(int vertexA, int vertexB) const;
	int GetEdgeWeight(int vertexA, int vertexB) const;
	const Edge& GetEdge(int vertexA, int vertexB) const;
	std::vector<Edge> GetAllEdgesFromVertex(int vertex);
	void RemoveVertex(int id);
	std::vector<Vertex<T>*> GetConnectedVerticies(int vertexID);
	const std::vector<Edge>& Edges() const { return m_edges; };
	const std::unordered_map<int, Vertex<T>>& Vertices() const { return m_vertices; }
private:
	std::vector<Edge> m_edges;
	std::unordered_map<int, Vertex<T>> m_vertices;
};


template <class T>
void Graph<T>::AddVertex(int id, T data)
{
	if (m_vertices.find(id) == m_vertices.end())
	{
		//Add to vertex list

		m_vertices.insert(std::make_pair(id, Vertex<T>(id, data)));
	}
}

template <class T>
void Graph<T>::AddEdge(int vertexA, int vertexB, int weight)
{
	Edge newEdge(vertexA, vertexB, weight);
	//check if edge already exists
	for (auto edge : m_edges)
	{
		if(newEdge == edge)
			return;
	}
	m_edges.push_back(newEdge);
}

template <class T>
bool Graph<T>::ContainsEdge(int vertexA, int vertexB) const
{
	for (auto edge : m_edges)
	{
		if(edge.VertexA == vertexA && edge.VertexB == vertexB)
			return true;
	}
	return false;
}

template <class T>
int Graph<T>::GetEdgeWeight(int vertexA, int vertexB) const
{
	for (auto edge : m_edges)
	{
		if (edge.VertexA == vertexA && edge.VertexB == vertexB)
			return edge.Weight;
	}
	return std::numeric_limits<int>::max();
}

template <class T>
const Edge& Graph<T>::GetEdge(int vertexA, int vertexB) const
{
	for (auto edge : m_edges)
	{
		if (edge.VertexA == vertexA && edge.VertexB == vertexB)
			return edge;
	}
	return Edge{std::numeric_limits<int>::max(),std::numeric_limits<int>::max() ,std::numeric_limits<int>::max() };
}

template <class T>
std::vector<Edge> Graph<T>::GetAllEdgesFromVertex(int vertex)
{
	std::vector<Edge> edges;
	for (auto edge : m_edges)
	{
		if (edge.VertexA == vertex)
			edges.push_back(edge);
	}
	return edges;
}

template <class T>
void Graph<T>::RemoveVertex(int id)
{
	if(m_vertices.find(id) != m_vertices.end())
	{
		delete m_vertices.at(id);
		m_vertices.erase(id);

		//remove all edges with that vertex
		for (int i = 0; i < m_edges.size(); ++i)
		{
			if (m_edges[i].VertexA == id || m_edges[i].VertexB == id)
				m_edges.erase(m_edges.begin() + i);
		}
	}
}

template <class T>
std::vector<Vertex<T>*> Graph<T>::GetConnectedVerticies(int vertexID)
{
	std::vector<Vertex<T>*> verticies;

	for (auto edge : m_edges)
	{
		if (edge.VertexA == vertexID)
			verticies.push_back(m_vertices[edge.VertexA]);
	}

	return verticies;
}

