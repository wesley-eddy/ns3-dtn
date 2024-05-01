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

#include "bp-routing-agent.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("BpRoutingAgent");

namespace ns3 {

TypeId 
BpRoutingAgent::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpRoutingAgent")
    .SetParent<Object> ()
  ;
  return tid;
}

BpRoutingAgent::BpRoutingAgent ()
{ 
  NS_LOG_FUNCTION (this);
}

BpRoutingAgent::~BpRoutingAgent ()
{ 
  NS_LOG_FUNCTION (this);
}

} // namespace ns3
