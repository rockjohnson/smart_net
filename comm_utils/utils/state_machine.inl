/*
 * state_machine.cpp
 *
 *  Created on: Apr 30, 2009
 *      Author: rock
 */

//#include "../hdr/state_machine.h"

//namespace nm_utils
//{

	template <typename T>
	CStateMachine<T>::CStateMachine(T *t)
	:m_iCurState(0), m_t(t)//, m_pFun(pFun)
	{
		// TODO Auto-generated constructor stub
	}

	template <typename T>
	CStateMachine<T>::~CStateMachine()
	{
		// TODO Auto-generated destructor stub
	}

	template <typename T>
	int CStateMachine<T>::reg_evt_state(int iStartState, int iEvt, int iEndState, PFUN_TRANSFORM_STATE fun)
	{
		if (m_evt_handler[iStartState].find(iEvt) != m_evt_handler[iStartState].end())
		{
			ASSERT(false);
			return -1;
		}

		m_evt_handler[iStartState][iEvt] = SElem(iEndState, fun);

		return 0;
	}

	template <typename T>
	int CStateMachine<T>::post_event(int iEvt, pvoid_t pV)
	{
#ifdef __USED_IN_MULTI_THREAD__
		scope_lock_t lk(m_lock);//ǧ��Ҫ���Ű�����������ƣ���Ϊ����Ļ�����ǰ״̬���Ƿ��̻߳���ģ�������߼����⣻
#endif
		typename evt_h_t::iterator iter2;
		typename event_handler_t::iterator iter1;
		if ((iter1 = m_evt_handler.find(m_iCurState)) == m_evt_handler.end()
				|| (iter2 = iter1->second.find(iEvt)) == iter1->second.end())
		{
			//ASSERT(false);
			return -1;
		}
		int iEndState = iter2->second.iState;
		if (NULL != (iter2->second.fun) && (m_t->*(iter2->second.fun))(m_iCurState, iEvt, iEndState, pV) < 0)
		{
			//transform state failed!
			return -2;
		} 
		set_cur_state(iEndState);

		//transform_state(iEvt, m_evt_handler[m_iCurState][iEvt].iState, m_evt_handler[m_iCurState][iEvt].fun);

		return RET_SUC;
	}

//}
