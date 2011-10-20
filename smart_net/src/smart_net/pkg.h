/*
 * pkg.h
 *
 *  Created on: Apr 24, 2009
 *      Author: rock
 */

#ifndef __PKG_H__
#define __PKG_H__

#include "defines.h"
#include <mem.h>
#ifdef __PLATEFORM_LINUX__
#include <arpa/inet.h>
#endif
#include "type.h"

namespace nm_pkg
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DECL_OPCODE(opcode) \
		public:\
		enum {PKG_OPCODE = opcode};\
		static int get_opcode() \
		{ \
			return PKG_OPCODE; \
			}
#define DECL_PKG(pkg_cls) \
public:\
	static ui32_t get_max_size() \
	{ \
		return sizeof(pkg_cls); \
	} \
	ui32_t get_real_size() \
	{ \
		return sizeof(pkg_cls); \
	}

//#define SET_PKG_HANDLER(CONN, PKG)
//		DISPATCHER(CONN).reg_fun((int)PKG::PKG_OPCODE, &CDispMgr<nm_utils::CSmartPtr<CONN> >::dispatch_fun<PKG>)

#define PROCESS_PROTOCAL(PKG) \
	int process(PKG &pkg, ui32_t uiPkgTag)

#define PROCESS_UDP_PROTOCAL(PKG) \
	int process(mem_ptr_t &pPkgMem, PKG &pkg, ui32_t uiSrcIp, us16_t usSrcPort, ui32_t uiPkgTag)

#define VERSION 0x00
//#define CHECK_CODE 0x0
#define PKG_SIZE_TYPE_SIZE sizeof(ui32_t)
#define MINI_PKG_HDR_SIZE (PKG_SIZE_TYPE_SIZE + 1)  /*bytes*/ //����Ϣϵͳ�е�CPkgHdr�еģ�ǰ4���ֽ��Ǳ���Ϣ���ȣ���5���ֽ�Ϊ��Ϣ����

#define NORMAL_PKG 0x00

#define __USING_COMPRESSED_DATA__

#ifdef __USING_COMPRESSED_DATA__
#define COMPRESSED_PKG 0x01
#define __PKG_COMPRESSED_THRESHOLD__ 16 /*bytes*/
#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	using namespace nm_utils;

//#pragma pack(4)
#pragma pack(push, 4)

	//package header
	class CPkgHdr
	{
	public:
		CPkgHdr(us16_t usOpcode, ui32_t uiLen, byte_t bVer = VERSION, byte_t bType = NORMAL_PKG,
			/*byte_t bChk = CHECK_CODE,*/ ui32_t uiTag = 0/*0 means not used in this msg*/)
			:m_uiLen(HTONL(uiLen)), m_bType(bType), m_bVer(bVer)
			, /*m_bChk(bChk),*/ m_usOpcode(HTONS(usOpcode)),m_uiTag(uiTag)
		{}
		~CPkgHdr(){}

		us16_t get_opcode(){return NTOHS(m_usOpcode);}
		byte_t get_ver(){return m_bVer;}
		//byte_t get_chkcd(){return m_bChk;}
		void set_len(ui32_t uiLen)
		{
			m_uiLen = HTONL(uiLen);
		}
		ui32_t get_len()
		{
			return NTOHL(m_uiLen);
		}

		void set_tag(ui32_t uiTag)
		{
			m_uiTag = uiTag;
		}

		ui32_t get_tag()
		{
			return m_uiTag;
		}

		byte_t get_type()
		{
			return m_bType;
		}

		void set_type(byte_t bType)
		{
			m_bType = bType;
		}

	protected:
		//all are in network byte order
		ui32_t m_uiLen;    //total length, must be the first member
		byte_t m_bType;    //0: normal, 1: compressed....
		byte_t m_bVer;     //version & check code
		us16_t m_usOpcode; //operate code
		ui32_t m_uiTag;    //the tag which can uniquely identify one msg, host byte order is enough; 0 means no use;
		//byte_t m_bChk;     //check code
	};

	//pay attention, in fact, the following definitions are package bodys, not including package header, the header is CPkgHdr;
	//and the the size of package body must be constant, if the content is too big to push into one package body, please using
	//two or more package body.
	//the size of package body must less than MAX_MEM_SIZE

//	class CPkgReg
//	{
//		DECL_OPCODE(EP_REG)
//	public:
//		CPkgReg(ui32_t uiID)
//		:m_uiID(uiID){}
//
//	public:
//		static ui32_t get_max_size()
//		{
//			return sizeof(CPkgReg);
//		}
//		ui32_t get_real_size()
//		{
//			return sizeof(CPkgReg);
//		}
//
//		ui32_t get_id()
//		{
//			return m_uiID;
//		}
//
//		void set_id(int iId)
//		{
//			m_uiID = iId;
//		}
////		void process(chat_ptr_t &client)
////		{
////			;
////		}
//	private:
//		ui32_t m_uiID;
//		char buf[20];
//	};
//
//	//��Ϊ����Ϣϵͳ�ĵ�����Ϣ��Ĵ�С����С��MAX_MEM_SIZE�����������¶���
//#define MAX_CONTENT_SIZE(DEF_MAX_SIZE, PKG_BDY_HDR_SIZE)
//	(DEF_MAX_SIZE > (MAX_MEM_SIZE-PKG_BDY_HDR_SIZE)
//	? (MAX_MEM_SIZE-PKG_BDY_HDR_SIZE) : DEF_MAX_SIZE)
//
//#define USER_DEFINED_MAX_CONTENT_SIZE (4096)  //�û������ҵ���ϵĵ����������ֵ
//#define UNICAST_CONTENT_MAX_SIZE MAX_CONTENT_SIZE(USER_DEFINED_MAX_CONTENT_SIZE, sizeof(CPkgUnicast))
//	class CPkgUnicast
//	{
//		DECL_OPCODE(EP_UNICAST)
//	public:
//		static ui32_t get_max_size()
//		{
//			return sizeof(CPkgUnicast) + UNICAST_CONTENT_MAX_SIZE;
//		}
//		ui32_t get_real_size()
//		{
//			return sizeof(CPkgUnicast) + ntohs(m_usDataLen);
//		}
//
//		void set_data_len(us16_t usLen)
//		{
//			m_usDataLen = htons(usLen);
//		}
//		us16_t get_data_len()
//		{
//			return ntohs(m_usDataLen);
//		}
//
//		byte_t* get_data()
//		{
//			return m_bytes;
//		}
//
//		void set_data(const char *pBytes, us16_t usBytes)
//		{
//			ASSERT(usBytes <= (UNICAST_CONTENT_MAX_SIZE-1));
//			memcpy(m_bytes, pBytes, usBytes);
//			m_bytes[usBytes] = '\0';
//			m_usDataLen = htons(usBytes+/*NULL*/1);
//		}
//	private:
//		ui32_t m_uiDestID;
//		us16_t m_usDataLen; //data length, network byte order
//		byte_t m_bytes[0];
//	};

//#pragma pack()
#pragma pack(pop)


//	class CProtocalMgr
//	{
//	public:
//		void init()
//		{
//			//
//			SET_PKG_HANDLER(CLocalConn, CPkgReg);
//			SET_PKG_HANDLER(CLocalConn, CPkgUnicast);
//
//			//
//			SET_PKG_HANDLER(CUpperConn, CPkgReg);
//			SET_PKG_HANDLER(CUpperConn, CPkgUnicast);
//		}
//	};
//
//#define REGISTER_PROTOCOLS
//	nm_utils::CSingleton<nm_busi::CProtocalMgr>::instance().init();

}

#endif /* __PKG_H__ */
