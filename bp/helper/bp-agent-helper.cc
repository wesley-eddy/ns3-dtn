/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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

#include "bp-agent-helper.h"
#include "ns3/string.h"
#include "ns3/names.h"
#include "ns3/simulator.h"


namespace ns3 {

BpAgentHelper::BpAgentHelper ()
  : m_eid ("dtn:none"),
    m_routingAgent (0)
{
}

BpAgentContainer
BpAgentHelper::Install (Ptr<Node> node)
{
  return BpAgentContainer (InstallPriv (node));
}

BpAgentContainer
BpAgentHelper::Install (std::string nodeName)
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return BpAgentContainer (InstallPriv (node));
}

BpAgentContainer
BpAgentHelper::Install (NodeContainer c)
{
  BpAgentContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<BpAgent6>
BpAgentHelper::InstallPriv (Ptr<Node> node)
{
  if (m_eid.Uri () == "dtn:none")
    NS_FATAL_ERROR ("BpAgentHelper::InstallPriv (): do not have endpoint id!");
  if (m_routingAgent == 0)
    NS_FATAL_ERROR ("BpAgentHelper::InstallPriv (): do not have bundle routing agent! " << m_eid.Uri ());

  Ptr<BpAgent6> bpAgent = CreateObject<BpAgent6> ();
  bpAgent->Open (node);   
  bpAgent->SetBpEndpointId (m_eid);
  bpAgent->SetRoutingAgent (m_routingAgent);
  Simulator::Schedule (Seconds (0.0), &BpAgent6::Initialize, bpAgent);

  return bpAgent;
}

void 
BpAgentHelper::SetBpEndpointId (BpEndpointId eid)
{
  m_eid = eid;
}

void 
BpAgentHelper::SetRoutingAgent (Ptr<BpRoutingAgent> rt)
{
  m_routingAgent = rt;
}


} // namespace ns3
