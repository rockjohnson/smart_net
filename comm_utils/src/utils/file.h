/*
 * file.h
 *
 *  Created on: Jan 7, 2011
 *      Author: rock
 */

#ifndef __XXX_FILE_H__
#define __XXX_FILE_H__

#include "../common/common.h"

namespace nm_utils
{
	/**
	 * base file class
	 * */
	class CFile
	{
		public:
			CFile();
			virtual ~CFile();

		public:
			virtual int32_t create(const cmn_string_t &strFullName) = 0;
			virtual int32_t open(const cmn_string_t &strFullName, int32_t i32Mode = 0) = 0;
			virtual int32_t close() = 0;
			virtual int32_t set(byte_t *pBytes, u_int32_t ui32Len) = 0;
			virtual int32_t get(byte_t *pBuf, u_int32_t ui32Size) = 0;
			virtual int32_t flush() = 0;
			virtual bool is_open() = 0;
	};

	/**
	 * ...
	 * */
	class CLiteFile : public CFile
	{
		public:
			CLiteFile();
			virtual ~CLiteFile();

		public:
			int32_t create(const cmn_string_t &strFullName);
			int32_t open(const cmn_string_t &strFullName, int32_t i32Mode = 0);
			int32_t close();
			int32_t set(byte_t *pBytes, u_int32_t ui32Len);
			int32_t get(byte_t *pBuf, u_int32_t ui32Size);
			int32_t flush();
			bool is_open();

		private:
			int32_t m_i32FileDesc;
	};
}

#endif /* __FILE_H__ */
