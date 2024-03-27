#pragma once

#include <vector>

class Simulator
{
public:
	Simulator() { Init(); }
	~Simulator() { CleanUp(); }

	void Step() {}
public:

private:
	void Init() {}

	void CleanUp() {}
};
