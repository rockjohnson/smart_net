/*
 * conn.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef __CONN_H__
#define __CONN_H__

#include <deque>
#include <utils/state_machine.h>
#include <memory/mem.h>
#include <log/smart_log.h>
#include <network/sn_socket_impl.h>
#include "../framework/sn_engine.h"


namespace nm_smartnet
{
	/**
	 *
	 * */
	class CRupEndpoint;
	typedef nm_utils::CSmartPtr<nm_smartnet::CRupEndpoint> rup_endpoint_ptr_t;

	/**
	 *
	 * */
	class CRupAcceptor: public nm_framework::IIoObj
	{
	public:
		CRupAcceptor(const nm_framework::sn_engine_ptr_t&);
		virtual ~CRupAcceptor();

		DISALLOW_COPY_AND_ASSIGN( CRupAcceptor);

		enum
		{
			ES_OPENED = 0, ES_CLOSED, ES_ADDING_INTO_IT, ES_DELING_FROM_IT
		};

		enum
		{
			EE_NONE, EE_OPEN = 0, EE_CLOSE, EE_ADDED_INTO_IT, EE_DELED_FROM_IT, EE_INTERNAL_ERR
		};

	public:
		virtual void handle_input_evt();
		virtual void handle_output_evt();
		virtual void handle_io_error(int32_t i32ErrCode);
		virtual void handle_added_into_io_task(int32_t i32IoType, int32_t i32ReturnCode);
		virtual void handle_deled_from_io_task(int32_t i32IoType);
		virtual int32_t get_ioobj_handle();

	public:
		int32_t open(const cmn_string_t &strIP, u_int16_t ui16Port);
		bool is_opened()
		{
			return m_sm.get_cur_state() == ES_OPENED;
		}
		void on_opened();
		int32_t close();
		virtual void on_closed();
		int32_t add_endpoint(const rup_endpoint_ptr_t &pTcpEP);
		int32_t del_endpoint(const rup_endpoint_ptr_t &pTcpEP);
		nm_framework::sn_engine_ptr_t& get_engine()
		{
			return m_pSNEngine;
		}

	private:
		int32_t handling_closed_to_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_close_while_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_adding_into_it_to_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_close_while_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_internal_err_while_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_deling_from_it_to_closed(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_internal_err_while_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);

	private:
		nm_utils::CStateMachine<CRupAcceptor> m_sm;
		nm_framework::sn_engine_ptr_t m_pSNEngine;
		nm_network::rup_sock_ptr_t m_pTcpSockListener;
		int32_t m_i32PendingEvt;
		nm_utils::CSmartLog m_log;
		nm_network::CIpv4Addr m_bindAddr;

		nm_utils::CSpinLock m_lkIdleEndpoints;
		typedef std::deque<rup_endpoint_ptr_t> tcp_endpoint_deque_t;
		tcp_endpoint_deque_t m_dqIdleEndpoints;
	};
	typedef nm_utils::CSmartPtr<nm_smartnet::CRupAcceptor> tcp_acceptor_ptr_t;

	/**
	 * tcp connecter.
	 * */
	class CRupConnector: public nm_framework::ITimerObj, public nm_framework::IIoObj
	{
		friend class CRupEndpoint;

		enum
		{
			ES_OPENED = 0,
			ES_CLOSED,
			ES_CHECKING_TIMER,
			ES_ADDING_INTO_OT,
			ES_DELING_FROM_OT_CLOSE,
			ES_ADDING_INTO_TT,
			ES_DELING_FROM_TT,
			ES_CONNECTING,
			ES_DELING_FROM_OT_NORMAL
		};

		enum
		{
			EE_NONE,
			EE_OPEN = 0,
			EE_CONNECT,
			EE_CLOSE,
			EE_ADDED_INTO_OT,
			EE_DELED_FROM_OT,
			EE_INTERNAL_ERR,
			EE_ADDED_INTO_TT,
			EE_CONNECTED,
			EE_DELED_FROM_TT
		};

	public:
		CRupConnector(const nm_framework::sn_engine_ptr_t&);
		virtual ~CRupConnector();

		DISALLOW_COPY_AND_ASSIGN( CRupConnector);
	public:
		int32_t open(const cmn_string_t &strAcceptorIP, u_int16_t ui16AcceptorPort, u_int64_t ui64IntervalInUs);
		bool is_opened()
		{
			return m_sm.get_cur_state() == ES_OPENED;
		}
		int32_t close();

	protected:
		virtual void handle_input_evt();
		virtual void handle_output_evt();
		virtual void handle_io_error(int32_t i32ErrCode);
		virtual void handle_added_into_io_task(int32_t i32IoType, int32_t i32ReturnCode);
		virtual void handle_deled_from_io_task(int32_t i32IoType);
		virtual sock_handle_t get_ioobj_handle();

		virtual void handle_add_into_timer_task();
		virtual void handle_del_from_timer_task();

	private:
		int32_t add_endpoint(const rup_endpoint_ptr_t&);
		int32_t del_endpoint(const rup_endpoint_ptr_t&);
		nm_framework::sn_engine_ptr_t& get_engine()
		{
			return m_pSNEngine;
		}

		int32_t handling_closed_to_adding_into_tt(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_close_while_adding_into_tt(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_adding_into_tt_to_checking_timer(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_close_while_checking_timer(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_checking_timer_to_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_close_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_adding_into_ot_to_connecting(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_close_while_connecting(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_internal_err_while_connecting(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_connected(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_internal_err_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_deling_from_ot_close_to_deling_from_tt(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_close_while_deling_from_ot_normal(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_internal_err_while_deling_from_ot_normal(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_deling_from_ot_normal_to_checking_timer(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_deling_from_tt_to_closed(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);

	protected:
		virtual void on_timer();
		virtual void on_opened();
		virtual void on_closed();

	private:
		nm_network::rup_sock_ptr_t m_pTcpSock;
		cmn_string_t m_strAcceptorIp;
		u_int16_t m_ui16AcceptorPort;
		nm_utils::CStateMachine<CRupConnector> m_sm;
		u_int64_t m_ui64Interval;
		nm_framework::sn_engine_ptr_t m_pSNEngine;
		int32_t m_i32PendingEvt;
		nm_utils::CSpinLock m_lkIdleEps;
		typedef std::deque<rup_endpoint_ptr_t> tcp_endpoint_deque_t;
		tcp_endpoint_deque_t m_dqIdleEndpoints;
		nm_utils::CSmartLog m_log;
	};
	typedef nm_utils::CSmartPtr<nm_smartnet::CRupConnector> tcp_connector_ptr_t;

	/**
	 * tcp endpoint
	 * */
	class CRupEndpoint: public nm_framework::IIoObj
	{
		enum
		{
			ES_ADDED_INTO_HELPER = 0,
			ES_OPENED_READY,
			ES_OPENED,
			ES_CLOSING,
			ES_CLOSED_READY,
			ES_CLOSED,
			ES_ADDING_INTO_OT,
			ES_ADDING_INTO_IT,
			ES_DELED_FROM_IT,
			ES_DELING_FROM_IT,
			ES_DELING_FROM_OT
		};

		enum
		{
			EE_NONE = 0,
			EE_OPEN,
			EE_OPENED,
			EE_CLOSE,
			EE_CLOSED,
			EE_IOERR,
			EE_INTERNAL_ERR,
			EE_CONNECTED,
			EE_ADDED_INTO_OT,
			EE_ADDED_INTO_IT,
			EE_DELED_FROM_IT,
			EE_DELED_FROM_OT
		};

	public:
		CRupEndpoint(const tcp_acceptor_ptr_t&);
		CRupEndpoint(const tcp_connector_ptr_t&);
		virtual ~CRupEndpoint();

		DISALLOW_COPY_AND_ASSIGN( CRupEndpoint);

	protected:
		virtual void handle_input_evt();
		virtual void handle_output_evt();
		virtual void handle_io_error(int32_t i32ErrCode);
		virtual sock_handle_t get_ioobj_handle();
		virtual void handle_added_into_io_task(int32_t i32IoType, int32_t i32RetCode);
		virtual void handle_deled_from_io_task(int32_t i32IoType);

	public:
		///first, you should open this endpoint, but it is async. and if succeed, then on_opened will callback.
		virtual int32_t open();
		bool is_opened()
		{
			return m_sm.get_cur_state() == ES_OPENED;
		}
		///if you want close this endpoint, please invoke this func, but it is aysnc.
		virtual int32_t close();
		///send data, async.
		virtual int32_t send_data(nm_mem::mem_ptr_t &pData);
		//int32_t get_type();
		nm_network::ipv4_addr_ptr_t& get_peer_addr() const;
		nm_network::ipv4_addr_ptr_t& get_local_addr() const;
		int32_t handle_connected(nm_network::rup_sock_ptr_t&);

	protected:
		///
		virtual void on_opened() = 0;
		virtual void on_closed() = 0;
		virtual void on_recved_data(nm_mem::mem_ptr_t &pData) = 0;

	private:
		void init_sm();
		int32_t handling_closed_to_added_into_helper(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_added_into_helper_to_closed(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_close_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_internal_err_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_internal_err_while_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_close_while_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_internal_err_while_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_close_while_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_deling_from_it_to_deling_from_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_deling_from_ot_to_closed(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_adding_into_it_to_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_added_into_ot_to_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_added_into_helper_to_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);

	private:
		nm_utils::CStateMachine<CRupEndpoint> m_sm; ///state machine make the obj state thread-safe
		int32_t m_i32SMPendingEvt; ///为了简化设计，在状态达到OPENNED之前，发生的IO ERROR或 INTERNAL ERROR，并不直接处理，而是在达到这个状态时才处理。
		tcp_connector_ptr_t m_pTcpConnector;
		tcp_acceptor_ptr_t m_pTcpAcceptor;
		nm_framework::sn_engine_ptr_t m_pSNEngine;
		nm_network::rup_sock_ptr_t m_pTcpSock;
		nm_network::ipv4_addr_ptr_t m_pPeerAddr;
		nm_utils::CSmartLog m_log;
	};

	/**
	 * reliable multicast endpoint.
	 * */
	class CRmpEndpoint: public nm_framework::IIoObj
	{
		enum
		{
			ERMP_RECV_ENDPOINT = 0, ERMP_SEND_ENDPOINT
		};

		enum
		{
			ES_ADDED_INTO_HELPER = 0,
			ES_OPENED_READY,
			ES_OPENED,
			ES_CLOSING,
			ES_CLOSED_READY,
			ES_CLOSED,
			ES_ADDING_INTO_OT,
			ES_ADDING_INTO_IT,
			ES_DELED_FROM_IT,
			ES_DELING_FROM_IT,
			ES_DELING_FROM_OT
		};

		enum
		{
			EE_NONE = 0,
			EE_OPEN,
			EE_OPENED,
			EE_CLOSE,
			EE_CLOSED,
			EE_IOERR,
			EE_INTERNAL_ERR,
			EE_CONNECTED,
			EE_ADDED_INTO_OT,
			EE_ADDED_INTO_IT,
			EE_DELED_FROM_IT,
			EE_DELED_FROM_OT
		};

	public:
		CRmpEndpoint(nm_framework::sn_engine_ptr_t &pNetEngine, int32_t i32Type);
		virtual ~CRmpEndpoint();

		DISALLOW_COPY_AND_ASSIGN( CRmpEndpoint);

	public:
		int32_t open(const cmn_string_t &strMulticastIP, const cmn_string_t &strBindIP, u_int16_t ui16BindPort);
		int32_t close();
		int32_t send_data(nm_mem::mem_ptr_t &pData);

	protected:
		virtual void handle_input_evt(); ///handle input event.
		virtual void handle_output_evt(); ///handle ouput event.
		virtual void handle_error_evt(); ///handle error event.
		///
		virtual void on_opened();
		virtual void on_closed();
		virtual void on_error();
		virtual void on_recved_data(nm_mem::mem_ptr_t &pData) = 0;
	private:
		int32_t init_sm();

		int32_t handling_closed_to_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_close_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t handling_adding_into_ot_to_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);

	private:
		nm_utils::CStateMachine<nm_smartnet::CRmpEndpoint> m_sm;
		nm_framework::sn_engine_ptr_t m_pEngine;
		nm_network::rmp_sock_ptr_t m_pSock;
		cmn_string_t m_strBindIp;
		u_int16_t m_ui16BindPort;
		cmn_string_t m_strMulticastIp;
		int32_t m_i32Type;
	};
	typedef nm_utils::CSmartPtr<nm_smartnet::CRmpEndpoint> rmp_endpoint_ptr_t;
}

#endif /* CONN_H_ */
