/*
 * mem.h
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#ifndef __MEM_H__
#define __MEM_H__

#include "../common/common.h"
#include "../utils/smart_ptr.h"

namespace nm_mem
{

class CMemBlock : public nm_cmn_base::ICommonBase
{
public:
	CMemBlock();
	~CMemBlock();

public:
	cmn_byte_t* get_mem();
	cmn_byte_t* get_data();

	void reset();

private:
	cmn_byte_t *m_pBytes;
	u_int32_t m_ui32Sz;
	u_int32_t m_ui32Offset;
	u_int32_t m_ui32Len;
};
typedef nm_utils::CSmartPtr<nm_mem::CMemBlock> mem_ptr_t;

}

#endif /* MEM_H_ */
