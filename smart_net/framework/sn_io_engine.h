/*
 * sn_io_engine.h
 *
 *  Created on: Mar 2, 2011
 *      Author: rock
 */

#ifndef __SN_IO_ENGINE_H__
#define __SN_IO_ENGINE_H__

#include "sn_io_obj.h"

namespace nm_framework
{

class CIoEngine
{
public:
	CIoEngine();
	virtual ~CIoEngine();

public:
	int32_t start(u_int32_t ui32InputThreadCnt, u_int32_t ui32OutputThreadCnt, int32_t i32IoEvtNotifier, int32_t i32MsTimeout);
	int32_t stop();

	int32_t add_io_obj(const io_obj_ptr_t &pIoObj);
	int32_t del_io_obj(const io_obj_ptr_t &pIoObj);

private:
	///
	typedef std::vector<nm_thread::thread_ptr_t> io_thread_vec_t;
	io_thread_vec_t m_vecIoThreads; ///io threads
	///
	typedef std::vector<nm_framework::io_task_ptr_t> io_task_vec_t;
	io_task_vec_t m_vecinputtasks;
	io_task_ptr_t m_vecoutputtasks;
};
typedef nm_utils::CSmartPtr<nm_framework::CIoEngine> io_engine_ptr_t;

}

#endif /* __SN_IO_ENGINE_H__ */
