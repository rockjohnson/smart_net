/*
 * defines.h
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#ifndef __SMARTNET_DEFINES_H__
#define __SMARTNET_DEFINES_H__

#define __EPOLL__ (1)
#define __SELECT__ (2)

#if __PLATFORM__ == __PLATFORM_LINUX__
#define __IO_EVT_NOTIFY_MECHANISM__ __EPOLL__
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
#define __IO_EVT_NOTIFY_MECHANISM__ __SELECT__
#endif

#define HTONS(u16) \
	htons(u16)
#define HTONL(u32) \
	htonl(u32)

#endif /* DEFINES_H_ */
