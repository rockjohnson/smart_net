/*
 * defines.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __PLATFORM_WINDOWS__ (0)
#define __PLATFORM_LINUX__    (1)
#define __PLATFORM_APPLE__   (2)
#define __PLATFORM_INTEL__   (3)
#define __PLATFORM_UNIX__ __PLATEFORM__LINUX__

// must be first (win 64 also define _WIN32)
#if defined( _WIN64 )
#  define __PLATFORM__ __PLATFORM_WINDOWS__
#elif defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#  define __PLATFORM__ __PLATFORM_WINDOWS__
#elif defined( __APPLE_CC__ )
#  define __PLATFORM__ __PLATFORM_APPLE__
#elif defined( __INTEL_COMPILER )
#  define __PLATFORM__ __PLATFORM_INTEL__
#else
#  define __PLATFORM__ __PLATFORM_LINUX__
#endif

#define __COMPILER_MICROSOFT__ (0)
#define __COMPILER_GNU__       (1)
#define __COMPILER_BORLAND__   (2)
#define __COMPILER_INTEL__     (3)

#ifdef _MSC_VER
#  define __COMPILER__ __COMPILER_MICROSOFT__
#elif defined( __BORLANDC__ )
#  define __COMPILER__ __COMPILER_BORLAND__
#elif defined( __INTEL_COMPILER )
#  define __COMPILER__ __COMPILER_INTEL__
#elif defined( __GNUC__ )
#  define __COMPILER__ __COMPILER_GNU__
#else
#  pragma error "FATAL ERROR: Unknown compiler."
#endif

#if __COMPILER__ == __COMPILER_MICROSOFT__
#  pragma warning( disable : 4267 )                         // conversion from 'size_t' to 'int', possible loss of data
#  pragma warning( disable : 4786 )                         // identifier was truncated to '255' characters in the debug information
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


//#define __USING_SIMPLE_LOG__ (1)
#define __USING_GNU_ATOMIC__ (1)
#define __DEBUG_VER__ (1)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ZERO (0)
#define RET_SUC (0)
#define RET_ERR (-1)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SYS_ASSERT(expr) \
	assert((expr))

#define IF_TRUE_THEN_RETURN_CODE(expr, retcode) \
	if ((expr)) \
	{ \
		return retcode; \
	}

#define IF_TRUE_THEN_RETURN(expr) \
	if ((expr)) \
	{ \
		return; \
	}

#endif /* DEFINES_H_ */
