#include "array.h"

int main(void) {
  {
    array(int) integers = NULL; 
    array_push(integers, 0x10);
    array_push(integers, 123);
    array_push(integers, 567);
    array_push(integers, 987);

    array_sort(integers);
    int key = 987;
    int* result = array_find(integers, key);
    if (result == NULL) {
      printf("Value for key %d not found!\n", key);
    } else {
      print(*result);
    }

    array_set(integers, 3, 010);

    result = array_find(integers, key);
    if (result == NULL) {
      printf("Value for key %d not found!\n", key);
    } else {
      print(*result);
    }

    array_pop(integers);

    array_foreach(integers, it) {
      print(*it);
    }

    array(int) mapped = NULL;
    array_map(integers, mapped, it * 2);
    
    array_foreach(mapped, it) {
      print(*it);
    }

    array(int) filtered = NULL;
    array_filter(integers, filtered, it % 2 == 0);

    array_foreach(filtered, it) {
      print(*it);
    }

    int sum = 0; 
    array_reduce(integers, sum, sum + it);
    print("The sum is:");
    print(sum);
  }
  // integers goes of scope, is automatically freeed.
  {
    array(float) floats = NULL; 
    array_push(floats, 0x10);
    array_push(floats, 123);
    array_push(floats, 567);
    array_push(floats, 987);

    array_set(floats, 3, 010);

    array_pop(floats);

    array_sort(floats);

    for (size_t i = 0; i < array_count(floats); ++i) {
      print(array_get(floats, i));
    }
  }
  // floats goes of scope, is automatically freeed.
  {
    array(double) doubles = NULL; 
    array_push(doubles, 0x10);
    array_push(doubles, 123);
    array_push(doubles, 567);
    array_push(doubles, 987);

    array_set(doubles, 3, 010);

    array_sort(doubles);

    array_pop(doubles);

    for (size_t i = 0; i < array_count(doubles); ++i) {
      print(array_get(doubles, i));
    }
  }
  // doubles goes of scope, is automatically freeed.
  {
    typedef struct {
      long double x;
    } Big;
    array(Big) bigs = NULL;
    Big a = { .x = 2.718281828L };
    array_push(bigs, a); 
    array_push(bigs, a); 
    array_push(bigs, a); 

    array_foreach(bigs, it) {
      printf("%Lf\n", it->x); 
    }
    printf("Header size: %zu\n", sizeof(Header));
    printf("AlignedHeader size: %zu\n", sizeof(AlignedHeader));
    printf("long double align: %zu\n", _Alignof(long double));
  }

  return 0;
}
