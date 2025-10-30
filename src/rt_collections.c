#include "rt_collections.h"

bool 
rt__ensure_capacity(void **data, size_t *data_cap, size_t expected_cap, size_t init_cap)
{
    if (*data_cap < expected_cap) {
        size_t c = *data_cap ? *data_cap : init_cap;
        while (c < expected_cap) c *= 2;

        void *p = *data ? realloc(*data, c) : malloc(c);
        if (!p) return false;

        *data = p;
        *data_cap = c;
    }

    return true;
}

/* IMPLEMENTATION */
bool rt_array_init(RT_Array *arr, size_t size, size_t elem_size)
{
    if (!arr || size == 0 || elem_size == 0 || arr->data) return false;

    arr->capacity = size > 0 ? size * elem_size : RT_ARRAY_INIT_CAP;
    arr->data = malloc(arr->capacity);
    if (!arr->data) return false;
    arr->elem_size = elem_size;
    arr->size = 0;

    return true;
}

void rt_array_free(RT_Array *arr)
{
    if (!arr || !arr->data) return;
    free(arr->data);
    arr->data = NULL;
    arr->capacity = 0;
    arr->size = 0;
    arr->elem_size = 0;
}

bool rt_array_push(RT_Array *arr, const void *value)
{
    if (!arr || rt_array_is_full(arr)) return false;

    memcpy((char*)arr->data + (arr->size * arr->elem_size), value, arr->elem_size);
    arr->size++;

    return true;
}

bool rt_array_pop(RT_Array *arr, void *out)
{
    if (!arr || arr->size == 0) return false;

    if (out) memcpy(out, (char*)arr->data + (arr->size * arr->elem_size), arr->elem_size);
    arr->size--;

    return true;
}

bool rt_array_del(RT_Array *arr, size_t index)
{
    if (!arr || rt_array_is_empty(arr) || index >= arr->size) return false;
    
    char *src = (char*)arr->data + ((index + 1) * arr->elem_size);
    char *dst = (char*)arr->data + (index * arr->elem_size);
    memmove(src, dst, arr->size * arr->elem_size);
    arr->size--;

    return true;
}

bool rt_array_set(RT_Array *arr, size_t index, const void *value)
{
    if (!arr || rt_array_is_empty(arr) || index >= arr->size || !value) return false;

    memcpy((char*)arr->data + (index * arr->elem_size), value, arr->elem_size);

    return false;
}

bool rt_array_get(RT_Array *arr, size_t index, void *out)
{
    if (!arr || rt_array_is_empty(arr) || index >= arr->size || !out) return false;
    memcpy(out, (char*)arr->data + (index * arr->elem_size), arr->elem_size);
    return true;
}

void rt_array_find_if(RT_Array *arr, void *data, bool (*callback)(const void *data, void *user_data))
{
    if (!(arr && callback) || arr->size == 0) return;
    
    for (size_t i = 0; i < arr->size; ++i) {
        if (callback((char*)arr->data + (i * arr->elem_size), data)) return;
    }
}

bool rt_darray_init(RT_DynamicArray *arr, size_t element_size)
{
    if (!arr || element_size == 0) return false;

    arr->data = malloc(RT_DARRAY_INIT_CAP);
    if (!arr->data) return false;

    arr->capacity = RT_DARRAY_INIT_CAP;
    arr->size = 0;
    arr->element_size = element_size;

    return true;
}

void rt_darray_free(RT_DynamicArray *arr)
{
    if (!arr) return;
    free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
    arr->element_size = 0;
}

bool rt_darray_push(RT_DynamicArray *arr, const void *value)
{
    if (!arr || !value) return false;

    if (arr->size >= arr->capacity) {
        size_t new_cap = arr->capacity * 2;
        void *new_data = realloc(arr->data, new_cap * arr->element_size);
        if (!new_data) return false;
        arr->data = new_data;
        arr->capacity = new_cap;
    }

    memcpy((char*)arr->data + arr->size * arr->element_size, value, arr->element_size);
    arr->size++;

    return true;
}

bool rt_darray_get(RT_DynamicArray *arr, size_t index, void *out)
{
    if (!arr || !out || index >= arr->size) return false;
    memcpy(out, (char*)arr->data + index * arr->element_size, arr->element_size);
    return true;
}

bool rt_darray_set(RT_DynamicArray *arr, size_t index, const void *value)
{
    if (!arr || !value || index >= arr->size) return false;
    memcpy((char*)arr->data + index * arr->element_size, value, arr->element_size);
    return true;
}

