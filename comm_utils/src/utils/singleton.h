/*
 * singleton.h
 *
 *  Created on: Apr 4, 2009
 *      Author: rock
 */

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <stdlib.h>
#include "../common/defines.h"
#include "../thread/auto_lock.h"

namespace nm_utils
{
	template <typename T>
	class CSingleton
	{
	private:
		CSingleton()
		{
		}
		CSingleton(const CSingleton &)
		{
		}
		CSingleton& operator =(const CSingleton &)
		{
			return *this;
		}

	public:
		static T& instance()
		{
			if (s_t == NULL)
			{
				CScopeLock<CMutexLock> lock(s_lock);
				if (s_t == NULL)
				{
					s_t = SYS_NEW T;
					::atexit(destroy);
				}
			}

			return *s_t;
		}

	private:
		static void destroy()
		{
			if (s_t != NULL)
			{
				delete s_t;
				s_t = NULL;
			}
		}

	private:
		static T *s_t;
		static CMutexLock s_lock;
	};

	template<typename T>
	T* CSingleton<T>::s_t = NULL;
	template<typename T>
	CMutexLock CSingleton<T>::s_lock;
}

#endif /* __SINGLETON_H__ */
