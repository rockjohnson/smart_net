/*
 * event_engine.cpp
 *
 *  Created on: Mar 15, 2011
 *      Author: rock
 */

#include "event_engine.h"

namespace nm_utils
{

CEventHandleEngine::CEventHandleEngine()
{
}

CEventHandleEngine::~CEventHandleEngine()
{
}

void CEventHandleEngine::add_evt(const evt_ptr_t &pevt)
{
	spin_scopelk_t lk(m_lkcache);
	m_dquecache.push_back(pevt);
}

void CEventHandleEngine::exec()
{
	if (m_dquecache.empty())
	{
		return;
	}

	{
		SYS_ASSERT(m_dqueevts.empty());
		spin_scopelk_t lk(m_lkcache);
		m_dqueevts.swap(m_dquecache);
	}

	for (evt_deque_t::iterator iter = m_dqueevts.begin();
			iter != m_dqueevts.end(); iter++)
	{
		(*iter)->handle();
	}

	m_dqueevts.clear();
}

}
