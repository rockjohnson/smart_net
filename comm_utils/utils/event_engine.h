/*
 * event_engine.h
 *
 *  Created on: Mar 15, 2011
 *      Author: rock
 */

#ifndef __EVENT_ENGINE_H__
#define __EVENT_ENGINE_H__

#include <deque>

#include "../common/common.h"
#include "smart_ptr.h"
#include "smart_lock.h"

namespace nm_utils
{

/**
 *
 * */
class IEvent : public nm_base::ICommonBase
{
public:
	IEvent();
	virtual ~IEvent();

public:
	virtual void handle() = 0;
};
typedef nm_utils::CSmartPtr<nm_utils::IEvent> event_ptr_t;

/**
 *
 * */
class CEventHandleEngine
{
public:
	CEventHandleEngine();
	~CEventHandleEngine();

public:
	///
	void exec();
	///
	void post_event(const event_ptr_t &pevt);

private:
	CSpinLock m_lkcache;
	typedef std::deque<event_ptr_t> evt_deque_t;
	evt_deque_t m_dquecache;
	evt_deque_t m_dqueevts;
};

}

#endif /* __EVENT_ENGINE_H__ */
