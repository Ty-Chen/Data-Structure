#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using std::string;
using std::vector;

class Solution {
public:
	int myAtoi(string str) {

		int valid = 0;
		int ret = 0;

		for (char c : str)
		{
			if (c == ' ')
				continue;

			if (c == '-')
			{
				valid = -1;
			}
			else if ('0' < c && c <= '9')
			{
				if (valid == 0)
				{
					valid = 1;
				}

				if (ret > INT_MAX / 10 || (ret == INT_MAX / 10 && ((int)c - '0' > 7)))
				{
					if (valid == 1)
						return INT_MAX;
					else if (valid == -1)
						return INT_MIN;
				}
				ret = 10 * ret + (int)c - '0';
				
			}
			else if (valid == 0)
				return 0;
			else
				continue;
		}

		return ret * valid;
	}

	string longestCommonPrefix(vector<string>& strs) {
		int count = 0;
		int size = 0;
		char c;
		bool end = true;
		string ret;

		if (strs.size() == 0)
			return ret;

		vector<string>::iterator iter;

		iter = strs.begin();
		while (iter != strs.end())
		{
			if (size < (*iter).size())
				size = (*iter).size();
			iter++;
		}

		while (end)
		{
			iter = strs.begin();
			c = (*iter)[count];
			while ((iter + 1) != strs.end())
			{
				iter++;
				if (c != (*iter)[count])
				{
					end = false;
					break;
				}
			}

			if (end)
			{
				ret += c;
				count++;
			}	

			if (count >= size)
				return ret;
		}

		return ret;
	}

	vector<vector<int>> threeSum(vector<int>& nums) {
		int target = 0, val = 0;
		vector<int> tmp;
		vector<int>::iterator beginIter, endIter;
		vector<vector<int>> ret;

		if (nums.size() <= 2)
			return ret;

		sort(nums.begin(), nums.end());
		for (int i = 0; i < nums.size() - 2; i++)
		{
			target = 0 - nums[i];
			if (target < 0)
				break;

			beginIter = nums.begin() + i + 1;
			endIter = nums.end() - 1;

			while (beginIter < endIter)
			{
				int begin = *beginIter, end = *endIter;
				val = begin + end;
				if (val == target)
				{
					tmp.push_back(nums[i]);
					tmp.push_back(*beginIter);
					tmp.push_back(*endIter);
					ret.push_back(tmp);
					tmp.clear();
					while (beginIter < endIter && *beginIter == begin)
						beginIter++;
					while (beginIter < endIter && *endIter == end)
						endIter--;

				}
				else if (val < target)
				{
					beginIter++;
				}
				else
				{
					endIter--;
				}
			}

			while (i < nums.size() - 2 && nums[i] == nums[i + 1])
				i++;
		}

		return ret;
	}

	int threeSumClosest(vector<int>& nums, int target) {
		int val = 0, begin, end, ret = 0, inTarget, gap;
		int size = nums.size();
		vector<int>::iterator beginIter, endIter;

		if (size <= 2)
			return 0;

		gap = INT_MAX;

		sort(nums.begin(), nums.end());
		for (int i = 0; i < size - 2; i++)
		{
			inTarget = target - nums[i];

			beginIter = nums.begin() + i + 1;
			endIter = nums.end() - 1;

			while (beginIter < endIter)
			{
				begin = *beginIter;
				end = *endIter;
				val = begin + end;
				if (val == inTarget)
				{
					return target;
				}
				else if (val < inTarget)
				{
					beginIter++;
				}
				else
				{
					endIter--;
				}

				if (abs(val - inTarget) < gap)
				{
					gap = abs(val - inTarget);
					ret = val + nums[i];
				}
			}
		}

		return ret;
	}


