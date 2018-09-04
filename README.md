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
Concatenates multiple subranges sequentially. Requires that all the subranges have the same element\_type:
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

### ref\_range
Takes multiple lvalue references of the same type as arguments and returns an iterator that will yield them back in order
```cpp
std::vector<int> vec1 {1,2,3};
std::vector<int> vec2 {10, 20, 30};
std::vector<int> vec3 {1, 1, 1};
for (auto& v : ref_range(vec1, vec2, vec3)) {
  // x is a reference to either vec1, vec2 or vec3
  // iteration | x
  // 1         | vec1
  // 2         | vec2
  // 3         | vec3
}
```
