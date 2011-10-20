/*
 * mem_detector.h
 *
 *  Created on: May 22, 2009
 *      Author: rock
 */

#ifndef __MEM_DETECTOR_H__
#define __MEM_DETECTOR_H__

namespace nm_utils
{
	class CMemDetector
	{
	public:
		CMemDetector();
		virtual ~CMemDetector();

	private:
		int start();
		int stop();
	};

#define START_MEM_DETECTOR \
	nm_utils::CSingleton<nm_utils::CMemDetector>::instance();
}

#endif /* __MEM_DETECTOR_H__ */
