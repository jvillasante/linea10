#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <stdlib.h>

/*
 * Debug Macros
 */
#ifdef NDEBUG
#define DEBUG(M, ...)
#else
#define DEBUG(M, ...) fprintf(stderr, "[DEBUG] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define LOG_ERROR(M, ...) fprintf(stderr, "[ERROR] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_INFO(M, ...) fprintf(stderr, "[INFO] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define CHECK(exp, msg) if(!(exp)) { \
  fprintf(stderr, "[ERROR] (%s:%d, errno: %d - %s) %s\n", __FILE__, __LINE__, 0, "None", msg); \
  goto error; }

#define CHECK_IDKIT(err, msg) if (err != 0) { \
  fprintf(stderr, "[ERROR (IDKit)] (%s:%d, errno: %d - %s) %s\n", __FILE__, __LINE__, err, IEngine_GetErrorMsg(err), msg); \
  goto error; }

#define CHECK_VCOM(err, msg) if (err != GEN_OK) { \
  fprintf(stderr, "[ERROR (VCOM)] (%s:%d, errno: %d - %s) %s\n", __FILE__, __LINE__, err, VCOM_GetErrorMsg(err), msg); \
  goto error; }

/*
 * Use this macro for unused parameters right in the beginning of a function body
 * to suppress compiler warnings about unused parameters.
 * 
 * This is mainly meant for function parameters and not for unused local variables.
 */
#define UNUSED(ParamName) ((void)(0 ? ((ParamName) = (ParamName)) : (ParamName)))

#endif