	bool isValid(string s) {
		int end = s.size();
		char *little = "()";
		char *mid = "[]";
		char *large = "{}";

		for (int i = 0; i < end; i++)
		{
			int locate = end - i - 1;
			if (s[i] == little[0])
			{
				if (s[locate] == little[1])
					continue;
				else return false;
			}
			else if (s[i] == mid[0])
			{
				if (s[locate] == mid[1])
					continue;
				else return false;
			}
			else if (s[i] == large[0])
			{
				if (s[locate] == large[1])
					continue;
				else return false;
			}
			else return false;

			end--;
		}

		return true;
	}

	int removeElement(vector<int>& nums, int val) {
		int size = nums.size();
		if (size == 0)
			return 0;

		int ptr = 0;
		int end = 0;
		for (int i = 0; i < size; i++)
		{
			if (nums[i] == val)
			{
				if (end < i)
					end = i;
				if (end == size)
					return ptr;
				while (nums[end] == val)
				{
					end++;
					if (end == size)
					{
						return ptr;
					}
				}
				int tmp = nums[ptr];
				nums[ptr] = nums[end];
				nums[end] = tmp;
				end++;
				ptr++;
			}
			else
			{
				ptr++;
			}
		}
		return ptr;
	}

	int subDivide(int dividend, int divisor, bool minus)
	{
		int i = 0, times = 1;
		int originDivisor = divisor;
		while (dividend <= divisor && dividend <= 0)
		{
			dividend -= divisor;
			divisor += divisor;
			i += times;
			times += times;
		}

		if (dividend > originDivisor)
		{
			if (minus)
				return -i;
			else
				return i;
		}
		else
		{
			if (minus)
				return -i + subDivide(dividend, originDivisor, minus);
			else
				return i + subDivide(dividend, originDivisor, minus);
		}
	}

	int divide(int dividend, int divisor) {
		int i = 0, times = 1, left = 0;
		bool minus = false;
		
		if (dividend == INT_MIN)
		{
			if (divisor == -1)
				return INT_MAX;
			else if (divisor == 1)
				return INT_MIN;
			else if (divisor == INT_MIN)
				return 1;
		}
		else if (divisor == INT_MIN)
			return 0;

		if (dividend < 0 && divisor > 0)
		{
			minus = true;
			divisor = -divisor;
		}
		else if (dividend > 0 && divisor < 0)
		{
			minus = true;
			dividend = -dividend;
		}
		dividend = dividend > 0 ? -dividend : dividend;
		divisor = divisor > 0 ? -divisor : divisor;

		return subDivide(dividend, divisor, minus);
	}

	vector<int> searchRange(vector<int>& nums, int target) {

		if (nums.size() == 0)
			return { -1, -1 };

		int begin = 0, end = nums.size() - 1;

		if (nums[begin] > target || nums[end] < target)
			return { -1, -1 };

		while (begin <= end)
		{
			if (nums[begin] == target && nums[end] == target)
			{
				vector<int> ret = { begin, end };
				return ret;
			}

			if (nums[begin] < target)
				begin++;
			if (nums[end] > target)
				end--;

			if (nums[begin] > target || nums[end] < target)
			{
				return { -1, -1 };
			}
		}

		return { -1, -1 };
	}

	int trap(vector<int>& height) 
	{
		int begin = 0, end, ret = 0;
		bool findEnd = false;

		if (height.size() <= 2)
			return 0;

		while (height[begin] == 0)
		{
			begin++;
			if (begin >= height.size() - 2)
				return 0;
		}

		while (begin < height.size() - 2)
		{
			findEnd = false;
			end = begin + 1;
			while (end < height.size())
			{
				if (height[begin] <= height[end])
				{
					findEnd = true;
					break;
				}
				end++;
			}

			if (findEnd)
			{
				std::cout << "begin = " << begin << ", end = " << end;
				for (int i = begin; i < end; i++)
				{
					ret += height[begin] - height[i];
				}
				std::cout << ", ret = " << ret << std::endl;
				begin = end;
			}
			else
			{
				begin++;
			}
		}

		return ret;
	}


