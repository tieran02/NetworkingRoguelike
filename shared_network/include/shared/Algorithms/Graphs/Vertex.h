#pragma once

template<class T>
struct Vertex
{
	Vertex(int id, T data) : ID(id), Data(data) {}
	int ID;
	T Data;
};