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
#include "../utils/smart_lock.h"
#endif

namespace nm_utils
{
	template <typename T>
	class CStateMachine
	{
		typedef int32_t (T::*PFUN_TRANSFORM_STATE)(int32_t, int32_t, int32_t, cmn_pvoid_t);
		struct SElem
		{
			SElem(){}
			SElem(int32_t iS, PFUN_TRANSFORM_STATE f)
			:iState(iS), fun(f){}

			int32_t iState;
			PFUN_TRANSFORM_STATE fun;
		};
		typedef STD_MAP<int32_t/*event*/, SElem/*end state and fun*/> evt_h_t;
		typedef STD_MAP<int32_t/*start state*/, evt_h_t> event_handler_t;

	public:
		CStateMachine(T *t);
		virtual ~CStateMachine();

	public:

		void set_cur_state(int32_t iState)
		{
			nm_utils::mtx_scopelk_t lk(m_lkChangeState);
			m_i32curstate = iState;
		}
		int32_t reg_evt_state(int32_t iStartState, int32_t iEvt, int32_t iEndState, PFUN_TRANSFORM_STATE fun);
		int32_t post_evt(int32_t iEvt, cmn_pvoid_t pV);
		inline int32_t get_cur_state()
		{
			return m_i32curstate;
		}

		bool begin_lock_state(int32_t i32State)
		{
#if (__USED_IN_MULTI_THREAD__)
			m_lkChangeState.lock();
#endif
			return (get_cur_state() == i32State);
		}

		void end_lock_state()
		{
#if (__USED_IN_MULTI_THREAD__)
			m_lkChangeState.unlock();
#endif
		}

	private:
#ifdef __USED_IN_MULTI_THREAD__
		CMutexLock m_lkChangeState; //may be not needed, if all the state relative function is handled in single thread.
		//CSpinLock m_lkcurrentstate;
#endif
		int32_t m_i32curstate; //current state;
		T *m_t;
		event_handler_t m_evt_handler;
	};

#include "state_machine.inl"

}

#endif /* __STATE_MACHINE_H__ */
