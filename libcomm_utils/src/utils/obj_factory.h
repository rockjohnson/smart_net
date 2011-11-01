/*
 * obj_factory.h
 *
 *  Created on: Feb 17, 2011
 *      Author: rock
 */

#ifndef OBJ_FACTORY_H_
#define OBJ_FACTORY_H_

#include "../common/common.h"
#include "smart_ptr.h"

namespace nm_utils
{

template <typename T>
class CObjFactory : public nm_cmn_base::ICommonBase
{
public:
	typedef nm_utils::CSmartPtr<T> obj_ptr_t;
	typedef nm_utils::CSmartPtr<nm_utils::CObjFactory<T> > obj_factory_ptr_t;

public:
	CObjFactory(){}
	virtual ~CObjFactory(){}

public:
	virtual obj_ptr_t create_obj(int32_t i32Type) = 0;
//	{
//		(void)i32Type;
//		return obj_ptr_t(SYS_NOTRW_NEW(T));
//	}
};


}

#endif /* OBJ_FACTORY_H_ */
