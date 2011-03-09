/*
 * sn_io_engine.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: rock
 */

#include "sn_io_engine.h"
#include "../common/sn_common.h"

namespace nm_framework
{

CIoEngine::CIoEngine()
{
	// TODO Auto-generated constructor stub

}

CIoEngine::~CIoEngine()
{
	// TODO Auto-generated destructor stub
}

int32_t CIoEngine::start(u_int32_t ui32InputThreadCnt, u_int32_t ui32OutputThreadCnt, int32_t i32IoEvtNotifier, int32_t i32MsTimeout)
{
	IF_TRUE_THEN_RETURN_CODE(0 == ui32InputThreadCnt
			|| 0 == ui32OutputThreadCnt , CMNERR_COMMON_ERR);

		///create protocol wrapper
		m_vecProtoWrappers[EP_TCP] = SYS_NOTRW_NEW(CTcpWrapper);

		io_task_ptr_t pIoTask = NULL;
		thread_ptr_t pThread = NULL;
		int32_t i32Ret = CMNERR_SUC;
		while (ui32IoThreadCnt-- > 0)
		{
			pIoTask = SYS_NOTRW_NEW(CIoTask);
			if (NULL == pIoTask)
			{
				i32Ret = CMNERR_COMMON_ERR;
				break;
			}
			i32Ret = pIoTask->init(i32IoEvtNotifier, i32MsTimeout);
			if (CMNERR_SUC != i32Ret)
			{
				break;
			}
			//m_setIoTasks.push_back(pIoTask);

			pThread = SYS_NOTRW_NEW(CThread);
			if (NULL == pThread)
			{
				i32Ret = CMNERR_COMMON_ERR;
				break;
			}

			i32Ret = pThread->init();
			if (CMNERR_SUC != i32Ret)
			{
				break;
			}

			i32Ret = pThread->assign_task(pIoTask);
			if (CMNERR_SUC != i32Ret)
			{
				break;
			}

			i32Ret = pThread->start();
			if (CMNERR_SUC != i32Ret)
			{
				break;
			}
			m_vecInputThreads.push_back(pThread); ///not judge the return?
		}

		if (CMNERR_SUC != i32Ret)
		{
			stop();
		}

		return i32Ret;
}

int32_t CIoEngine::add_io_obj(const io_obj_ptr_t &pIoObj)
{
	int32_t i32Ret = CMNERR_SUC;
	if (pIoObj->get_output_flg() != 0)
	{
		int32_t iMinIdx = 0;
		int32_t iMinCnt = 0;
		for (int32_t i = 0; i < m_vecoutputtasks.size(); i++)
		{
			if (m_vecoutputtasks[i]->get_fd_cnt() < iMinCnt)
			{
				iMinIdx = i;
				iMinCnt = m_vecoutputtasks[i]->get_fd_cnt();
			}
		}

		pIoObj->set_output_task_id(iMinIdx);
		i32Ret = m_vecoutputtasks[iMinIdx]->add_io_obj(pIoObj);
		IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32Ret, i32Ret);
	}

	if (pIoObj->get_input_flg() != 0)
	{
		int32_t iMinIdx = 0;
		int32_t iMinCnt = 0;
		for (int32_t i = 0; i < m_vecinputtasks.size(); i++)
		{
			if (m_vecinputtasks[i]->get_fd_cnt() < iMinCnt)
			{
				iMinIdx = i;
				iMinCnt = m_vecinputtasks[i]->get_fd_cnt();
			}
		}

		pIoObj->set_input_task_id(iMinIdx);
		i32Ret = m_vecinputtasks[iMinIdx]->add_io_obj(pIoObj);
		IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32Ret, i32Ret);
	}

	return CMNERR_SUC;
}

}
