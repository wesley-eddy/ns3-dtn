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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1306  USA
 *
 * Author: Pierce Forte <pierceforte12@gmail.com>
 */

#include "bp-bundle-7.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("Bundle7");

namespace ns3 {

Bundle7::Bundle7(Ptr<Packet> adu)
: Bundle(adu) 
{
  NS_LOG_FUNCTION("bundle7 creation");
  m_primaryHeader = new BpHeader7();
  m_payloadHeader = new BpBlockHeader7(BpBlockHeader7::BUNDLE_PAYLOAD_BLOCK);
  m_primaryHeader->SetSourceEid(defaultEid);
  m_primaryHeader->SetDestinationEid(defaultEid);
  m_primaryHeader->SetReportEid(defaultEid);
}

Bundle7::~Bundle7() {
  free(m_payloadHeader);
  free(m_primaryHeader);
}

void
Bundle7::DoDispose(void) {
  Bundle::DoDispose();
  // pTODO dispose anything specific to v7 here
}

void 
Bundle7::SetPrimaryHeader(BpHeader* primaryHeader) {
  m_primaryHeader = dynamic_cast<BpHeader7*>(primaryHeader);
}

void
Bundle7::SetPayloadHeader(BpBlockHeader* payloadHeader) {
  m_payloadHeader = dynamic_cast<BpBlockHeader7*>(payloadHeader);
}

BpHeader7*
Bundle7::GetPrimaryHeader() {
  return m_primaryHeader;
}

BpBlockHeader7*
Bundle7::GetPayloadHeader() {
  return m_payloadHeader;
}

} // namespace ns3