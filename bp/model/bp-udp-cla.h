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
#ifndef BP_UDP_CLA_PROTOCOL_H
#define BP_UDP_CLA_PROTOCOL_H

#include "bp-cla.h"

namespace ns3 {

class BpUdpCla : public BpCla
{
public:

  static TypeId GetTypeId (void);

  BpUdpCla (Callback<void, Ptr<Bundle>> processBundleCallback = (Callback<void, Ptr<Bundle>>)0);
  virtual ~BpUdpCla ();

  /**
   * Enable the transport layer to receive packets
   *
   * \param local the endpoint id of registration
   * \param localAddress the address of the local endpoint id
   * \param bpNode the node of receiver bpAgent
   */
  virtual int EnableReceive (const BpEndpointId &local, InetSocketAddress localAddress, Ptr<Node> bpNode);

  /**
   *  Callbacks methods callded by NotifyXXX methods in TcpSocketBase
   *
   *  Those methods will further call the callback wrap methods in BpSocket
   *  The only exception is the DataRecv (), which will call BpSocket::Receive ()
   *
   *  The operations within those methods will be updated in the furture versions
   */

  /**
   * Set the local UDP port to listen on.
   */
  void SetPort(uint16_t port) { m_port = port; };

private:

  virtual TypeId GetSocketTypeId();

  /**
   * Set callbacks of the transport layer
   *
   * \param socket the transport layer socket
   */
  virtual void SetL4SocketCallbacks (Ptr<Socket> socket);

  uint16_t m_port;  // A local UDP port to listen on.
};

} // namespace ns3

#endif /* BP_UDP_CLA_PROTOCOL_H */

