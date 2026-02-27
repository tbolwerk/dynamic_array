# dynamic_array

A single-header generic dynamic array library for C, with automatic memory management, sorting, searching, and functional utilities.

---

## Setup

Copy `array.h` into your project and include it:

```c
#include "array.h"
```

Compile with debug logging enabled:
```bash
gcc -DDEBUG -o program main.c
```

---

## Quick Reference

| Macro / Function         | Description                              |
|--------------------------|------------------------------------------|
| `array(T)`               | Declare an auto-freed array of type `T`  |
| `array_push(a, item)`    | Append an item                           |
| `array_pop(a)`           | Remove and return last item              |
| `array_count(a)`         | Number of items                          |
| `array_capacity(a)`      | Current allocated capacity               |
| `array_elem_size(a)`     | Size of each element in bytes            |
| `array_sort(a)`          | Sort in ascending order                  |
| `array_find(a, key)`     | Binary search (array must be sorted)     |
| `array_foreach(a, it)`   | Iterate with pointer `it`                |
| `array_map(src, dst, expr)`    | Map expression over array          |
| `array_filter(src, dst, expr)` | Filter by expression               |
| `array_reduce(src, acc, expr)` | Reduce to accumulator              |
| `print(x)`               | Print any supported type with newline    |

---

## Usage

### Declaring an array

```c
array(int) nums = NULL;
```

`array(T)` expands to an `autofree T*` — a plain pointer that is automatically freed when it goes out of scope. No manual `free()` needed.

### Pushing items

```c
array_push(nums, 10);
array_push(nums, 20);
array_push(nums, 30);
```

The array is lazily initialized on the first push. It grows automatically as needed.

### Accessing elements

```c
nums[0];                        // direct index
array_count(nums);              // number of elements
```

### Popping

```c
array_pop(nums);                // removes 
```

Popping from an empty array does nothing. 

### Automatic cleanup

```c
{
    array(int) nums = NULL;
    array_push(nums, 1);
    array_push(nums, 2);
}
// nums is automatically freed here
```

No `free()` required. Works correctly with early returns too.

---

## Sorting and Searching

### Sorting

```c
array_sort(nums);               // sorts ascending in place
```

Supported types out of the box: `int`, `float`, `double`.

### Searching

```c
array_sort(nums);               // array must be sorted first
int* result = array_find(nums, 20);

if (result == NULL) {
    printf("not found\n");
} else {
    printf("found: %d\n", *result);
}
```

`array_find` uses binary search (`bsearch`). It requires the array to be sorted first. It returns a pointer to the element, or `NULL` if not found. If there are duplicates, the returned pointer is not guaranteed to be the first occurrence.

### Adding a new type

```c
// 1. define the comparator
define_cmp(uint32_t)

// 2. add to _Generic in array_cmp
#define array_cmp(a) (_Generic((__typeof__(*(a))*)(a), \
    int*:      cmp_int,                                \
    uint32_t*: cmp_uint32_t                            \
))
```

---

## Functional Macros

All functional macros use `it` as the current element name.

### map

```c
array(int) src = NULL;
array_push(src, 1);
array_push(src, 2);
array_push(src, 3);

array(int) doubled = NULL;
array_map(src, doubled, it * 2);
// doubled = {2, 4, 6}

array(float) roots = NULL;
array_map(src, roots, sqrtf(it));
// roots = {1.0, 1.41, 1.73}
```

### filter

```c
array(int) evens = NULL;
array_filter(src, evens, it % 2 == 0);
// evens = {2}
```

### reduce

```c
int sum = 0;
array_reduce(src, sum, sum + it);
// sum = 6

int max = src[0];
array_reduce(src, max, it > max ? it : max);
```

### foreach

```c
array_foreach(nums, it) {
    printf("%d\n", *it);     // it is a pointer
}
```

---

## Printing

`print(x)` dispatches to the correct `printf` format based on type:

```c
print(42);        // %d
print(3.14f);     // %f
print(3.14);      // %lf
print("hello");   // %s
```

---

## Error Handling

Errors print to `stderr` and call `exit(1)`:

```c
void throw_error(char* message);
```

Triggered on:
- `malloc` returning `NULL` — `"Out of memory"`
- `realloc` returning `NULL` — `"Out of memory"`

---

## Debug Mode

Compile with `-DDEBUG` to log frees:

```bash
gcc -DDEBUG -o program main.c
```

Output:
```
freeing array at 0x... (count=4)
```

---

## Internals

The array uses a **header-prefixed allocation** — a single contiguous block where the `Header` struct lives just before the data pointer:

```
[ Header: count | capacity | elem_size | elem0 | elem1 | ... ]
                                        ^
                                        array pointer
```

This means the array pointer is a plain `T*` that can be indexed, passed to `qsort`, `bsearch`, and any standard C function directly.

`array_header(array)` recovers the header by subtracting one `Header` from the pointer.

The array grows at **2** capacity ensuring its growing by integer multiplication.

---

## Limitations

- `autofree` and `__typeof__` require GCC or Clang — not supported on MSVC
- `array_sort` and `array_find` require the type to be registered in `array_cmp`'s `_Generic`
- `array_find` does not guarantee returning the first element when duplicates exist
- Not thread-safe
