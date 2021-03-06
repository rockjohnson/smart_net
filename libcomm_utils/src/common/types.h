/*
 * types.h
 *
 *  Created on: Jul 2, 2010
 *      Author: rock
 */

#ifndef __TYPES_H__
#define __TYPES_H__

#include <sys/types.h>
#include <string>

typedef void* cmn_pvoid_t;
typedef char cmn_char_t;
typedef const char* cmn_cstr_t;
typedef std::string cmn_string_t;
typedef unsigned char cmn_byte_t;

#if __PLATFORM__ == __PLATFORM_LINUX__
typedef int32_t sock_handle_t;
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
typedef SOCKET sock_handle_t;
#endif


#endif /* TYPES_H_ */
