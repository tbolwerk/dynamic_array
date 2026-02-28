#include "assert.h"
#include "stdalign.h"
#include "stddef.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

#ifndef ARRAY_H
#define ARRAY_H
// To test growing array.
#define INIT_CAPACITY 100

typedef struct {
  size_t count;
  size_t capacity;
  size_t elem_size;
} Header;

typedef struct {
  Header header;
  _Alignas(max_align_t) unsigned char array[];
} AlignedHeader;

static inline AlignedHeader* aligned_array_header(void* array) {
    return (AlignedHeader*)((char*)array - offsetof(AlignedHeader, array));
}

static inline Header* array_header(void* array) {
    return &aligned_array_header(array)->header;
}

static inline size_t array_count(void* array) {
  return array_header(array)->count;
}

static inline size_t array_elem_size(void* array) {
  return array_header(array)->elem_size;
}

static inline size_t array_capacity(void* array) {
  return array_header(array)->capacity;
}

static inline void* throw_error(char* message) {
#ifdef DEBUG
  fprintf(stderr, "%s\n", message);
#endif
  return NULL;
}

void* array_init(size_t elem_size) {
  AlignedHeader* aligned_header = malloc(elem_size * INIT_CAPACITY + sizeof(AlignedHeader));
  if (aligned_header == NULL) return throw_error("Out of memory");

  Header* header = &aligned_header->header;
  header->count = 0;
  header->capacity = INIT_CAPACITY;
  header->elem_size = elem_size;

  return aligned_header->array;
}

void* array_grow(void *array) {
  size_t elem_size = array_header(array)->elem_size;
  size_t new_capacity = array_header(array)->capacity * 2;

  if (new_capacity < array_capacity(array)) return throw_error("Capacity overflow");
  if (elem_size > SIZE_MAX / new_capacity) return throw_error("Size overflow");

  AlignedHeader* aligned_header = aligned_array_header(array);
  aligned_header = realloc(aligned_header, elem_size * new_capacity + sizeof(AlignedHeader));
  if (aligned_header == NULL) return throw_error("Out of memory");

  aligned_header->header.capacity = new_capacity;

  return aligned_header->array;
}

void array_pop(void* array) {
  if (array_count(array) == 0) return;
  array_header(array)->count --;
}

#ifdef DEBUG
#define array_get(array, i) \
    (*(assert(i < array_count(array)), &(array)[i]))
#else
#define array_get(array, i) (array)[i]
#endif
    
#ifdef DEBUG
#define array_set(array, index, item)      \
  do {                                     \
    assert(index < array_count(array));    \
    array[index] = item;                   \
  } while(0)                               
#else
#define array_set(array, index, item) array[index] = item
#endif

#define array_push(array, item)                                                   \
  do {                                                                            \
    if ((array) == NULL) (array) = array_init(sizeof(*(array)));                  \
    if (array_count(array) >= array_capacity(array)) (array) = array_grow(array); \
    (array)[array_header(array)->count++] = (item);                               \
  } while(0)

void array_free(void* ptr) {
    void* array = *(void**)ptr;
    if (array == NULL) return;
#ifdef DEBUG
    printf("freeing array at %p (count=%zu)\n", array, array_count(array));
#endif
    free(aligned_array_header(array));
    *(void**)ptr = NULL;
}

#define autofree __attribute__((cleanup(array_free)))

#define array(T) autofree T*

#define print(x) _Generic((x), \
  int:    printf("%d\n", x),   \
  float:  printf("%f\n", x),   \
  double: printf("%lf\n",x),   \
  char*:  printf("%s\n", x)    \
)

#define define_cmp(T) \
    int cmp_##T(const void* a, const void* b) { \
        T _a = *(T*)a, _b = *(T*)b;             \
        return (_a > _b) - (_a < _b);           \
    }

#define TYPES        \
    X(int)           \
    X(float)         \
    X(double)        \

#define X(T) define_cmp(T)
TYPES
#undef X

#define array_cmp(a) (_Generic((__typeof__(*(a))*)(a),  \
    int*:    cmp_int,                                   \
    float*:  cmp_float,                                 \
    double*: cmp_double                                 \
))

#define array_sort(array) \
  qsort(array, array_count(array), array_elem_size(array), array_cmp(array));
#define array_find(array, key) \
    bsearch(&(__typeof__(*(array))){key}, array, array_count(array), array_elem_size(array), array_cmp(array))

#define array_foreach(a, it) \
  for (__typeof__(*(a))* it = (a); it < (a) + array_count(a); it++)


// array_map(src, dest, it * 2)
#define array_map(src, dest, expr)                              \
    do {                                                        \
        array_foreach(src, _it) {                               \
            __typeof__(*(src)) it = *_it;                       \
            array_push(dest, expr);                             \
        }                                                       \
    } while(0)

// array_filter(src, dest, it % 2 == 0)
#define array_filter(src, dest, expr)                           \
    do {                                                        \
        array_foreach(src, _it) {                               \
            __typeof__(*(src)) it = *_it;                       \
            if (expr) array_push(dest, it);                     \
        }                                                       \
    } while(0)

// int sum = 0; array_reduce(nums, sum, sum + it);
#define array_reduce(src, acc, expr)                            \
    do {                                                        \
        array_foreach(src, _it) {                               \
            __typeof__(*(src)) it = *_it;                       \
            acc = expr;                                         \
        }                                                       \
    } while(0)
#endif // ARRAY_H

