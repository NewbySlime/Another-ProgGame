#ifndef MEMORY_HEADER
#define MEMORY_HEADER

bool memapi_initialize(size_t max_size);
void *memapi_get_heap_address();

void *memapi_bypass_memcpy(void *_src, const void *_dst, size_t datalen);
void *memapi_bypass_malloc(size_t datalen);
void *memapi_bypass_malloc_alligned(size_t datalen, size_t allign);
void *memapi_bypass_realloc(void *block, size_t new_size);
void memapi_bypass_free(void *addr);

#endif