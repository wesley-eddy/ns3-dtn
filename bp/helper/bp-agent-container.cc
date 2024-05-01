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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 * Modified by Dizhi Zhou <dizhi.zhou@gmail.com>
 */

#include "ns3/names.h"
#include "bp-agent-container.h"

namespace ns3 {

BpAgentContainer::BpAgentContainer ()
{
}

BpAgentContainer::BpAgentContainer (Ptr<BpAgent6> bpAgent)
{
  m_bpAgents.push_back (bpAgent);
}

BpAgentContainer::BpAgentContainer (std::string name)
{
  Ptr<BpAgent6> bpAgent = Names::Find<BpAgent6> (name);
  m_bpAgents.push_back (bpAgent);
}


BpAgentContainer::Iterator 
BpAgentContainer::Begin (void) const
{
  return m_bpAgents.begin ();
}
BpAgentContainer::Iterator 
BpAgentContainer::End (void) const
{
  return m_bpAgents.end ();
}

uint32_t 
BpAgentContainer::GetN (void) const
{
  return m_bpAgents.size ();
}

Ptr<BpAgent6> 
BpAgentContainer::Get (uint32_t i) const
{
  return m_bpAgents[i];
}

void 
BpAgentContainer::Add (BpAgentContainer other)
{
  for (Iterator i = other.Begin (); i != other.End (); i++)
    {
      m_bpAgents.push_back (*i);
    }
}
void 
BpAgentContainer::Add (Ptr<BpAgent6> bpAgent)
{
  m_bpAgents.push_back (bpAgent);
}
void 
BpAgentContainer::Add (std::string name)
{
  Ptr<BpAgent6> bpAgent = Names::Find<BpAgent6> (name);
  m_bpAgents.push_back (bpAgent);
}

void 
BpAgentContainer::Start (Time start)
{
  for (Iterator i = Begin (); i != End (); ++i)
    {
      Ptr<BpAgent> bp = *i;
      bp->SetStartTime (start);
    }
}

void 
BpAgentContainer::Stop (Time stop)
{
  for (Iterator i = Begin (); i != End (); ++i)
    {
      Ptr<BpAgent> bp = *i;
      bp->SetStopTime (stop);
    }
}


} // namespace ns3
