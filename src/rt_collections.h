#ifndef _INC_RT_COLLECTIONS
#define _INC_RT_COLLECTIONS

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

bool 
rt__ensure_capacity(void **data, size_t *data_cap, size_t expected_cap, size_t init_cap);

/* Static array */
#define RT_ARRAY_INIT_CAP 1024
#define RT_ARRAY_PRINT_AS(arr, type) \
    do {\
        for (size_t i = 0; i < (arr)->size; ++i) {\
            printf("%d ", ((type*)(arr)->data)[i]);\
        }\
        printf("\n");\
    } while (0);

typedef struct _RT_Array {
    void *data;
    size_t elem_size;
    size_t size;
    size_t capacity;
} RT_Array;

bool rt_array_init(RT_Array *arr, size_t size, size_t elem_size);
void rt_array_free(RT_Array *arr);
bool rt_array_push(RT_Array *arr, const void *value);
bool rt_array_pop(RT_Array *arr, void *out);
bool rt_array_del(RT_Array *arr, size_t index);
bool rt_array_set(RT_Array *arr, size_t index, const void *value);
bool rt_array_get(RT_Array *arr, size_t index, void *out);
void rt_array_find_if(RT_Array *arr, void *data, bool (*callback)(const void *data, void *user_data));

static inline bool rt_array_is_empty(RT_Array *arr)
{
    return !(arr && arr->data) || arr->size == 0;
}

static inline bool rt_array_is_full(RT_Array *arr)
{
    return (arr && arr->data) && (arr->size * arr->elem_size) >= arr->capacity;
}

/* Dynamic array */
#define RT_DARRAY_INIT_CAP 1024
typedef struct _RT_DynamicArray {
    void *data;
    size_t size;
    size_t capacity;
    size_t element_size;
} RT_DynamicArray;

bool rt_darray_init(RT_DynamicArray *arr, size_t elem_size);
void rt_darray_free(RT_DynamicArray *arr);
bool rt_darray_push(RT_DynamicArray *arr, const void *value);
bool rt_darray_get(RT_DynamicArray *arr, size_t index, void *out);
bool rt_darray_set(RT_DynamicArray *arr, size_t index, const void *value);
bool rt_darray_pop(RT_DynamicArray *arr, void *out);
void rt_darray_print(RT_DynamicArray *arr);

/* D-Linked List */
#define RT_LIST_BACK 0
#define RT_LIST_FRONT 1

typedef struct _RT_ListNode {
    void *data;
    struct _RT_ListNode *next;
    struct _RT_ListNode *prev;
} RT_ListNode;

typedef struct _RT_List {
    RT_ListNode *head;
    RT_ListNode *tail;
    size_t size;
    size_t elem_size;
} RT_List;

void rt_list_init(RT_List *list, size_t elem_size);
void rt_list_free(RT_List *list);
bool rt_list_push(RT_List *list, const void *value, int where);
bool rt_list_pop(RT_List *list, void *out, int where);

static inline bool rt_list_push_back(RT_List *list, const void *value)
{
    return rt_list_push(list, value, RT_LIST_BACK);
}

static inline bool rt_list_push_front(RT_List *list, const void *value)
{
    return rt_list_push(list, value, RT_LIST_FRONT);
}


static inline bool rt_list_pop_front(RT_List *list, void *out)
{
    return rt_list_pop(list, out, RT_LIST_FRONT);
}

static inline bool rt_list_pop_back(RT_List *list, void *out)
{
    return rt_list_pop(list, out, RT_LIST_BACK);

}

bool rt_list_remove_node(RT_List *list, RT_ListNode *node);
RT_ListNode* rt_list_get_node(RT_List *list, size_t index);
bool rt_list_get(RT_List *list, size_t index, void *out);
void* rt_list_get_ptr(RT_List *list, size_t index);
bool rt_list_get_head(RT_List *list, void *out);
bool rt_list_get_tail(RT_List *list, void *out);
bool rt_list_find(RT_List *list, const void *value, void *out);
void rt_list_find_if(RT_List *list, void *data, bool (*callback)(const void *data, void *user_data));
bool rt_list_update(RT_List *list, size_t index, void *new_value);
void rt_list_foreach(RT_List *list, void *data, bool (*callback)(const void *data, void *user_data));

static inline size_t rt_list_size(RT_List *list)
{
    return list ? list->size : 0;
}

