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
 *          Dizhi Zhou <dizhi.zhou@gmail.com>
 */
#ifndef BP_STATIC_ROUTING_PROTOCOL_H
#define BP_STATIC_ROUTING_PROTOCOL_H

#include "bp-routing-agent.h"
#include "ns3/inet-socket-address.h"
#include <map>

namespace ns3 {

struct dtnRoute {
  BpEndpointId dstEid,     // bundle destination endpoint ID
               nxtHop;     // next hop to use
  bool up;                 // whether route is active or not
  Ipv4Address addr;
  uint16_t port;
  Ptr<BpCla> cla;  // convergence layer instance to use

  std::string note;
};

/**
 * \brief This is an abstract base class of bundle routing agent
 *
 */
class BpStaticRoutingAgent : public BpRoutingAgent
{
public: 
  static TypeId GetTypeId (void);

  /**
   * Constructor
   */
  BpStaticRoutingAgent ();

  /**
   * Destroy
   */
  virtual ~BpStaticRoutingAgent ();

  virtual BpEndpointId NextHopEid(BpEndpointId &dst);
  virtual Ptr<BpCla> NextHopCla(BpEndpointId &eid);

  virtual void UpRoute (BpEndpointId &dst, BpEndpointId &nxt, std::string *note);
  virtual void DownRoute (BpEndpointId &dst, BpEndpointId &nxt, std::string *note);

  virtual int AddRoute (BpEndpointId &dst, BpEndpointId &nxt, bool up, Ipv4Address addr, uint16_t port, Ptr<BpCla> cla, std::string *note=NULL);

  /**
   *  \return the internet socket address of matched eid; If there is no 
   *  match route, return the 127.0.0.1 with port 0
   */
  virtual InetSocketAddress GetRoute (BpEndpointId eid);

private:
  std::map <BpEndpointId, std::list<struct dtnRoute>*> m_routes;
};



}  // namespace ns3

#endif /* BP_STATIC_ROUTING_PROTOCOL_H */
