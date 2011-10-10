/*
 * conn.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef __CONN_H__
#define __CONN_H__

#include <utils/state_machine.h>
#include <memory/mem.h>

#include "../framework/sn_endpoint.h"
#include "../framework/sn_engine.h"
#include "../network/sn_socket_impl.h"

namespace nm_smartnet
{

	/**
	 * tcp endpoint enum.
	 * */
	enum ETcpEndpointType
	{
		E_TCP_INBOUND_ENDPOINT = 0, E_TCP_OUTBOUND_ENDPOINT
	};

	/**
	 * tcp endpoint
	 * */
	enum
	{
		ES_INIT = 0, ES_IN_ENGINE, ES_IN_ENGINE_AND_IN_RECV_THREAD, ES_IN_ENGINE_AND_IN_SEND_THREAD, ES_IN_ENGINE_RUNNING, ES_IN_ENGINE_STOP_RUNNING
	};

	enum
	{
		EE_ADD_INTO_ENGINE = 0,
		EE_DEL_FROM_ENGINE,
		EE_IN_ENGINE_ADD_INTO_SEND_THREAD,
		EE_IN_ENGINE_DEL_FROM_SEND_THREAD,
		EE_IN_ENGINE_ADD_INTO_RECV_THREAD,
		EE_IN_ENGINE_DEL_FROM_RECV_THREAD
	};

	class CTcpAcceptor: public nm_framework::IIoObj
	{
	public:
		CTcpAcceptor(const nm_framework::sn_engine_ptr_t&);
		virtual ~CTcpAcceptor();

		DISALLOW_COPY_AND_ASSIGN( CTcpAcceptor);

		enum
		{
			EES_OPENNED = 0, EES_CLOSED
		};

		enum
		{
			EEE_OPEN = 0, EEE_CLOSE
		};

	public:
		virtual void handle_input_evt();
		virtual void handle_output_evt();
		virtual void handle_error_evt();
		virtual void handle_inserted_to_ioset(int32_t i32IoType, int32_t i32ReturnCode);
		virtual void handle_erased_from_ioset(int32_t i32IoType);
		virtual int32_t get_fd();

		virtual u_int32_t get_io_evt(int32_t i32IoType);
		virtual void set_input_task_id(int32_t i32id);
		virtual int32_t get_input_task_id();
		virtual void set_output_task_id(int32_t i32id);
		virtual int32_t get_output_task_id();

	public:
		int32_t open(const cmn_string_t &strIP, u_int16_t ui16Port);
		int32_t close();

	private:
		int32_t opening(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, pvoid_t pVoid);
		int32_t closing(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, pvoid_t pVoid);

	private:
		nm_utils::CStateMachine<CTcpAcceptor> m_sm;
		nm_framework::sn_engine_ptr_t m_pSNEngine;
		nm_network::CTcpSock m_sock;
		int32_t m_i32InputTaskId;
		int32_t m_i32OutputTaskId;
	};
	typedef nm_utils::CSmartPtr<nm_smartnet::CTcpAcceptor> tcp_acceptor_ptr_t;

	/**
	 * tcp connect.
	 * */
	class CTcpConnector: public nm_framework::ITimerObj, public nm_framework::IIoObj
	{
	public:
		CTcpConnector(const nm_framework::sn_engine_ptr_t&);
		virtual ~CTcpConnector();

	public:
		virtual void handle_input_evt();
		virtual void handle_output_evt();
		virtual void handle_error_evt();
		virtual void handle_inserted_to_ioset(int32_t i32IoType, int32_t i32ReturnCode);
		virtual void handle_erased_from_ioset(int32_t i32IoType);
		virtual int32_t get_fd();

		virtual u_int32_t get_io_evt(int32_t i32IoType);
		virtual void set_input_task_id(int32_t i32id);
		virtual int32_t get_input_task_id();
		virtual void set_output_task_id(int32_t i32id);
		virtual int32_t get_output_task_id();

	public:
		virtual void check(u_int64_t ui64curtimeus);

	public:
		int32_t open(const cmn_string_t &strAcceptorIP, u_int16_t ui16AcceptorPort);
		int32_t close();
	};
	typedef nm_utils::CSmartPtr<nm_smartnet::CTcpConnector> tcp_connector_ptr_t;

	class CTcpEndpoint: public nm_framework::IIoObj
	{
	public:
		CTcpEndpoint();
		virtual ~CTcpEndpoint();

		DISALLOW_COPY_AND_ASSIGN( CTcpEndpoint);

	public:
		///first, you should open this endpoint, but it is async. and if succeed, then on_opened will callback.
		virtual int32_t open(const nm_network::CIpv4Addr &listenaddr, const nm_network::CIpv4Addr &peeraddr,
				const nm_framework::sn_engine_ptr_t psmartnetmgr);
		///if you want close this endpoint, please invoke this func, but it is aysnc.
		virtual int32_t close();
		///send data, async.
		virtual int32_t send_data(nm_memory::mem_ptr_t &pdata);
		int32_t get_type();
		nm_network::ipv4_addr_ptr_t& get_peer_addr() const;
		nm_network::ipv4_addr_ptr_t& get_local_addr() const;

	protected:
		virtual void handle_input_evt(); ///handle input event.
		virtual void handle_output_evt(); ///handle ouput event.
		virtual void handle_error_evt(); ///handle error event.
		///
		virtual void on_opened() = 0;
		virtual void on_recved_data(nm_memory::mem_ptr_t &pData) = 0;
		virtual void on_io_error(int32_t i32ErrCode) = 0;
		///
		int32_t get_fd();

	private:
		nm_utils::CStateMachine<CTcpEndpoint> m_sm;
		nm_utils::CMutexLock m_lkendpoint;
		volatile bool m_bopenned;
		nm_utils::CAtomicCounter<int32_t> m_enginerefcnt;

		nm_framework::sn_engine_ptr_t m_psmartnetmgr;
		nm_network::tcp_sock_ptr_t m_psock;
		nm_network::ipv4_addr_ptr_t m_plistenaddr;
		nm_network::ipv4_addr_ptr_t m_ppeeraddr;
	};
	typedef nm_utils::CSmartPtr<nm_smartnet::CTcpEndpoint> tcp_endpoint_ptr_t;

	/**
	 * tcp connect endpoint.
	 * */
	//class CTcpConnecter;
	//typedef nm_utils::CSmartPtr<CTcpConnecter> tcp_connecter_ptr_t;
	//class CTcpOutboundEndpoint : public CTcpInboundEndpoint
	//{
	//public:
	//	CTcpOutboundEndpoint();
	//	virtual ~CTcpOutboundEndpoint();
	//
	//public:
	//	int32_t start(net_addr_ptr_t &pRemoteAddr, int32_t i32ConnTimeout, int32_t i32MaxRetries);
	//	int32_t stop();
	//
	//protected:
	//	void on_closed(int32_t iErrCode);
	//
	//private:
	//	net_addr_ptr_t m_pRemoteAddr;
	//	tcp_connecter_ptr_t m_pConnecter;
	//};
	//typedef nm_utils::CSmartPtr<nm_smartnet::CTcpOutboundEndpoint> tcp_ob_endpoint_ptr_t;

	class CRmpEndpoint: public nm_framework::IIoObj
	{
	public:
		CRmpEndpoint(nm_framework::sn_engine_ptr_t &pNetEngine);
		virtual ~CRmpEndpoint();

		DISALLOW_COPY_AND_ASSIGN( CRmpEndpoint);

	public:
		int32_t open(const std::string &strMulticastIP, const std::string &strBindIP, u_int16_t ui16BindPort);
		int32_t close();
		int32_t send_data(nm_memory::mem_ptr_t &pData);

	protected:
		virtual void handle_input_evt(); ///handle input event.
		virtual void handle_output_evt(); ///handle ouput event.
		virtual void handle_error_evt(); ///handle error event.
		///
		virtual void on_opened() = 0;
		virtual void on_closed() = 0;
		virtual void on_error() = 0;
		virtual void on_recved_data(nm_memory::mem_ptr_t &pData) = 0;
	};

}

#endif /* CONN_H_ */
