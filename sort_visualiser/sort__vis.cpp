#include <SDL.h>
#include <iostream>
#include <random>
#include <ranges>
#include <algorithm>

int main() {
	std::random_device rd;
	std::mt19937 gen(rd);
	std::uniform_int_distribution<int> d(1, 99);
	std::vector<int> v;


}