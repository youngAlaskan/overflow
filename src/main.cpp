#include <iostream>

#include "application.h"

int main()
{
	try
	{
		Application app = Application(800, 800);

		app.Run();
	}
	catch (const std::exception& e)
	{
		(void) e; return 1;
	}

	return 0;
}