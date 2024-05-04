#include "string_builder.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "../vendor/stb_sprintf.h"

#include <string.h>

StringBuilder::StringBuilder(void) {
    start = null;
    current = null;
    count = 0;
    allocator_data = null;
    allocator = default_allocator;
}

void StringBuilder::init(Allocator _allocator, void *_allocator_data) {
    allocator = _allocator;
    allocator_data = _allocator_data;
}

void StringBuilder::maybe_grow(s64 length_to_add) {
    // first time we append to the builder
    if (start == null) {
        u8 *bytes = (u8 *)allocator(ALLOCATOR_ALLOCATE, size_of(StringNode) + STRING_BUFFER_INSTANCE_LENGTH, 0, null, allocator_data, 0);
        start = (StringNode *)bytes;
        if (start) {
            start->count = 0;
            start->next = null;
            start->data = bytes + size_of(StringNode);

            current = start; // we always operate on the current string buffer
        }
        return;
    }

    if (length_to_add > (STRING_BUFFER_INSTANCE_LENGTH - current->count)) { // we need to append a new chunk to the builder
        u8 *bytes = (u8 *)allocator(ALLOCATOR_ALLOCATE, size_of(StringNode) + STRING_BUFFER_INSTANCE_LENGTH, 0, null, allocator_data, 0);
        StringNode *node = (StringNode *)bytes;
        if (node) {
            node->count = 0;
            node->next = null;
            node->data = bytes + size_of(StringNode);

            current->next = node;
            current = node;
        }
    }
}

void StringBuilder::append(char *s) {
    Assert(s != null);

    u64 length = strlen(s);
    maybe_grow(length);

    u8 *dest = current->data + current->count;
    current->count += length; // increment buffer count
    count += length; // increment builder count
    memcpy(dest, s, length);
}

void StringBuilder::append(char *s, s64 length) {
    Assert(s != null);
    maybe_grow(length);

    u8 *dest = current->data + current->count;
    current->count += length; // increment buffer count
    count += length; // increment builder count
    memcpy(dest, s, length);
}

void StringBuilder::append(u8 byte) {
    Assert(s != null);
    maybe_grow(size_of(byte));

    u8 *dest = current->data + current->count;
    current->count += size_of(byte); // increment buffer count
    count += size_of(byte); // increment builder count
    *dest = byte;
}

bool StringBuilder::print(char* format, ...) {
    u8 buffer[STRING_BUFFER_INSTANCE_LENGTH];

    va_list arguments;
    va_start(arguments, format);
    s64 length = stbsp_vsprintf((char *)buffer, format, arguments);
    if (length <= 0) return false;
    va_end(arguments);

    append((char *)buffer, length);

    return true;
}

u8 *StringBuilder::to_string(void) {
    Assert(count > 0);
    u8 *buffer = (u8 *)default_allocator(ALLOCATOR_ALLOCATE, count + 1, 0, null, null, 0);
    if (!buffer) return null;
    
    StringNode *it = start;
    u8 *dest = buffer;
    while (it) {
        memcpy(dest, it->data, it->count);
        dest += it->count;
        it = it->next;
    }
    *dest = 0;

    return buffer;
}

u8 *StringBuilder::to_string(Allocator allocator, void *allocator_data) {
    Assert(count > 0);
    u8 *buffer = (u8 *)allocator(ALLOCATOR_ALLOCATE, count + 1, 0, null, allocator_data, 0);
    if (!buffer) return null;
    
    StringNode *it = start;
    u8 *dest = buffer;
    while (it) {
        memcpy(dest, it->data, it->count);
        dest += it->count;
        it = it->next;
    }
    *dest = 0;

    return buffer;
}

void StringBuilder::free_buffers(void) {
    StringNode *it = start;
    while (it) {
        StringNode *buffer = it;
        it = it->next;
        allocator(ALLOCATOR_FREE, 0, 0, buffer, allocator_data, 0);
    }
}