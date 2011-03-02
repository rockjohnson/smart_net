/*
 * sn_io_engine.h
 *
 *  Created on: Mar 2, 2011
 *      Author: rock
 */

#ifndef __SN_IO_ENGINE_H__
#define __SN_IO_ENGINE_H__

namespace nm_framework
{

class CIoEngine
{
public:
	CIoEngine();
	virtual ~CIoEngine();

	typedef std::vector<nm_thread::thread_ptr_t> io_thread_vec_t;
	io_thread_vec_t m_vecIoThreads; ///mainly handle input and business.
	nm_thread::thread_ptr_t m_pThreadHandleOutput; ///only handle output.
};
typedef nm_utils::CSmartPtr<nm_framework::CIoEngine> io_engine_ptr_t;

}

#endif /* __SN_IO_ENGINE_H__ */
