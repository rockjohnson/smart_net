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

template <class CLS>
class CObjFactory
{
public:
	CObjFactory();
	virtual ~CObjFactory();

public:
	virtual CSmartPtr<CLS> create_obj()
	{
		return CSmartPtr<CLS>(SYS_NOTRW_NEW(CLS));
	}
};

}

#endif /* OBJ_FACTORY_H_ */
