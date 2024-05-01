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
#ifndef BP_TCP_CLA_H
#define BP_TCP_CLA_H

#include "bp-cla.h"

namespace ns3 {

class BpTcpCla : public BpCla
{
public:

  static TypeId GetTypeId (void);

  /**
   * \brief Constructor
   */
  BpTcpCla (Callback<void, Ptr<Bundle>> processBundleCallback = (Callback<void, Ptr<Bundle>>)0);

  /**
   * Destroy
   */
  virtual ~BpTcpCla ();

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
   * \brief connection succeed callback
   */
  void ConnectionSucceeded (Ptr<Socket> socket );

  /**
   * \brief connection fail callback
   */
  void ConnectionFailed (Ptr<Socket> socket);

  /**
   * \brief connection succeed callback
   */
  bool ConnectionRequest (Ptr<Socket>,const Address &);

  /**
   * \brief new connection created callback
   */
  void NewConnectionCreated (Ptr<Socket>, const Address &);
  
private:

  virtual TypeId GetSocketTypeId();

  /**
   * Set callbacks of the transport layer
   *
   * \param socket the transport layer socket
   */
  virtual void SetL4SocketCallbacks (Ptr<Socket> socket);
};

} // namespace ns3

#endif /* BP_TCP_CLA_H */

