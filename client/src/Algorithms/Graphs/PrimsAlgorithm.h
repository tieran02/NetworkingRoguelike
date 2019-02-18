#pragma once
#include "Vertex.h"
#include "Graph.h"
#include <stack>

template<class T>
class PrimsAlgorithm
{
public:
	PrimsAlgorithm(Graph<T> graph) : m_graph(graph), SIZE((int)graph.Vertices().size()) {}
	void MST();
	std::vector<Edge> GetVerticiesInOrder() { return m_stack; }
private:
	Graph<T> m_graph;
	const int SIZE{ 0 };
	std::vector<Edge> m_stack;

	int minKey(const std::vector<int>& key, const std::vector<bool>& mstSet) const;
};

template <class T>
void PrimsAlgorithm<T>::MST()
{
	typedef std::pair<int, int> iPair;

	std::priority_queue<iPair, std::vector<iPair>, std::greater<iPair> > pq;

	std::vector<int> parent(SIZE, -1);
	std::vector<int> key(SIZE, std::numeric_limits<int>::max());
	std::vector<bool> inMST(SIZE, false);

	int src = 0;
	pq.push(std::make_pair(0, src));
	key[src] = 0;

	while (!pq.empty())
	{
		int u = pq.top().second;
		pq.pop();
		inMST[u] = true;

		//for each edge of a vertex
		for (auto& edge : m_graph.GetAllEdgesFromVertex(u))
		{
			int v = edge.VertexB;
			int weight = edge.Weight;

			if(inMST[v] == false && key[v] > weight)
			{
				key[v] = weight;
				pq.push(std::make_pair(key[v], v));
				parent[v] = u;
				m_stack.push_back(Edge{ parent[v],v,0 });
			}
		}
	}
}

template <class T>
int PrimsAlgorithm<T>::minKey(const std::vector<int>& key, const std::vector<bool>& mstSet) const
{
	// Initialize min value
	int min = std::numeric_limits<int>::max();
	int min_index;

	for (int v = 0; v < SIZE; v++)
	{
		if (mstSet[v] == false && key[v] < min)
		{
			min = key[v];
			min_index = v;
		}
	}

	return min_index;
}
