#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include <unordered_set>
#include "Random.h"
#include "Algorithms/Graphs/PrimsAlgorithm.h"
#include "DungeonGeneration/DungeonChunk.h"
#include "sstream"
#include "Utility/Log.h"

int sign(int val) {
	return ((0) < val) - (val < (0));
}

DungeonChunk::DungeonChunk(int x, int y) : chunkX(x), chunkY(y)
{
	//Allocate dungeon tiles
	m_tiles = new DungeonTile*[CHUNK_SIZE];
	for (int i = 0; i < (int)CHUNK_SIZE; ++i)
		m_tiles[i] = new DungeonTile[CHUNK_SIZE];

	int id{ 0 };
	for (int y = 0; y < (int)CHUNK_SIZE; y++)
	{
		for (int x = 0; x < (int)CHUNK_SIZE; x++)
		{
			m_tiles[y][x].x = x;
			m_tiles[y][x].y = y;
			m_tiles[y][x].id = id++;
			m_tiles[y][x].type = DungeonTileType::EMPTY;
		}
	}
}

DungeonChunk::~DungeonChunk()
{
	for (int i = 0; i < (int)CHUNK_SIZE; ++i) {
		delete[] m_tiles[i];
	}
	delete[] m_tiles;
}

void DungeonChunk::Draw(sf::RenderWindow& window)
{
	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(16, 16));
	rectangle.setFillColor(sf::Color::Blue);

	sf::Vector2i offset{ chunkX * (int)CHUNK_SIZE * 16, chunkY * (int)CHUNK_SIZE * 16 };

	for (int y = 0; y < (int)CHUNK_SIZE; ++y)
	{
		for (int x = 0; x < (int)CHUNK_SIZE; ++x)
		{
			sf::Vector2f position = sf::Vector2f((x*16.0f), (y*16.0f)) + (sf::Vector2f)offset;
			rectangle.setPosition(position);

			switch (m_tiles[y][x].type)
			{
			case DungeonTileType::EMPTY:
				break;
			case DungeonTileType::WALL:
				window.draw(rectangle);
				break;
			default:
				break;
			}
		}
	}
}

bool DungeonChunk::InBounds(int x, int y) const
{
	return x >= 0 && x < (int)CHUNK_SIZE && y >= 0 && y < (int)CHUNK_SIZE;
}

void DungeonChunk::AddNeighbour(DungeonChunk* chunk)
{
	m_neighbourChunks.push_back(chunk);
}

const std::vector<DungeonRoom>& DungeonChunk::GetRooms() const
{
    return m_rooms;
}
void DungeonChunk::connectRooms()
{
	if (m_rooms.size() <= 1)
	{
		return;
	}

	Graph<DungeonRoom*> graph;
	//Add all rooms to the graph and connect all rooms with one another
	for (int i = 0; i < m_rooms.size(); ++i)
	{
		graph.AddVertex(i, &m_rooms[i]);
	}

	for (int i = 0; i < m_rooms.size(); ++i)
	{
		for (int j = 0; j < m_rooms.size(); ++j)
		{
			if(i == j)
				continue;

			graph.AddEdge(i, j, getDistanceBetweenRooms(m_rooms[i],m_rooms[j])); //TODO add weight as distance
		}
	}
	PrimsAlgorithm<DungeonRoom*> minimalSpanningTree(graph);
	minimalSpanningTree.MST();
	auto order = minimalSpanningTree.GetVerticiesInOrder();
	for (int i = 0; i < (int)order.size(); i++)
	{
		const auto& edge = order[i];
		DungeonRoom* roomA = graph.Vertices().at(edge.VertexA).Data;
		DungeonRoom* roomB = graph.Vertices().at(edge.VertexB).Data;

		connectRoom(*roomA, *roomB);
	}
}

void DungeonChunk::connectRoom(DungeonRoom& roomA, DungeonRoom& roomB)
{
	if (roomA.IsConnected(roomB))
		return;

	DungeonRoom::ConnectRooms(roomA, roomB);

	int bestDistance = 0;
	DungeonTile* bestTileA{ nullptr };
	DungeonTile* bestTileB{ nullptr };
	DungeonRoom* bestRoomA{ nullptr };
	DungeonRoom* bestRoomB{ nullptr };
	bool possibleConnectionFound = false;

	for (int tileIndexA = 0; tileIndexA < (int)roomA.GetEdgeTiles().size(); ++tileIndexA)
	{
		for (int tileIndexB = 0; tileIndexB < (int)roomB.GetEdgeTiles().size(); ++tileIndexB)
		{
			DungeonTile* tileA = roomA.GetEdgeTiles()[tileIndexA];
			DungeonTile* tileB = roomB.GetEdgeTiles()[tileIndexB];
			int distance = (int)(pow(tileA->x - tileB->x, 2) + pow(tileA->y - tileB->y, 2));

			if (distance < bestDistance || !possibleConnectionFound)
			{
				bestDistance = distance;
				possibleConnectionFound = true;
				bestTileA = tileA;
				bestTileB = tileB;
				bestRoomA = &roomA;
				bestRoomB = &roomB;
			}
		}
	}

	if (possibleConnectionFound)
	{
		createPassage(*bestTileA, *bestTileB);
	}
}

