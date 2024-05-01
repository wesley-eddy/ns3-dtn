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
 *          Dizhi Zhou <dizhi.zhou@gmail.com>
 */
#ifndef BP_ROUTING_AGENT_H
#define BP_ROUTING_AGENT_H

#include "ns3/object.h"

#include "bp-endpoint-id.h"
#include "bp-cla.h"

namespace ns3 {

/**
 * \brief This is an abstract base class of bundle routing agent
 *
 */
class BpRoutingAgent : public Object
{
public: 
  static TypeId GetTypeId (void);

  /**
   * Constructor
   */
  BpRoutingAgent ();

  /**
   * Destroy
   */
  virtual ~BpRoutingAgent ();

  /**
   * \brief Return the EID to be used as the next-hop node for a destination.
   *
   * \param dst Destination Endpoint ID
   */
  virtual BpEndpointId NextHopEid(BpEndpointId &dst) = 0;

  /**
   * \brief Return the CLA used to reach a particular next-hop EID.
   *
   * \param eid Next-hop Endpoint ID
   */
  virtual Ptr<BpCla> NextHopCla(BpEndpointId &eid) = 0;
};


}  // namespace ns3

#endif /* BP_ROUTING_AGENT_H */
