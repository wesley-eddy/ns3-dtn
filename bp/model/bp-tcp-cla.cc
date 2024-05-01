/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2020 Wesley Eddy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Wesley Eddy <wes@mti-systems.com>
 *          Pierce Forte <pierceforte12@gmail.com>
 */

#include "bp-tcp-cla.h"
#include "ns3/tcp-socket-factory.h"

// default port number of dtn bundle tcp convergence layer, which is 
// defined in draft-irtf--dtnrg-tcp-clayer-0.6
#define DTN_BUNDLE_TCP_PORT 4556 

NS_LOG_COMPONENT_DEFINE ("BpTcpCla");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (BpTcpCla);

TypeId 
BpTcpCla::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpTcpCla")
    .SetParent<BpCla> ()
    .AddConstructor<BpTcpCla> ()
  ;
  return tid;
}

BpTcpCla::BpTcpCla (Callback<void, Ptr<Bundle>> processBundleCallback)
: BpCla(processBundleCallback)
{ 
  NS_LOG_FUNCTION (this);
}

BpTcpCla::~BpTcpCla ()
{ 
  NS_LOG_FUNCTION (this);
}

int
BpTcpCla::EnableReceive (const BpEndpointId &local, InetSocketAddress localAddress, Ptr<Node> bpNode)
{ 
  NS_LOG_FUNCTION (this << " " << local.Uri ());

  uint16_t port;
  InetSocketAddress defaultAddr ("127.0.0.1", 0);
  if (localAddress == defaultAddr)
    port = DTN_BUNDLE_TCP_PORT;
  else
    port = localAddress.GetPort ();

  InetSocketAddress address (Ipv4Address::GetAny (), port);

  // set tcp socket in listen state
  Ptr<Socket> socket = Socket::CreateSocket (bpNode, GetSocketTypeId());
  if (socket->Bind (address) < 0)
    return -1;
  if (socket->Listen () < 0)
    return -1;
  if (socket->ShutdownSend () < 0)
    return -1; 

  SetL4SocketCallbacks (socket);
 
  // store the sending socket so that the convergence layer can dispatch the hundles to different tcp connections
  std::map<BpEndpointId, Ptr<Socket> >::iterator it = m_l4RecvSockets.end ();
  it = m_l4RecvSockets.find (local);
  if (it == m_l4RecvSockets.end ())
    m_l4RecvSockets.insert (std::pair<BpEndpointId, Ptr<Socket> >(local, socket));  
  else
    return -1;


  return 0;
}

void 
BpTcpCla::SetL4SocketCallbacks (Ptr<Socket> socket)
{ 
  NS_LOG_FUNCTION (this << " " << socket);
  socket->SetConnectCallback (
    MakeCallback (&BpTcpCla::ConnectionSucceeded, this),
    MakeCallback (&BpTcpCla::ConnectionFailed, this));

  socket->SetCloseCallbacks (
    MakeCallback (&BpTcpCla::NormalClose, this),
    MakeCallback (&BpTcpCla::ErrorClose, this));

  socket->SetAcceptCallback (
    MakeCallback (&BpTcpCla::ConnectionRequest, this),
    MakeCallback (&BpTcpCla::NewConnectionCreated, this));

  socket->SetDataSentCallback (
    MakeCallback (&BpTcpCla::DataSent, this));

  socket->SetSendCallback (
    MakeCallback (&BpTcpCla::Sent, this));

  socket->SetRecvCallback (
    MakeCallback (&BpTcpCla::DataRecv, this));
}

void 
BpTcpCla::ConnectionSucceeded (Ptr<Socket> socket)
{ 
  NS_LOG_FUNCTION (this << " " << socket);
} 

void 
BpTcpCla::ConnectionFailed (Ptr<Socket> socket)
{ 
  NS_LOG_FUNCTION (this << " " << socket);
}

bool
BpTcpCla::ConnectionRequest (Ptr<Socket> socket, const Address &address)
{ 
  NS_LOG_FUNCTION (this << " " << socket << " " << address);
  return true;
}

void 
BpTcpCla::NewConnectionCreated (Ptr<Socket> socket, const Address &address)
{ 
  NS_LOG_FUNCTION (this << " " << socket << " " << address);
  SetL4SocketCallbacks (socket);  // reset the callbacks due to fork in TcpSocketBase
}

TypeId
BpTcpCla::GetSocketTypeId() {
  return TcpSocketFactory::GetTypeId();
}

} // namespace ns3
