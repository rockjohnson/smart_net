/*
 * shared_obj.h
 *
 *  Created on: Feb 10, 2011
 *      Author: rock
 */

#ifndef __SHARED_OBJ_H__
#define __SHARED_OBJ_H__

#include <vector>

#include "../common/sn_common.h"
#include "sn_endpoint.h"
#include "sn_proto_wrapper.h"


namespace nm_framework
{

/**
 * the smart net manager.
 * */
class CSmartNetMgr: public nm_base::ICommonBase
{
public:
	CSmartNetMgr();
	virtual ~CSmartNetMgr();

public:
	int32_t start(u_int32_t ui32InputThreadCnt,
			u_int32_t ui32OutputThreadCnt,
			int32_t i32IoEvtNotifier,
			int32_t i32MsTimeout);
	int32_t stop();
	///
	int32_t add_endpoint(const endpoint_ptr_t &pEP); ///not thread safe
	int32_t del_endpoint(const endpoint_ptr_t &pEP); ///not thread safe

private:
	DISALLOW_COPY_AND_ASSIGN(CSmartNetMgr);
	///io engines
	 m_pIoEngine;
	///network protocol wrappers
	typedef std::vector<proto_wrapper_ptr_t> proto_wrapper_vec_t;
	proto_wrapper_vec_t m_vecProtoWrappers;
};
typedef nm_utils::CSmartPtr<nm_framework::CSmartNetMgr> smart_net_mgr_ptr_t;

}

#endif /* __SHARED_OBJ_H__ */
