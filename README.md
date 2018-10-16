# iterutils
Collection of several header-only range iterator implementations for C++17

### zipped\_range

Iterates multiple subranges simultaneously, stopping at the shortest range. Compatible with structured binding
```cpp
std::vector<int> vec1 {1, 2, 3};
std::vector<int> vec2 {10, 20, 30};
for (auto [x,y] : zipped_range(vec1, vec2)) {
  // x and y are reference to elements of vec1 and vec2, respectively
  // iteration | x | y
  // 1         | 1 | 10
  // 2         | 2 | 20
  // 3         | 3 | 30
}
```

### chained\_range
Concatenates multiple subranges sequentially. Requires that all the subranges have the same value\_type:
```cpp
std::vector<int> vec1 {1, 2, 3};
std::vector<int> vec2 {10, 20, 30};
for (auto& x : chained_range(vec1, vec2)) {
  // x is a reference to an element of either vec1, vec2 or vec3
  // iteration | x 
  // 1         | 1
  // 2         | 2
  // 3         | 3
  // 4         | 10
  // 5         | 20
  // 6         | 30
}
```

### strided\_range
Takes a range, an offset and a stride value. Yields one element from range starting from 'offset' for every 'stride' elements. Requires the subrange to have a random access iterator.
```cpp
std::vector<int> vec1 {1, 2, 3, 10, 20, 30};
for (auto& x : strided_range(vec1, 1, 2)) {
  // x is a reference to an element of v1
  // iteration | x
  // 1         | 2
  // 2         | 10
  // 3         | 30
}
```

### alternated\_range
Alternates between elements of multiple ranges in order. Requires that all the subranges have the same value\_type:
```cpp
std::vector<int> vec1 {1,2,3};
std::vector<int> vec2 {10, 20, 30};
for (auto& x : alternated_range(v1, v2)) {
	// x is a reference to an element of v1 or v2
	// iteration | x
	// 1         | 1
	// 2         | 10
	// 3         | 2
	// 4         | 20
	// 5         | 3
	// 6         | 30
}
```

