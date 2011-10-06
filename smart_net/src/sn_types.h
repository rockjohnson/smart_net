/*
 * sn_types.h
 *
 *  Created on: Mar 10, 2011
 *      Author: rock
 */

#ifndef __SN_TYPES_H__
#define __SN_TYPES_H__

#include "sn_defines.h"

#if __PLATFORM__ == __PLATFORM_LINUX__
typedef int32_t sock_handle_t;
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
typedef SOCKET sock_handle_t;
#endif


#endif /* __SN_TYPES_H__ */
