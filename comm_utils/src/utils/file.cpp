/*
 * file.cpp
 *
 *  Created on: Jan 7, 2011
 *      Author: rock
 */

#include "file.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "utils.h"

namespace nm_utils
{
	CFile::CFile()
	{
		// TODO Auto-generated constructor stub
	}

	CFile::~CFile()
	{
		// TODO Auto-generated destructor stub
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CLiteFile::CLiteFile()
	: m_i32FileDesc(-1)
	{
	}

	CLiteFile::~CLiteFile()
	{
		close();
	}

#define CREATE_MODE (0777)
	int32_t CLiteFile::create(const cmn_string_t &strFullName)
	{
		SYS_ASSERT(0 > m_i32FileDesc);
		///int open (const char *FILENAME, int FLAGS[, mode_t MODE])
		///argument MODE is used only when a file is created, but it doesn't
		///hurt to supply the argument in any case.
		///The FLAGS argument controls how the file is to be opened, This is a bit mask
		if ("" == strFullName)
		{
			return RET_ERR;
		}

		cmn_string_t strDir, strFile;
		int iRet = nm_utils::split_file_name(strFullName, strDir, strFile);
		if (0 <= iRet)
		{
			nm_utils::create_recursive_dir_ex(strDir.c_str(), CREATE_MODE);
		}

		return (m_i32FileDesc = ::open(strFullName.c_str(), O_CREAT|O_RDWR|O_TRUNC|O_EXCL), S_IRWXU) < 0 ? -1 : RET_SUC;
	}

	int32_t CLiteFile::open(const cmn_string_t &strFullName, int32_t i32Mode)
	{
		SYS_ASSERT(0 > m_i32FileDesc);

		if ("" == strFullName)
		{
			return RET_ERR;
		}

		return (m_i32FileDesc = ::open(strFullName.c_str(), O_RDWR|i32Mode)) < 0 ? -1 : RET_SUC;
	}

	int32_t CLiteFile::close()
	{
		int32_t iRet = RET_SUC;
		if (0 <= m_i32FileDesc)
		{
			iRet = ::close(m_i32FileDesc);
			m_i32FileDesc = -1;
		}

		return iRet;
	}

	int32_t CLiteFile::set(cmn_byte_t *pBytes, u_int32_t ui32Len)
	{
		SYS_ASSERT(0 <= m_i32FileDesc);

		return write(m_i32FileDesc, static_cast<void*>(pBytes), ui32Len);
	}

	int32_t CLiteFile::flush()
	{
		SYS_ASSERT(0 <= m_i32FileDesc);

		return fsync(m_i32FileDesc);
	}

	int32_t CLiteFile::get(cmn_byte_t *pBuf, u_int32_t ui32Size)
	{
		SYS_ASSERT(0 <= m_i32FileDesc /*&& SSIZE_MAX >= ui32Size*/);

		///On  success,  the  number  of bytes read is returned (zero indicates end of file), and the file position is advanced by this number.  It is not an
	    ///error if this number is smaller than the number of bytes requested; this may happen for example because fewer bytes are actually  available  right
	    ///now  (maybe because we were close to end-of-file, or because we are reading from a pipe, or from a terminal), or because read() was interrupted by a signal
		return read(m_i32FileDesc, static_cast<void*>(pBuf), ui32Size);
	}

	bool CLiteFile::is_open()
	{
		return 0 <= m_i32FileDesc;
	}
}
