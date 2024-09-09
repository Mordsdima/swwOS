#pragma once

typedef struct {
    const char* name;
    int (*init)(void);
    int (*fini)(void);
} module_t;