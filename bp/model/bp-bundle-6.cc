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

#include "bp-bundle-6.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("Bundle6");

namespace ns3 {

Bundle6::Bundle6(Ptr<Packet> adu)
: Bundle(adu) 
{
  NS_LOG_FUNCTION("bundle6 creation");
  m_primaryHeader = new BpHeader6();
  m_payloadHeader = new BpBlockHeader6(BpBlockHeader6::BUNDLE_PAYLOAD_BLOCK);
  m_primaryHeader->SetSourceEid(defaultEid);
  m_primaryHeader->SetDestinationEid(defaultEid);
  m_primaryHeader->SetReportEid(defaultEid);
  m_primaryHeader->SetCustEid(defaultEid);
}

Bundle6::~Bundle6() {
  free(m_payloadHeader);
  free(m_primaryHeader);
}

void
Bundle6::DoDispose(void) {
  Bundle::DoDispose();
  // pTODO dispose anything specific to v6 here
  // and move this custTx check to parent (and make a pure virual method for 
  // custodyRequested so it can be done differently in v6 vs v7)
  if (m_primaryHeader->CustTxReq()) Simulator::Remove(nextRetrans);
}

void 
Bundle6::SetPrimaryHeader(BpHeader* primaryHeader) {
  m_primaryHeader = dynamic_cast<BpHeader6*>(primaryHeader);
}

void
Bundle6::SetPayloadHeader(BpBlockHeader* payloadHeader) {
  m_payloadHeader = dynamic_cast<BpBlockHeader6*>(payloadHeader);
}

BpHeader6*
Bundle6::GetPrimaryHeader() {
  return m_primaryHeader;
}

BpBlockHeader6*
Bundle6::GetPayloadHeader() {
  return m_payloadHeader;
}

void
Bundle6::ClearEvents() {
  Bundle::ClearEvents();
  if (m_primaryHeader->CustTxReq() && nextRetrans.IsRunning()) {
    nextRetrans.Cancel();
  }
}

} // namespace ns3