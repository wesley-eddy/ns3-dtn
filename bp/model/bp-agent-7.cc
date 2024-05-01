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
 * Author: Pierce Forte <pierceforte12@gmail.com>
 */

#include "bp-agent-7.h"

NS_LOG_COMPONENT_DEFINE ("BpAgent7");

namespace ns3 {

BpAgent7::BpAgent7 ()
  /*: BpAgent()*/
{ 
  NS_LOG_FUNCTION (this);
}

BpAgent7::~BpAgent7 ()
{ 
  NS_LOG_FUNCTION (this);

  //pTODO implement
}

TypeId
BpAgent7::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpAgent7")
    .SetParent<BpAgent> ()
    .AddConstructor<BpAgent7> ();
  return tid;
}

int 
BpAgent7::Send (Ptr<Packet> p, const BpEndpointId &src, const BpEndpointId &dst, const Time &lifetime, bool custody, uint32_t priority)
{ 
  NS_LOG_FUNCTION (this << " " << src.Uri () << " " << dst.Uri ());

  //pTODO implement

  return 0;
}

void BpAgent7::Forward(Bundle* b) {
  //pTODO implement
}

int BpAgent7::EnqueueForDeliveryToApplication(Bundle* b) {
  //pTODO implement

  return 0;
}

void BpAgent7::Deliver(Bundle* b) {
  NS_LOG_FUNCTION(this);
  
  //pTODO implement
}

} // namespace ns3