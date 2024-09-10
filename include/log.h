#pragma once

#define INFO_STRING "[info]"
#define WARN_STRING "[warn]"
#define ERRO_STRING "[error]"

void log_info(const char* string, ...);
void log_warn(const char* string, ...);
void log_erro(const char* string, ...);