static inline bool rt_list_is_empty(RT_List *list)
{
    return (list == NULL) || (list->head == NULL && list->tail == NULL);
}

static inline RT_ListNode* rt_list_next(RT_ListNode *node)
{
    return node ? node->next : NULL;
}

static inline RT_ListNode* rt_list_prev(RT_ListNode *node)
{
    return node ? node->prev : NULL;
}

/* Stack (using RT_List) */
typedef struct _RT_Stack {
    RT_List *list;
} RT_Stack;

bool rt_stack_init(RT_Stack *stack, size_t elem_size);
void rt_stack_free(RT_Stack *stack);
bool rt_stack_push(RT_Stack *stack, const void *value);
bool rt_stack_peek(RT_Stack *stack, void *out);
bool rt_stack_pop(RT_Stack *stack, void *out);
bool rt_stack_is_empty(RT_Stack *stack);

/* Deque (using RT_List) */
typedef struct _RT_Deque {
    RT_List *list;
} RT_Deque;

bool rt_deque_init(RT_Deque *deque, size_t elem_size);
void rt_deque_free(RT_Deque *deque);
bool rt_deque_peek(RT_Deque *deque, void *out, int where);

static inline bool rt_deque_push(RT_Deque *deque, void *value, int where)
{
    return (deque && value) && rt_list_push(deque->list, value, where);;
}

static inline bool rt_deque_push_back(RT_Deque *deque, void *value)
{
    return (deque && value) && rt_list_push_back(deque->list, value);
}

static inline bool rt_deque_push_front(RT_Deque *deque, void *value)
{
    return (deque && value) && rt_list_push_front(deque->list, value);
}

static inline bool rt_deque_pop(RT_Deque *deque, void *out, int where)
{
    return deque && rt_list_pop(deque->list, out, where);
}

static inline bool rt_deque_pop_front(RT_Deque *deque, void *out)
{
    return deque && rt_list_pop_front(deque->list, out);
}

static inline bool rt_deque_pop_back(RT_Deque *deque, void *out)
{
    return deque && rt_list_pop_back(deque->list, out);
}

static inline bool rt_deque_peek_front(RT_Deque *deque, void *out)
{
    return deque && rt_deque_peek(deque, out, RT_LIST_FRONT);
}

static inline bool rt_deque_peek_back(RT_Deque *deque, void *out)
{
    return deque && rt_deque_peek(deque, out, RT_LIST_BACK);
}

static inline bool rt_deque_is_empty(RT_Deque *deque)
{
    return !deque || rt_list_is_empty(deque->list);
}

/* Ring Buffer */
#define RT_RBUFFER_INIT_CAP 1024
typedef struct _RT_RingBuffer {
    void *data;
    size_t elem_size;
    size_t capacity;
    size_t capacity_items;
    size_t size;
    size_t head;
    size_t tail;
} RT_RingBuffer;

bool rt_rbuffer_init(RT_RingBuffer *buffer, size_t size, size_t elem_size);
void rt_rbuffer_free(RT_RingBuffer *buffer);
bool rt_rbuffer_read(RT_RingBuffer *buffer, void *out);
bool rt_rbuffer_write(RT_RingBuffer *buffer, void *value);
bool rt_rbuffer_peek(RT_RingBuffer *buffer, void *out);

static inline bool rt_rbuffer_is_empty(RT_RingBuffer *buffer)
{
    return !(buffer && buffer->data);
}

static inline bool rt_rbuffer_is_full(RT_RingBuffer *buffer)
{
    return buffer->size >= buffer->capacity_items;
}

/* File Buffer */
#define RT_FBUFFER_INIT_CAP 1024
typedef struct _RT_FileBuffer {
    uint8_t *data;
    size_t capacity;
    size_t size;
} RT_FileBuffer;

