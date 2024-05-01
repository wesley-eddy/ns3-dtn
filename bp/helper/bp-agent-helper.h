/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 University of New Brunswick
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
 * Author: Dizhi Zhou <dizhi.zhou@gmail.com>
 */

#ifndef BP_AGENT_HELPER_H
#define BP_AGENT_HELPER_H

#include <stdint.h>
#include <string>
#include "ns3/object-factory.h"
#include "ns3/attribute.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/bp-agent-container.h"
#include "ns3/bp-agent.h"
#include "ns3/bp-routing-agent.h"
#include "ns3/bp-endpoint-id.h"
#include "ns3/bp-udp-cla.h"

namespace ns3 {

/**
 * \brief A helper to make it easier to instantiate an ns3::BpAgent
 * on a set of nodes.
 */
class BpAgentHelper
{
public:
  /**
   * Create an BpAgentHelper to make it easier to work with BpAgent
   */
  BpAgentHelper ();

  /**
   * Install an ns3::BpAgent on each node of the input container
   * configured with all the attributes set with SetAttribute.
   *
   * \param c NodeContainer of the set of nodes on which an BpAgent
   * will be installed.
   *
   * \returns Container of Ptr to the BpAgents installed.
   */
  BpAgentContainer Install (NodeContainer c);

  /**
   * Install an ns3::BpAgent on the node configured with all the
   * attributes set with SetAttribute.
   *
   * \param node The node on which an BpAgent will be installed.
   * \returns Container of Ptr to the BpAgents installed.
   */
  BpAgentContainer Install (Ptr<Node> node);

  /**
   * Install an ns3::BpAgent on the node configured with all the
   * attributes set with SetAttribute.
   *
   * \param nodeName The node on which an BpAgent will be installed.
   * \returns Container of Ptr to the BpAgents installed.
   */
  BpAgentContainer Install (std::string nodeName);

  /**
   * Set endpoint id
   *
   * \param eid endpoint id
   */
  void SetBpEndpointId (BpEndpointId eid);

  /**
   * Set bundle routing agent
   *
   * \param rt bundle routing agent
   */
  void SetRoutingAgent (Ptr<BpRoutingAgent> rt);

private:
  /**
   * \internal
   * Install an ns3::BpAgent on the node
   *
   * \param node The node on which an BpAgent will be installed.
   * \returns Ptr to the BpAgent installed.
   */
  Ptr<BpAgent6> InstallPriv (Ptr<Node> node);

private:
  BpEndpointId m_eid;                        /// endpoint id
  Ptr<BpRoutingAgent> m_routingAgent;  /// bundle routing agent
};

} // namespace ns3

#endif /* BP_AGENT_HELPER_H */

