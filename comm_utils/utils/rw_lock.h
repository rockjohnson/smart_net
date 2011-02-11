/*
* rw_lock.h
*
*  Created on: Jul 16, 2009
*      Author: rock
*/

#ifndef RW_LOCK_H_
#define RW_LOCK_H_

namespace nm_utils
{

	template<typename TSem, typename TLock, ui32_t uiMaxReaders = 32/*the max concurrent users.*/>
	class CRWLock //lock for read and write, there can be multiple threads in reading.
	{
	public:
		CRWLock(/*unsigned int uiMaxReaders = 32*/) :
		  /*m_uiMaxReader(uiMaxReaders),*/ m_sem(uiMaxReaders)
		  {
			  //m_sem.create(uiMaxReaders);
		  }

		  void lock_read()
		  {
			  m_sem.acquire();
		  }

		  void unlock_read()
		  {
			  m_sem.release();
		  }

		  void lock_write()
		  {
			  m_lock.lock();
			  for (int i = 0; i < uiMaxReaders; ++i)
				  m_sem.acquire();
			  m_lock.unlock();
		  }

		  void unlock_write()
		  {
			  for (int i = 0; i < uiMaxReaders; ++i)
				  m_sem.release();
		  }

	private:
		//unsigned int m_uiMaxReader; // the max concurrent users.
		TSem m_sem;
		TLock m_lock;
	};

	//Here's how we can use it in applications:

	//CRWLock mutex;
	//
	//void ReaderThread::run()
	//{
	//    ...
	//    mutex.lockRead();
	//    read_file();
	//    mutex.unlockRead();
	//    ...
	//}
	//
	//void WriterThread::run()
	//{
	//    ...
	//    mutex.lockWrite();
	//    write_file();
	//    mutex.unlockWrite();
	//    ...
	//}

	template<typename T>
	class CScopeLock_r
	{
	public:
		CScopeLock_r(T &lock) :
			m_lock(lock)
		{
			m_lock.lock_read();
		}

		~CScopeLock_r()
		{
			m_lock.unlock_read();
		}

	private:
		T &m_lock;
	};

	template<typename T>
	class CScopeLock_w
	{
	public:
		CScopeLock_w(T &lock) :
		m_lock(lock)
		{
			m_lock.lock_write();
		}

		~CScopeLock_w()
		{
			m_lock.unlock_write();
		}

	private:
		T &m_lock;
	};
}

#endif /* RW_LOCK_H_ */
