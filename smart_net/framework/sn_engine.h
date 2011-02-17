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
	class CNetEngine : public nm_base::ICommonBase
	{
		public:
			CNetEngine();
			virtual ~CNetEngine();

		public:
			int32_t start(u_int32_t ui32IoThreadCnt);
			int32_t stop();

			int32_t add_io_obj(io_obj_ptr_t &pIoObj);
			int32_t del_io_obj(io_obj_ptr_t &pIoObj);

		private:
			typedef std::vector<nm_thread::thread_ptr_t> io_thread_vec_t;
			io_thread_vec_t m_vecIoThreads;
	};
	typedef nm_utils::CSmartPtr<nm_framework::CNetEngine> net_engine_ptr_t;

}

#endif /* __SHARED_OBJ_H__ */
