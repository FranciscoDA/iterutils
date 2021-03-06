
#include "../zipped_range.h"
#include "../chained_range.h"
#include "../strided_range.h"
#include "../cycle_range.h"
#include "../ncycle_range.h"
#include "../alternated_range.h"
#include "../series_range.h"
#include "../repeat_range.h"

#include <iostream>
#include <vector>
#include <sstream>

using namespace iterutils;

struct HeavyObject {
	int i;
	HeavyObject(const HeavyObject& other) { throw std::exception(); }
	HeavyObject(int _i) : i(_i) {}
	~HeavyObject() { i = 0; }
};

int main() {
	std::vector<HeavyObject> v1, v2, v3;
	v1.reserve(5);
	for (int i = 0; i < v1.capacity(); ++i)
		v1.emplace_back(i);
	v2.reserve(9);
	for (int i = 0; i < v2.capacity(); ++i)
		v2.emplace_back(i*10);
	v3.reserve(3);
	for(int i = 0; i < v3.capacity(); ++i)
		v3.emplace_back(i*5);

	std::cout << "Test 1: Structured binding with zipped_range" << std::endl;
	int product = 0;
	for (auto [a,b] : zipped_range(v1,v2))
		product += a.i*b.i;
	std::cout << "vector product from zipped range (shortest): " << product << std::endl;

	std::cout << "Test 2: Concatenation of two ranges" << std::endl;
	int sum = 0;
	int count = 0;
	for (auto& x : chained_range(v1,v2)) {
		sum += x.i;
		count += 1;
	}
	std::cout << "Average value: " << double(sum)/double(count) << std::endl;

	std::cout << "Test 3: Strided range" << std::endl;
	sum = 0;
	for (auto& x : strided_range(v2, 0, 4))
		sum += x.i;
	std::cout << "Trace of v2 as if it were a 3x3 matrix: " << sum << std::endl;

	std::cout << "Test 4: Nesting special ranges" << std::endl;
	std::cout << "v2 = " << std::endl;
	for(auto [a,b,c] : zipped_range(strided_range(v2, 0, 3), strided_range(v2, 1, 3), strided_range(v2, 2, 3))) {
		std::cout << a.i << " " << b.i << " " << c.i << std::endl;
	}

	std::cout << "Test 5: Three range concatenation" << std::endl;
	std::cout << "v1..v2..v3 = ";
	for(auto& x : chained_range(v1, v2, v3))
		std::cout << x.i << ",";
	std::cout << std::endl;

	std::cout << "Test 6: Cycle iterator" << std::endl;
	std::cout << "v1..v1..v1 = ";
	for(auto& x : ncycle_range(v1, 3)) {
		std::cout << x.i << ",";
	}
	std::cout << std::endl;

	std::cout << "Test 7: alternated range" << std::endl;
	std::cout << "alternated(v1,v2) = ";
	int i = 0;
	for (auto& x : alternated_range(v1,v2)) {
		std::cout << x.i << ",";
		++i;
	}
	std::cout << std::endl;

	for (auto& e : alternated_range(std::array<std::string,4>{"hello", "world", "foo", "bar"}, repeat_range(std::string(".")))) {
		std::cout << e;
	}
	std::cout << std::endl;
}

