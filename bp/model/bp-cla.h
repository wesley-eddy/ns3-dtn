/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) Pierce Forte
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
 *          Carter T. Edmond <carter.t.edmond1@gmail.com>
 */
#ifndef BP_CLA_H
#define BP_CLA_H

#include "ns3/object.h"
#include "ns3/socket.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/bp-endpoint-id.h"
#include "ns3/bp-bundle-6.h"
#include "ns3/bp-bundle-7.h"
#include "ns3/inet-socket-address.h"
#include "ns3/log.h"

namespace ns3 {

/**
 * \brief CLA protocol abstract base class 
 *
 * This is an abstract base class for CLA protocol of BP layer 
 */
class BpCla : public Object
{
public:

  static TypeId GetTypeId (void);

  /**
   * Constructor
   */
  BpCla (Callback<void, Ptr<Bundle>> processBundleCallback);

  /**
   * Destroy
   */
  virtual ~BpCla ();

  /**
   * send packet to the transport layer
   *
   * \param packet packet to send
   * \param dstAddress the address of the destination endpoint id
   * \param bpNode the node of sender bpAgent
   */
  virtual int SendPacket (Ptr<Packet> packet, InetSocketAddress dstAddress, Ptr<Node> bpNode);

  /**
   * Send a bundle, the CLA will handle all encoding of headers.
   *
   * \param bundle to be sent
   * \param size number of bytes of bundle ADU to be sent
   * \param offset offest to read ADU bytes from
   * \param dstAddress the address of the destination endpoint id
   * \param bpNode the node of sender bpAgent
   */
  virtual int SendBundle (Ptr<Bundle6> bundle, InetSocketAddress dstAddress, Ptr<Node> bpNode);

  /**
   * Enable the transport layer to receive packets
   *
   * \param local the endpoint id of registration
   * \param localAddress the address of the local endpoint id
   * \param bpNode the node of receiver bpAgent
   */
  virtual int EnableReceive (const BpEndpointId &local, InetSocketAddress localAddress, Ptr<Node> bpNode) = 0;

  /**
   * Disable the transport layer to receive packets
   *
   * \param local the endpoint id of registration
   */
  virtual int DisableReceive (const BpEndpointId &local);

  /**
   * Enable this bundle node to send bundles at the transport layer
   *
   * \param src the source endpoint id
   * \param dst the destination endpoint id
   * \param dstAddress the address of the destination endpoint id
   * \param bpNode the node of the sender bpAgent
   */
  virtual int EnableSend (const BpEndpointId &src, const BpEndpointId &dst, InetSocketAddress dstAddress, Ptr<Node> bpNode);

  /**
   * Get the transport layer socket
   *
   * \param packet the bundle required to be transmitted
   * \param dstAddress the address of the destination endpoint id
   * \param bpNode the node of the sender bpAgent
   *
   * \return the sender socket
   */
  virtual Ptr<Socket> GetL4Socket (Ptr<Packet> packet, InetSocketAddress dstAddress, Ptr<Node> bpNode);

  /**
   * \brief normal close callback
   */
  virtual void NormalClose (Ptr<Socket> socket);

  /**
   * \brief error close callback
   */
  virtual void ErrorClose (Ptr<Socket> socket);

  /**
   * \brief data sent callback
   */
  virtual void DataSent (Ptr<Socket>,uint32_t size);

  /**
   * \brief sent callback
   */
  virtual void Sent (Ptr<Socket>,uint32_t size);

  /**
   * \brief data receive callback
   */
  virtual void DataRecv (Ptr<Socket> socket);

  virtual bool IsReady() { return m_ready; }

  virtual void SetReady(bool ready);

  /**
   * Enable Compressed Bundle Header Encoding (CBHE).
   */
  virtual void UseCbhe();

  /**
   * Enable Compressed Bundle Header Encoding (CBHE).
   */
  virtual bool UsesCbhe();

  /**
   * \brief Builds serialized bundle as CBOR indefinite array for BPv7
   * 
   * \param bundle bundle to serialize
   */
  static Ptr<Packet> SerializeBundle(Ptr<Bundle7> bundle); //TODO static modifier for testing purposes; remove later
  
  /**
   * \brief Decodes packet at reception and constructs bundle from contents 
   * 
   * \param packet the encoded bundle to deserialize
   */
  static Ptr<Bundle7> DeserializeBundle(Ptr<Packet> packet); //TODO static modifier for testing purposes; remove later
  
protected:

  std::map<BpEndpointId, Ptr<Socket> > m_l4SendSockets; /// the transport layer sender sockets
  std::map<BpEndpointId, Ptr<Socket> > m_l4RecvSockets; /// the transport layer receiver sockets

  /**
   * Set callbacks of the transport layer
   *
   * \param socket the transport layer socket
   */
  virtual void SetL4SocketCallbacks (Ptr<Socket> socket) = 0;

  virtual TypeId GetSocketTypeId() = 0;

private:

  bool m_ready;

  bool m_cbhe; // Set to true if CBHE should be used.

  Callback<void, Ptr<Bundle>> m_processBundleCallback;

  uint8_t m_tos; // IP ToS value used in sending packets.
};

} // namespace ns3

#endif /* BP_CLA_H */