bool rt_darray_pop(RT_DynamicArray *arr, void *out)
{
    if (!arr || arr->size == 0 || !out) return false;
    arr->size--;
    memcpy(out, (char*)arr->data + arr->size * arr->element_size, arr->element_size);

    if (arr->size > 0 && arr->size <= arr->capacity / 4 && arr->capacity > RT_DARRAY_INIT_CAP) {
        size_t new_cap = arr->capacity / 2;
        void *new_data = realloc(arr->data, new_cap * arr->element_size);
        if (new_data) {
            arr->data = new_data;
            arr->capacity = new_cap;
        }
    }
    return true;
}

void rt_darray_print(RT_DynamicArray *arr)
{
    for (size_t i = 0; i < arr->size; ++i) {
        int elem;
        rt_darray_get(arr, i, &elem);
        printf("%d ", elem);
    }
    printf("\n");
}

void rt_list_init(RT_List *list, size_t elem_size)
{
    if (!list || elem_size == 0) return;

    list->head = NULL;
    list->tail = NULL;
    list->elem_size = elem_size;
    list->size = 0;
}

void rt_list_free(RT_List *list)
{
    if (!list || list->size == 0) return;

    RT_ListNode *node = list->head;
    RT_ListNode *next;
    while (node) {
        next = node->next;
        free(node->data);
        free(node);
        node = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

bool rt_list_push(RT_List *list, const void *value, int where)
{
    if (!list || !value) return false;

    RT_ListNode *node = malloc(sizeof(RT_ListNode));
    if (!node) return false;
    node->data = malloc(list->elem_size);
    if (!node->data) {
        free(node);
        return false;
    }
    node->next = NULL;
    node->prev = NULL;
    memcpy(node->data, value, list->elem_size);

    if (where == RT_LIST_FRONT) {
        node->next = list->head;
        if (list->head) list->head->prev = node;
        list->head = node;
        if (list->size == 0) list->tail = node;
    } else { // RT_LIST_BACK by default
        if (list->size == 0) {
            list->head = node;
            list->tail = node;
        } else {
            node->prev = list->tail;
            list->tail->next = node;
            list->tail = node;
        }
    }
    list->size++;
    return true;
}

bool rt_list_pop(RT_List *list, void *out, int where)
{
    if (!list || list->size == 0) return false;

    RT_ListNode *node = (where == RT_LIST_FRONT) ? list->head : list->tail;
    
    if (out) memcpy(out, node->data, list->elem_size);

    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;

    if (where == RT_LIST_FRONT) {
        list->head = node->next;
    } else {
        list->tail = node->prev;
    }

    free(node->data);
    free(node);
    list->size--;
    return true;
}

bool rt_list_remove_node(RT_List *list, RT_ListNode *node)
{
    if (!list || !node || list->size == 0) return false;

    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;

    if (node == list->head) list->head = node->next;    
    if (node == list->tail) list->tail = node->prev;

    free(node->data);
    free(node);
    list->size--;    
    return true;
}

RT_ListNode *rt_list_get_node(RT_List *list, size_t index)
{
    if (!list || index >= list->size) return NULL;

    RT_ListNode *cur_node = NULL;

    if (index > list->size / 2) {
        cur_node = list->tail;
        for (size_t i = list->size - 1; cur_node && i != index; --i) {
            cur_node = cur_node->prev;
        }
    } else {
        cur_node = list->head;
        for (size_t i = 0; cur_node && i != index; ++i) {
            cur_node = cur_node->next;
        }
    }
    
    return cur_node;
}

bool rt_list_get(RT_List *list, size_t index, void *out)
{
    if (!list || !out || index >= list->size) return false;

    RT_ListNode *node = rt_list_get_node(list, index);
    if (!node) return false;

    memcpy(out, node->data, list->elem_size);
    return true;
}

void *rt_list_get_ptr(RT_List *list, size_t index)
{
    if (!list || index >= list->size) return NULL;
    RT_ListNode *node = rt_list_get_node(list, index);
    return node ? node->data : NULL;
}

bool rt_list_get_head(RT_List *list, void *out)
{
    if (!list || list->size == 0 || !out) return false;
    memcpy(out, list->head->data, list->elem_size);
    return true;
}

bool rt_list_get_tail(RT_List *list, void *out)
{
    if (!list || list->size == 0 || !out) return false;
    memcpy(out, list->tail->data, list->elem_size);
    return true;
}

bool rt_list_find(RT_List *list, const void *value, void *out)
{
    if (!list || list->size == 0) return false;

    RT_ListNode *node;
    for (node = list->head; node; node = node->next) {
        if (memcmp(value, node->data, list->elem_size) == 0) {
            memcpy(out, node->data, list->elem_size);
            return true;
        }
    }

    return false;
}

void rt_list_find_if(RT_List *list, void *data, bool (*callback)(const void *data, void *user_data))
{
    if (!list || !callback) return;

    for (RT_ListNode *node = list->head; node; node = node->next) {
        if (callback(node->data, data)) return;
    }
}

bool rt_list_update(RT_List *list, size_t index, void *new_value)
{
    if (!list || list->size == 0 || index >= list->size || !new_value) return false;

    void *data = rt_list_get_ptr(list, index);
    if (!data) return false;

    memcpy(data, new_value, list->elem_size);
    return true;
}

void rt_list_foreach(RT_List *list, void *data, bool (*callback)(const void *data, void *user_data))
{
    if (!list || list->size == 0) return;

    RT_ListNode *node = list->head;
    while (node) {
        RT_ListNode *next = node->next;
        if (callback(node->data, data)) return;
        node = next;
    }
}

bool rt_stack_init(RT_Stack *stack, size_t elem_size)
{
    if (!stack || elem_size == 0) return false;

    stack->list = malloc(sizeof(RT_List));
    if (!stack->list) return false;

    rt_list_init(stack->list, elem_size);
    return true;
}

void rt_stack_free(RT_Stack *stack)
{
    if (!stack || !stack->list) return;
    rt_list_free(stack->list);
    free(stack->list);
}

bool rt_stack_push(RT_Stack *stack, const void *value)
{
    if (!stack || !stack->list || !value) return false;
    return rt_list_push_back(stack->list, value);
}

bool rt_stack_peek(RT_Stack *stack, void *out)
{
    if (!stack || !stack->list || rt_list_is_empty(stack->list)) return false;
    memcpy(out, stack->list->tail->data, stack->list->elem_size);
    return true;
}

bool rt_stack_pop(RT_Stack *stack, void *out)
{
    if (!stack || !stack->list || rt_list_is_empty(stack->list)) return false;
    return rt_list_pop_back(stack->list, out);
}

bool rt_stack_is_empty(RT_Stack *stack)
{
    return !(stack && stack->list) || rt_list_is_empty(stack->list);
}

bool rt_deque_init(RT_Deque *deque, size_t elem_size)
{
    if (!deque || elem_size == 0) return false;

    deque->list = malloc(sizeof(RT_List));
    if (!deque->list) return false;
    rt_list_init(deque->list, elem_size);

    return true;
}

void rt_deque_free(RT_Deque *deque)
{
    if (!deque) return;

    rt_list_free(deque->list);
    free(deque->list);
    deque->list = NULL;
}

bool rt_deque_peek(RT_Deque *deque, void *out, int where)
{
    if (!deque) return false;
    if (where == RT_LIST_BACK) {
        return rt_list_get_tail(deque->list, out);
    }
    return rt_list_get_head(deque->list, out);
}

bool rt_rbuffer_init(RT_RingBuffer *buffer, size_t size, size_t elem_size)
{
    if (!buffer || elem_size == 0) return false;

    size_t cap = size ? size * elem_size : RT_RBUFFER_INIT_CAP;
    buffer->data = malloc(cap);
    if (!buffer->data) return false;

    buffer->capacity = cap;
    buffer->capacity_items = cap / elem_size;
    buffer->elem_size = elem_size;
    buffer->head = 0;
    buffer->tail = 0;
    buffer->size = 0;
    
    return true;
}

void rt_rbuffer_free(RT_RingBuffer *buffer)
{
    if (!buffer || !buffer->data) return;

    free(buffer->data);
    buffer->data = NULL;
    buffer->size = 0;
    buffer->capacity = 0;
    buffer->elem_size = 0;
    buffer->head = 0;
    buffer->tail = 0;
}

bool rt_rbuffer_read(RT_RingBuffer *buffer, void *out)
{
    if (!buffer || !out || buffer->size == 0) return false;

    memcpy(out, (char*)buffer->data + (buffer->head * buffer->elem_size), buffer->elem_size);
    buffer->head = (buffer->head + 1) % buffer->capacity_items;
    buffer->size--;
    return true;
}

bool rt_rbuffer_write(RT_RingBuffer *buffer, void *value)
{
    if (!buffer || !buffer->data || !value || rt_rbuffer_is_full(buffer)) return false;

    memcpy((char*)buffer->data + (buffer->tail * buffer->elem_size), value, buffer->elem_size);
    buffer->tail = (buffer->tail + 1) % buffer->capacity_items;
    buffer->size++;
    return true;
}

bool rt_rbuffer_peek(RT_RingBuffer *buffer, void *out)
{
    if (!buffer || buffer->size == 0 || !out) return false;

    memcpy(out, (char*)buffer->data + (buffer->head * buffer->elem_size), buffer->elem_size);
    return true;
}

bool rt_fbuffer_init(RT_FileBuffer *buffer)
{
    if (!buffer) return false;

    buffer->data = NULL;
    buffer->size = 0;
    buffer->capacity = 0;

    return true;
}

void rt_fbuffer_free(RT_FileBuffer *buffer)
{
    if (!buffer) return;
    if (buffer->data) {
        free(buffer->data);
        buffer->data = NULL;
        buffer->size = 0;
        buffer->capacity = 0;
    }
}

size_t rt_fbuffer_read_file(RT_FileBuffer *buffer, const char *file_path)
{
    if (!buffer || !file_path) return 0;

    FILE *f = fopen(file_path, "rb");
    if (!f) return 0;

    fseek(f, 0L, SEEK_END);
    size_t fsize = ftell(f);
    rewind(f);

    if (fsize == 0) {
        fclose(f);
        return 0;
    }

    if (!rt__ensure_capacity(
        (void*)&buffer->data, 
        &buffer->capacity, 
        fsize, 
        RT_FBUFFER_INIT_CAP)
    ) return 0;
    
    if (fread(buffer->data, 1, fsize, f) != fsize) {
        fclose(f);
        return 0;
    }

    buffer->size = fsize;
    
    fclose(f);
    return fsize;
}

size_t rt_fbuffer_write_file(RT_FileBuffer *buffer, const char *file_path)
{
    if (!(buffer && buffer->data) || buffer->size == 0 || !file_path) return false;

    FILE *f = fopen(file_path, "wb");
    if (!f) return 0;

    size_t bytes_w = fwrite(buffer->data, 1, buffer->size, f);
    if (bytes_w < buffer->size) {
        fclose(f);
        return 0;
    }

    fclose(f);
    return bytes_w;
}

bool rt_fbuffer_set(RT_FileBuffer *buffer, const void *data, size_t size)
{
    if (!buffer || !data || size == 0) return false;

    if (!rt__ensure_capacity(
        (void*)&buffer->data, 
        &buffer->capacity, 
        size, 
        RT_FBUFFER_INIT_CAP)
    ) return false;
    
    memcpy(buffer->data, data, size);
    buffer->size = size;

    return true;
}

bool rt_fbuffer_clone(RT_FileBuffer *dst, const RT_FileBuffer *src)
{
    if (!(dst && src && src->data) || src->size == 0) return false;
    if (dst == src) return true;

    if (!rt__ensure_capacity(
        (void*)&dst->data, 
        &dst->capacity, 
        src->size, 
        RT_FBUFFER_INIT_CAP)
    ) return false;

    dst->size = src->size;

    memcpy(dst->data, src->data, src->size);

    return true;
}

bool rt_fbuffer_copy(RT_FileBuffer *dst, const void *src, size_t size)
{
    RT_FileBuffer srcb = { .data = (uint8_t*)src, .size = size };
    return rt_fbuffer_clone(dst, &srcb);
}

uint8_t* rt_fbuffer_at(const RT_FileBuffer *buffer, size_t offset)
{
    if (!buffer || buffer->size == 0 || offset >= buffer->size) return NULL;
    return buffer->data + offset;
}

void rt_fbuffer_clear(RT_FileBuffer *buffer)
{
    if (!buffer) return;
    buffer->size = 0;
}

bool rt_fbuffer_reserve(RT_FileBuffer *buffer, size_t size)
{
    if (!buffer || size == 0) return false;
    if (buffer->capacity >= size) return true; // there is enough mem already

    if (!rt__ensure_capacity(
        (void*)&buffer->data, 
        &buffer->capacity, 
        size, 
        RT_FBUFFER_INIT_CAP)
    ) return false;

    return true;
}

bool rt_fbuffer_resize(RT_FileBuffer *buffer, size_t size)
{
    if (!buffer || size == 0) return false;

    if (!rt__ensure_capacity(
        (void*)&buffer->data, 
        &buffer->capacity, 
        size, 
        RT_FBUFFER_INIT_CAP)
    ) return false;

    buffer->size = size;

    return true;
}

bool rt_fbuffer_fill(RT_FileBuffer *buffer, uint8_t byte)
{
    if (!(buffer && buffer->data) || buffer->size == 0) return false;
    memset(buffer->data, byte, buffer->size);
    return true;
}

bool rt_fbuffer_fill_ex(RT_FileBuffer *buffer, uint8_t byte, size_t size)
{
    if (!buffer || size == 0) return false;
    
    if (!rt__ensure_capacity(
        (void*)&buffer->data, 
        &buffer->capacity, 
        size, 
        RT_FBUFFER_INIT_CAP)
    ) return false;

    buffer->size = size;
    
    return rt_fbuffer_fill(buffer, byte);
}

bool rt_hashtable_init(RT_HashTable *table, size_t buckets_count)
{
    if (!table) return false;
    if (buckets_count == 0) buckets_count = RT_HASHTABLE_INIT_BUCKETS_COUNT;
    
    table->buckets = calloc(buckets_count, sizeof(RT_HTBucket));
    if (!table->buckets) return false;

    table->buckets_count = buckets_count;
    table->size = 0;
    table->hash_func = NULL;

    return true;
}

void rt_hashtable_free(RT_HashTable *table)
{
    if (!table) return;

    for (size_t i = 0; i < table->buckets_count; ++i) {
        RT_HTEntry *e = table->buckets[i].head;
        while (e) {
            RT_HTEntry *n = e->next;
            free(e->key);
            free(e->value);
            free(e);
            e = n;
        }
    }

    free(table->buckets);
    table->buckets = NULL;
    table->hash_func = NULL;
    table->buckets_count = 0;
    table->size = 0;
}

bool rt_hashtable_insert(RT_HashTable *table, const char *key, void *value, size_t value_size)
{
    if (!table || !key || !value || value_size == 0) return false;

    size_t bucket_index = rt__hashtable_bucket_getindex(table, key);
    RT_HTBucket *bucket = &table->buckets[bucket_index];
    RT_HTEntry *cur_entry = bucket->head;

    while (cur_entry) { // update if exists
        if (strcmp(cur_entry->key, key) == 0) {
            printf("[RT] Insert operation | bucket_index:%zu, key:`%s`, value_pointer:%p, value:%d, value_size:%zu\n",
                bucket_index, key, value, *(int*)value, value_size 
            );
            return rt__hashtable_update_entry_value(cur_entry, value, value_size);
        }

        cur_entry = cur_entry->next;
    }

    // insert new entry
    size_t ksize = strlen(key) + 1;
    RT_HTEntry *new_entry = rt__hashtable_entry_alloc(ksize, value_size);
    if (!new_entry) return false;

    strcpy_s(new_entry->key, ksize, key);
    memcpy_s(new_entry->value, value_size, value, value_size);
    new_entry->value_size = value_size;

    new_entry->next = bucket->head;
    bucket->head = new_entry;

    bucket->count++;
    table->size++;

    printf("[RT] Inserted hashtable element | bucket_index:%zu, key:`%s`, value_pointer:%p, value:%d, value_size:%zu\n",
       bucket_index, key, value, *(int*)value, value_size 
    );

    return true;
}

bool rt_hashtable_get(RT_HashTable *table, const char *key, void *out, size_t out_size)
{
    if (!table || !key || !out || out_size == 0) return false;

    RT_HTEntry *entry = rt__hashtable_get_entry(table, key);
    if (!entry) return false;
    if (out_size < entry->value_size) return false;
    memcpy_s(out, out_size, entry->value, entry->value_size);

    return true;
}

bool rt_hashtable_remove(RT_HashTable *table, const char *key)
{
    if (!table || !key) return false;

    size_t bucket_index = rt__hashtable_bucket_getindex(table, key);
    RT_HTBucket *bucket = &table->buckets[bucket_index];
    RT_HTEntry *entry = bucket->head;
    RT_HTEntry *prev_entry = NULL;

    while (entry) {
        if (strcmp(key, entry->key) == 0) {
            free(entry->value);
            free(entry->key);

            if (prev_entry) {
                prev_entry->next = entry->next;
            } else {
                bucket->head = entry->next;
            }

            free(entry);
            bucket->count--;
            table->size--;

            printf("[RT] Removed hashtable element | bucket_index:%zu, key:`%s`\n", bucket_index, key);

            return true;
        }

        prev_entry = entry;
        entry = entry->next;
    }

    return false;
}

bool rt_hashtable_contains(RT_HashTable *table, const char *key)
{
    if (!table || !key) return false;

    RT_HTBucket *bucket = rt_hashtable_get_bucket(table, key);
    RT_HTEntry *entry = rt_hashtable_entry_first(bucket);
    if (!entry) return false;

    do {
        if (strcmp(entry->key, key) == 0) 
            return true;
    } while ((entry = rt_hashtable_entry_next(entry)));

    return false;
}
