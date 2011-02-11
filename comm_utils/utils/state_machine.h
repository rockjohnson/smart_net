/*
 * state_machine.h
 *
 *  Created on: Apr 30, 2009
 *      Author: rock
 */

#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#include "../common/common.h"

#ifdef __USED_IN_MULTI_THREAD__
#include "../utils/auto_lock.h"
#endif

namespace nm_utils
{
	template <typename T>
	class CStateMachine
	{
		typedef int (T::*PFUN_TRANSFORM_STATE)(int,int,int,pvoid_t);
		struct SElem
		{
			SElem(){}
			SElem(int iS, PFUN_TRANSFORM_STATE f)
			:iState(iS), fun(f){}

			int iState;
			PFUN_TRANSFORM_STATE fun;
		};
		typedef STD_MAP<int/*event*/, SElem/*end state and fun*/> evt_h_t;
		typedef STD_MAP<int/*start state*/, evt_h_t> event_handler_t;

	public:
		CStateMachine(T *t);
		virtual ~CStateMachine();

	public:

		void set_cur_state(int iState){m_iCurState = iState;}
		int reg_evt_state(int iStartState, int iEvt, int iEndState, PFUN_TRANSFORM_STATE fun);
		int post_event(int iEvt, pvoid_t pV);
		int get_cur_state()
		{
			return m_iCurState;
		}

	private:
#ifdef __USED_IN_MULTI_THREAD__
		CMutexLock m_lock; //may be not needed, if all the state relative function is handled in single thread.
#endif
		int m_iCurState; //current state;
		T *m_t;
		event_handler_t m_evt_handler;
	};

#include "state_machine.inl"

}

#endif /* __STATE_MACHINE_H__ */