int DungeonChunk::getDistanceBetweenRooms(DungeonRoom& roomA, DungeonRoom& roomB)
{
	int bestDistance = 0;
	bool possibleConnectionFound = false;

	for (int tileIndexA = 0; tileIndexA < (int)roomA.GetEdgeTiles().size(); ++tileIndexA)
	{
		for (int tileIndexB = 0; tileIndexB < (int)roomB.GetEdgeTiles().size(); ++tileIndexB)
		{
			DungeonTile* tileA = roomA.GetEdgeTiles()[tileIndexA];
			DungeonTile* tileB = roomB.GetEdgeTiles()[tileIndexB];
			int distance = (int)(pow(tileA->x - tileB->x, 2) + pow(tileA->y - tileB->y, 2));

			if (distance < bestDistance || !possibleConnectionFound)
			{
				bestDistance = distance;
				possibleConnectionFound = true;
			}
		}
	}

	if (possibleConnectionFound)
	{
		return bestDistance;
	}
	return std::numeric_limits<int>::max();
}

void DungeonChunk::processMap()
{
	//add connections to neighbours
	for (auto neighbour_chunk : m_neighbourChunks)
	{
		int dx = chunkX - neighbour_chunk->GetX();
		int dy = chunkY - neighbour_chunk->GetY();

		if(dx == 0 && dy == 1) // connect to bottom
		{
			drawCircle(m_tiles[0][CHUNK_SIZE / 2], 2);
		}
		else if (dx == 0 && dy == -1) // connect to top
		{
			drawCircle(m_tiles[CHUNK_SIZE - 1][CHUNK_SIZE / 2], 2);
		}
		else if (dx == 1 && dy == 0) // connect to right
		{
			drawCircle(m_tiles[CHUNK_SIZE / 2][0], 2);
		}
		else if (dx == -1 && dy == 0) // connect to right
		{
			drawCircle(m_tiles[CHUNK_SIZE / 2][CHUNK_SIZE - 1], 2);
		}

	}
	detectRooms();

	connectRooms();
}

void DungeonChunk::detectRooms()
{
	m_rooms.clear();
	std::unordered_set<int> visitedNodes;

	for (int y = 0; y < (int)CHUNK_SIZE; y++)
	{
		for (int x = 0; x < (int)CHUNK_SIZE; x++)
		{
			if (visitedNodes.find(m_tiles[y][x].id) == visitedNodes.end() && m_tiles[y][x].type == DungeonTileType::EMPTY)
			{
				auto tiles = getRoom(x, y);
				m_rooms.push_back(tiles);

				for (auto& tile : tiles.GetTiles())
				{
					visitedNodes.insert(tile->id);
				}
			}
		}
	}
	std::stringstream stream;
	stream << "Region count " << m_rooms.size();
	LOG_TRACE(stream.str());
}

DungeonRoom DungeonChunk::getRoom(int startX, int startY)
{
	std::vector<DungeonTile*> tiles;

	std::unordered_set<int> visitedNodes;

	DungeonTileType tileType = m_tiles[startY][startX].type;

	std::queue<DungeonTile*> queue;
	queue.push(&m_tiles[startY][startX]);
	visitedNodes.insert(m_tiles[startY][startX].id);

	while (!queue.empty())
	{
		DungeonTile* tile = queue.front();
		queue.pop();
		tiles.push_back(tile);

		for (auto& neighbour : getNeighbours(tile->x, tile->y))
		{
			if (neighbour->type == tileType && visitedNodes.find(neighbour->id) == visitedNodes.end()) {
				visitedNodes.insert(neighbour->id);
				queue.push(neighbour);
			}
		}
	}

	return DungeonRoom(tiles, m_tiles, CHUNK_SIZE);
}

std::vector<DungeonTile*> DungeonChunk::getNeighbours(int startX, int startY)
{
	std::vector<DungeonTile*> tiles;
	for (int y = startY - 1; y <= startY + 1; ++y)
	{
		for (int x = startX - 1; x <= startX + 1; ++x)
		{
			if (InBounds(x, y) && (y == startY || x == startX))
			{
				tiles.push_back(&m_tiles[y][x]);
			}
		}
	}
	return tiles;
}

std::vector<DungeonTile*> DungeonChunk::getLine(const DungeonTile& from, const DungeonTile& to)
{
	std::vector<DungeonTile*> line;

	int x = from.x;
	int y = from.y;

	int dx = to.x - from.x;
	int dy = to.y - from.y;

	bool inverted = false;

	int step = sign(dx);
	int gradientStep = sign(dy);

	int longest = abs(dx);
	int shortest = abs(dy);

	if(longest < shortest)
	{
		inverted = true;
		longest = shortest;
		shortest = abs(dx);

		step = sign(dy);
		gradientStep = sign(dx);
	}

	int gradiantAccumlation = longest / 2;
	for (int i = 0; i < longest; ++i)
	{
		line.push_back(&m_tiles[y][x]);

		if (inverted)
			y += step;
		else
			x += step;

		gradiantAccumlation += shortest;
		if(gradiantAccumlation >= longest)
		{
			if (inverted)
				x += gradientStep;
			else
				y += gradientStep;

			gradiantAccumlation -= longest;
		}
	}

	return line;
}

void DungeonChunk::createPassage(const DungeonTile& tileA, const DungeonTile& tileB)
{
	auto line = getLine(tileA, tileB);

	//create passage by making a cirlce at each tile
	for (auto& tile : line)
	{
		drawCircle(*tile, 2);
	}
}


void DungeonChunk::drawCircle(const DungeonTile& tile, int r)
{
	for (int x = -r; x < r; ++x)
	{
		for (int y = -r; y < r; ++y)
		{
			if (x*x + y * y <= r * r)
			{
				int realX = tile.x + x;
				int realY = tile.y + y;
				if(InBounds(realX, realY))
				{
					m_tiles[realY][realX].type = DungeonTileType::EMPTY;
				}
			}
		}
	}
}