bool rt_fbuffer_init(RT_FileBuffer *buffer);
void rt_fbuffer_free(RT_FileBuffer *buffer);
size_t rt_fbuffer_read_file(RT_FileBuffer *buffer, const char *file_path);
size_t rt_fbuffer_write_file(RT_FileBuffer *buffer, const char *file_path);
bool rt_fbuffer_set(RT_FileBuffer *buffer, const void *data, size_t size);
bool rt_fbuffer_clone(RT_FileBuffer *dst, const RT_FileBuffer *src);
bool rt_fbuffer_copy(RT_FileBuffer *dst, const void *src, size_t size);
uint8_t *rt_fbuffer_at(const RT_FileBuffer *buffer, size_t offset);
void rt_fbuffer_clear(RT_FileBuffer *buffer);
bool rt_fbuffer_reserve(RT_FileBuffer *buffer, size_t size);
bool rt_fbuffer_resize(RT_FileBuffer *buffer, size_t size);
bool rt_fbuffer_fill(RT_FileBuffer *buffer, uint8_t byte);
bool rt_fbuffer_fill_ex(RT_FileBuffer *buffer, uint8_t byte, size_t size);

static inline bool rt_fbuffer_is_empty(const RT_FileBuffer *buffer)
{
    return !buffer || buffer->size == 0;
}

/* Hash Table (separate chaining) */
#define RT_HASHTABLE_LOADFACTOR_MAX 0.79 // TODO implement this
#define RT_HASHTABLE_INIT_BUCKETS_COUNT 256
typedef struct _RT_HTEntry {
    char *key;
    void *value;
    size_t value_size;
    struct _RT_HTEntry *next;
} RT_HTEntry;

typedef struct _RT_HTBucket {
    RT_HTEntry* head;
    size_t count;
} RT_HTBucket;

typedef struct _RT_HashTable {
    RT_HTBucket *buckets;
    size_t buckets_count;
    size_t size;
    size_t (*hash_func)(const char *key);
} RT_HashTable;

bool rt_hashtable_init(RT_HashTable *table, size_t buckets_num);
void rt_hashtable_free(RT_HashTable *table);
bool rt_hashtable_insert(RT_HashTable *table, const char *key, void *value, size_t value_size);
bool rt_hashtable_get(RT_HashTable *table, const char *key, void *out, size_t out_size);
bool rt_hashtable_remove(RT_HashTable *table, const char *key);
bool rt_hashtable_contains(RT_HashTable *table, const char *key);

static inline size_t rt__hashtable_hash(const char *key)
{
    if (!key) return 0;
    
    size_t hash = 5381;
    int c;
    
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

static inline size_t rt__hashtable_bucket_getindex(RT_HashTable *table, const char *key)
{
    return rt__hashtable_hash(key) % table->buckets_count;
}

static inline bool rt_hashtable_is_empty(RT_HashTable *table)
{
    return !table || table->size == 0;
}

static inline RT_HTEntry *rt__hashtable_entry_alloc(size_t key_size, size_t value_size)
{
    RT_HTEntry *entry = malloc(sizeof(RT_HTEntry));
    if (!entry) goto allocation_failure_entry;

    entry->key = malloc(key_size);
    if (!entry->key) goto allocation_failure_key;

    entry->value = malloc(value_size);
    if (!entry->value) goto allocation_failure_value;
    
    entry->next = NULL;
    entry->value_size = value_size;
    return entry;

    allocation_failure_value:
        free(entry->key);
    allocation_failure_key:
        free(entry);
    allocation_failure_entry:
        return NULL;
}

static inline bool rt__hashtable_update_entry_value(RT_HTEntry *entry, void *value, size_t vsize)
{
    void *new_value = entry->value 
        ? realloc(entry->value, vsize)
        : malloc(vsize);
    if (!new_value) return false;

    memcpy_s(new_value, vsize, value, vsize);
    entry->value = new_value;
    entry->value_size = vsize;

    return true;
}

static inline RT_HTEntry* rt__hashtable_get_entry(RT_HashTable *table, const char *key)
{
    size_t bi = rt__hashtable_bucket_getindex(table, key);
    RT_HTEntry *entry = (&table->buckets[bi])->head;

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry;
        }

        entry = entry->next;
    }

    return NULL;
}

static inline RT_HTBucket* rt_hashtable_get_bucket(RT_HashTable *table, const char *key)
{
    const size_t bi = rt__hashtable_bucket_getindex(table, key);
    return &table->buckets[bi];;
}

static inline RT_HTEntry* rt_hashtable_entry_first(RT_HTBucket *bucket)
{
    return bucket ? bucket->head : NULL;
}

static inline RT_HTEntry* rt_hashtable_entry_next(RT_HTEntry *entry)
{
    return entry ? entry->next : NULL;
}

#endif // _INC_RT_COLLECTIONS