	bool isMatch(string s, string p) 
	{
		int i = 0, j = 0, iStar = -1, jStar = -1, m = s.size(), n = p.size();
		while (i < m)
		{
			if (j < n && (s[i] == p[j] || p[j] == '?')) 
			{
				++i, ++j;
			}
			else if (j < n && p[j] == '*') 
			{
				iStar = i;
				jStar = j++;
			}
			else if (iStar >= 0) 
			{
				i = ++iStar;
				j = jStar + 1;
			}
			else return false;
		}
		while (j < n && p[j] == '*') 
			++j;//È¥³ý¶àÓàÐÇºÅ
		return j == n;
	}

	int jump(vector<int>& nums) {
		int size = nums.size();
		int dp[100] = { 0 };

		if (size == 1)
			return 0;
		for (int i = size - 2; i >= 0; i--)
		{
			if (nums[i] + i >= size - 1)
				dp[i] = 1;
			else
			{
				int min = size + 1;
				for (int j = 1; j <= nums[i]; j++)
				{
					if (dp[i + j] >= 0 && dp[i + j] < min)
					{
						min = dp[i + j] + 1;
					}
				}
				dp[i] = min;
			}
		}

		return dp[0];
	}


	int uniquePaths(int m, int n) {
		int x, y;
		int dp[100][100] = { 0 };

		for (x = m - 1; x >= 0; x--)
		{
			for (y = n - 1; y >= 0; y--)
			{
				if ((x == m - 1 && n - y == 2) || (y == n - 1 && m - x == 2))
					dp[x][y] = 1;
				else if (x == m - 1 && n - y > 2)
					dp[x][y] = dp[x][y + 1];
				else if (y == n - 1 && m - x > 2)
					dp[x][y] = dp[x + 1][y];
				else if (n - y >= 2 && m - x >= 2)
					dp[x][y] = dp[x + 1][y] + dp[x][y + 1];

				std::cout << "dp[" << x << "][" << y << "] = " << dp[x][y] << std::endl;
			}
		}

		return dp[0][0];
	}

};

void testAtoi()
{
	Solution A;
	int val = A.myAtoi("4193 with words");
	std::cout << val;

	getchar();
}

void testCommon()
{
	Solution A;
	vector<string> strs = { "flower", "flow", "flight" };

	std::cout << A.longestCommonPrefix(strs);
	getchar();
}

void testThreeNums()
{
	Solution A;
	vector<int> nums = {0, 0, 0};

	A.threeSum(nums);
	getchar();
}

void testThreeNumsCloset()
{
	Solution A;
	vector<int> nums = { -1, 2, 1, -4 };

	std::cout << A.threeSumClosest(nums, 1);
	getchar();
}


void testValid()
{
	Solution A;
	
	std::cout << A.isValid("()");
	getchar();
}

void testRemove()
{
	Solution A;
	vector<int> nums = { 4, 5 };
	A.removeElement(nums, 4);
}

void testDivide()
{
	Solution A;
	std::cout << A.divide(7, -3);
}

void testSearch()
{
	Solution A;
	vector<int> nums = { 5,7,7,8,8,10 };
	std::cout << A.searchRange(nums, 8)[0] << A.searchRange(nums, 8)[1];
}

void testTrap()
{
	Solution A;
	vector<int> nums = { 0,1,0,2,1,0,1,3,2,1,2,1 };
	std::cout << A.trap(nums);
}

void testMatch()
{
	Solution A;
	string s = "abefcdgiescdfimde";
	string p = "ab*cd?i*de";
	std::cout << A.isMatch(s, p);
}

void testJump()
{
	Solution A;
	vector<int> nums = { 2, 3, 1, 1, 4 };
	std::cout << A.jump(nums);
}

void testPath()
{
	Solution A;
	int m = 7, n = 3;
	std::cout << A.uniquePaths(m, n);
}

int main()
{
	//testAtoi();
	//testCommon();
	//testThreeNums();
	//testThreeNumsCloset();
	//testValid();
	//testRemove();
	//testDivide();
	//testSearch();
	//testTrap();
	//testMatch();
	//testJump();
	testPath();

	getchar();
}