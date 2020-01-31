
#include <vector>
#include <iostream>
#include <algorithm>

int maxArea(std::vector<int>& height) {
	int i = 0;
	int j = 0;
	int capacity = 0;
	int size = height.size();

	while(i < size - 1)
	{
		j = i + 1;
		while (j < size)
		{
			int tmpCap = std::min(height[i], height[j]) * (j - i);
			if (tmpCap >= capacity)
			{
				capacity = tmpCap;
				std::cout << capacity << std::endl;
			}
			j++;
		}
		i++;
	}

	return capacity;
}


int main()
{
	std::vector<int> height = { 1, 8, 6, 2, 5, 4, 8, 3, 7 };
	int ret = maxArea(height);
	std::cout << ret;
	getchar();
}