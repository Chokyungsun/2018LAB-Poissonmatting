// calculating the nearest neighbour pixel in foreground and background

#include "stdafx.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

#define boxsize 500

struct pixel
{
	int x, y;
	int distance;
};
bool compareDistance(const pixel& p1, const pixel& p2)
{
	return (p1.distance < p2.distance);
}

int main()
{
	std::string listFile = "pixelDistance.txt";

	std::ofstream writeFile(listFile.data());
	
	std::vector<pixel> pixels = {};
	for (int y = -boxsize; y <= boxsize; y++)
	{
		for(int x=-boxsize; x<=boxsize; x++)
		{
			pixel p;
			p.x = x;
			p.y = y;
			p.distance = x*x+y*y;

			pixels.push_back(p);
		}
	}
	std::sort(pixels.begin(), pixels.end(), compareDistance);

	if (writeFile.is_open())
	{
		for (std::vector<pixel>::iterator it = pixels.begin(); it != pixels.end(); ++it)
		{
			writeFile << (*it).x << "," << (*it).y;
			writeFile << '\n';
			
		}
		writeFile.close();
	}

	return 0;

}

