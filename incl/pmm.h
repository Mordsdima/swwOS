#pragma once
#include <stddef.h>
#include <limine.h>
#include <stdbool.h>

typedef struct memory_node {
    struct memory_node *next, *prev;
    size_t pages;
} memory_node_t;

void init_pmm();
uint64_t* kmalloc_pages(size_t pages);
void kfree_pages(uint64_t* addr, size_t pages);
