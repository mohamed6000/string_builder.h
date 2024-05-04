#pragma once
/*

A mutable string of characters used to dynamically build a string
You can use custom allocators
depends on stb_sprintf.h: https://github.com/nothings/stb/blob/master/stb_sprintf.h
and base_layer(common.h): https://github.com/mohamed6000/base_layer

*/

#include "common.h"

#define STRING_BUFFER_INSTANCE_LENGTH 1024

struct StringNode {
    s64 count;
    u8 *data;
    StringNode *next;
};

struct StringBuilder {
    StringNode *start;
    StringNode *current;
    s64 count;

    void *allocator_data;
    Allocator allocator;


    StringBuilder(void);
    void init(Allocator _allocator, void *_allocator_data);

    void maybe_grow(s64 length_to_add);

    void append(char *s);
    void append(char *s, s64 length);
    void append(u8 byte);

    // prints the items to the builder
    bool print(char* format, ...);

    u8 *to_string(void);
    u8 *to_string(Allocator allocator, void *allocator_data);

    void free_buffers(void);
};