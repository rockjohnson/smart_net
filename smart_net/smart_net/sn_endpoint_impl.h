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
#include "../framework/sn_mgr.h"
#include "../network/sn_socket.h"

namespace nm_smartnet
{

/**
 * tcp endpoint enum.
 * */
enum ETcpEndpointType
{
	E_TCP_INBOUND_ENDPOINT = 0,
	E_TCP_OUTBOUND_ENDPOINT
};

/**
 * tcp endpoint
 * */
enum
{
	ES_INIT = 0,
	ES_IN_ENGINE,
	ES_IN_ENGINE_AND_IN_RECV_THREAD,
	ES_IN_ENGINE_AND_IN_SEND_THREAD,
	ES_IN_ENGINE_RUNNING,
	ES_IN_ENGINE_STOP_RUNNING
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

class CTcpInboundEndpoint: public nm_framework::IEndpoint
{
public:
	CTcpInboundEndpoint();
	virtual ~CTcpInboundEndpoint();

public:
	///first, you should open this endpoint, but it is async. and if succeed, then on_opened will callback.
	virtual int32_t open(const nm_network::CIpv4Addr &listenaddr, const nm_network::CIpv4Addr &peeraddr, const nm_framework::smart_net_mgr_ptr_t psmartnetmgr);
	///if you want close this endpoint, please invoke this func, but it is aysnc.
	virtual int32_t close();
	///send data, async.
	virtual int32_t send_data(nm_memory::mem_ptr_t &pdata);
			int32_t get_type();
	nm_network::CIpv4Addr& get_listen_addr();

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
	nm_utils::CStateMachine<CTcpInboundEndpoint> m_sm;
	nm_utils::CMutexLock m_lkendpoint;
	volatile bool m_bopenned;
	nm_utils::CAtomicCounter<int32_t> m_enginerefcnt;

	nm_framework::smart_net_mgr_ptr_t m_psmartnetmgr;
	nm_network::tcp_sock_ptr_t m_psock;
	nm_network::CIpv4Addr m_listenaddr;
	nm_network::CIpv4Addr m_peeraddr;
};
typedef nm_utils::CSmartPtr<nm_smartnet::CTcpInboundEndpoint> tcp_ib_endpoint_ptr_t;

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

}

#endif /* CONN_H_ */
