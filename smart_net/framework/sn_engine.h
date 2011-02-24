/*
 * shared_obj.h
 *
 *  Created on: Feb 10, 2011
 *      Author: rock
 */

#ifndef __SHARED_OBJ_H__
#define __SHARED_OBJ_H__

#include "../framework/sn_io_task.h"
#include <vector>

namespace nm_framework
{
	/**
	 * the smart net manager.
	 * */
	class CIOEngine : public nm_base::ICommonBase
	{
		public:
			CIOEngine();
			virtual ~CIOEngine();

		public:
			int32_t start(u_int32_t ui32IoThreadCnt, int32_t i32IoEvtNotifier, int32_t i32MsTimeout);
			int32_t stop();

			int32_t add_io_obj(const io_obj_ptr_t &pIoObj);
			int32_t del_io_obj(const io_obj_ptr_t &pIoObj);

		private:
			typedef std::vector<nm_thread::thread_ptr_t> io_thread_vec_t;
			io_thread_vec_t m_vecIoThreads; ///mainly handle input and business.
			nm_thread::thread_ptr_t m_pThreadHandleOutput; ///only handle output.
	};
	typedef nm_utils::CSmartPtr<nm_framework::CIOEngine> net_engine_ptr_t;

}

#endif /* __SHARED_OBJ_H__ */