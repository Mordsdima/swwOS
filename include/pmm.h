#include <stddef.h>
#include <limine.h>
#include <stdbool.h>
#include <bootstub.h>

#pragma once

typedef struct memory_node {
    struct memory_node *next, *prev;
    size_t pages;
} memory_node_t;

int init_pmm(bootstub_t* bs);
uint64_t kmalloc_pages(size_t pages);
void kfree_pages(uint64_t addr, size_t pages);
bool pmm_is_initializated();