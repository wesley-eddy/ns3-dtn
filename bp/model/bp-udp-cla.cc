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

#include "bp-udp-cla.h"
#include "ns3/udp-socket-factory.h"

#define DTN_BUNDLE_UDP_PORT 4556 

NS_LOG_COMPONENT_DEFINE ("BpUdpCla");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (BpUdpCla);

TypeId 
BpUdpCla::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpUdpCla")
    .SetParent<BpCla> ()
    .AddConstructor<BpUdpCla> ()
  ;
  return tid;
}

BpUdpCla::BpUdpCla (Callback<void, Ptr<Bundle>> processBundleCallback)
: BpCla(processBundleCallback),
  m_port (DTN_BUNDLE_UDP_PORT)
{ 
  NS_LOG_FUNCTION (this);
}

BpUdpCla::~BpUdpCla ()
{ 
  NS_LOG_FUNCTION (this);
}

int
BpUdpCla::EnableReceive (const BpEndpointId &local, InetSocketAddress localAddress, Ptr<Node> bpNode)
{ 
  NS_LOG_FUNCTION (this << " " << local.Uri ());
  
  uint16_t port = m_port;
  InetSocketAddress address (Ipv4Address::GetAny (), port);

  // set udp socket in listen state
  Ptr<Socket> socket = Socket::CreateSocket (bpNode, GetSocketTypeId());
  if (socket->Bind (address) < 0) {
    NS_LOG_DEBUG ("BpUdpCla::EnableReceive (): Udp Bind port " << port);
    return -1;
  }

  SetL4SocketCallbacks (socket);
 
  // store the sending socket so that the convergence layer can dispatch the hundles to different udp connections
  std::map<BpEndpointId, Ptr<Socket> >::iterator it = m_l4RecvSockets.end ();
  it = m_l4RecvSockets.find (local);
  if (it == m_l4RecvSockets.end ())
    m_l4RecvSockets.insert (std::pair<BpEndpointId, Ptr<Socket> >(local, socket));  
  else
    return -1;

  return 0;
}

void 
BpUdpCla::SetL4SocketCallbacks (Ptr<Socket> socket)
{ 
  NS_LOG_FUNCTION (this << " " << socket);

  socket->SetCloseCallbacks (
    MakeCallback (&BpUdpCla::NormalClose, this),
    MakeCallback (&BpUdpCla::ErrorClose, this));

  socket->SetDataSentCallback (
    MakeCallback (&BpUdpCla::DataSent, this));

  socket->SetSendCallback (
    MakeCallback (&BpUdpCla::Sent, this));

  socket->SetRecvCallback (
    MakeCallback (&BpUdpCla::DataRecv, this));
}

TypeId
BpUdpCla::GetSocketTypeId() {
  return UdpSocketFactory::GetTypeId();
}

} // namespace ns3
