#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <cstdarg>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define GLEW_STATIC
#include "GL/glew.h"

#include <glfw/glfw3.h>

#include <glm/glm.hpp>

#include <any>
#include <set>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

using String = std::string;

template <typename T>
using Array = std::vector<T>;

template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

template <typename A, typename B>
using Pair = std::pair<A, B>;

template <typename T>
using Set = std::set<T>;

void LogFatal(const char *format, ...);
void LogError(const char *format, ...);
void LogInfo(const char *format, ...);
char *ReadEntireFile(const char *file_path);

#endif
