/*
 * auto_lock.h
 *
 *  Created on: Apr 4, 2009
 *      Author: rock
 */

#ifndef __AUTO_LOCK_H__
#define __AUTO_LOCK_H__

#if __PLATFORM__ == __PLATFORM_LINUX__
#include <pthread.h>
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
#include <Windows.h>
#endif
#include <assert.h>

#include "../common/common.h"

namespace nm_utils
{
    ///what do you want me to say? so simple!
	class CSpinLock
	{
		public:
			CSpinLock(bool bProcessShared = false)
			{
				SYS_ASSERT(RET_SUC == init(bProcessShared));
			}

			~CSpinLock()
			{
				SYS_ASSERT(RET_SUC == destroy());
			}

			int lock()
			{
				return pthread_spin_lock(&m_lk);
			}

			int try_lock()
			{
				return pthread_spin_trylock(&m_lk);
			}

			int unlock()
			{
				return pthread_spin_unlock(&m_lk);
			}

		private:
			int init(bool bProcessShared)
			{
				return pthread_spin_init(&m_lk,
						bProcessShared ? PTHREAD_PROCESS_SHARED
								: PTHREAD_PROCESS_PRIVATE);
			}

			int destroy()
			{
				return pthread_spin_destroy(&m_lk);
			}

		private:
			pthread_spinlock_t m_lk;
	};

	///
	class CMutexLock
	{
	public:
		inline CMutexLock()
		{
#if __PLATFORM__ == __PLATFORM_LINUX__
			pthread_mutex_init(&m_mtx, NULL);
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
			::InitializeCriticalSection(&m_cs);
#endif
		}

		inline ~CMutexLock()
		{
#if __PLATFORM__ == __PLATFORM_LINUX__
			pthread_mutex_destroy(&m_mtx);
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
			::DeleteCriticalSection(&m_cs);
#endif
		}

		inline void lock()
		{
#if __PLATFORM__ == __PLATFORM_LINUX__
			pthread_mutex_lock(&m_mtx);
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
			EnterCriticalSection(&m_cs);
#endif
		}

		inline bool try_lock()
		{
#if __PLATFORM__ == __PLATFORM_LINUX__
			return pthread_mutex_trylock(&m_mtx) == 0 ? true : false;
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
			return TryEnterCriticalSection(&m_cs) ? true : false;
#endif
		}

		inline void unlock()
		{
#if __PLATFORM__ == __PLATFORM_LINUX__
			pthread_mutex_unlock(&m_mtx);
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
			LeaveCriticalSection(&m_cs);
#endif
		}

	private:
#if __PLATFORM__ == __PLATFORM_LINUX__
		pthread_mutex_t m_mtx;
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
		CRITICAL_SECTION m_cs; 
#endif
	};

	template<typename T>
	class CScopeLock : public nm_cmn_base::CNoncopyable
	{
	public:
		CScopeLock(T &lock)
		:m_lock(lock)
		{
			m_lock.lock();
		}

		~CScopeLock()
		{
			m_lock.unlock();
		}


	private:
		T &m_lock;
	};

	typedef CScopeLock<CMutexLock> mtx_scopelk_t;
	typedef CScopeLock<CSpinLock> spin_scopelk_t;
}

#endif /* __AUTO_LOCK_H__ */
