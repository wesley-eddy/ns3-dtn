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
 * Authors: Pierce Forte <pierceforte12@gmail.com>
 *          Wesley Eddy <wes@mti-systems.com>
 */
#ifndef BP_BUNDLE_H
#define BP_BUNDLE_H

#include "ns3/packet.h"
#include "bp-endpoint-id.h"
#include "ns3/sequence-number.h"
#include "ns3/event-id.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "bp-header.h"
#include "bp-block-header.h"

namespace ns3 {

// pTODO: should remove this variable here 
// and reference from bp-agent.h?
// Commonly referenced default endpoint ID.
static BpEndpointId defaultEid = BpEndpointId("dtn:none");

class Bundle : public SimpleRefCount<Bundle> {
public:
  Bundle(Ptr<Packet> adu);

  virtual ~Bundle();

  static TypeId GetTypeId(void);

  virtual void DoDispose(void);

  virtual void SetPrimaryHeader(BpHeader* primaryHeader) = 0;

  virtual void SetPayloadHeader(BpBlockHeader* payloadHeader) = 0;

  virtual BpHeader* GetPrimaryHeader() = 0;

  virtual BpBlockHeader* GetPayloadHeader() = 0;

  virtual void ClearEvents();

  /*
  pTODO these methods might be helpful for enabling/disabling cust tx, but it may be okay 
  to remove them from here since places dealing w/ cust tx with bundles should know if that
  bundle is v6 or v7 (and the right way to enable/disable cust tx for that version)

  virtual void SetCustTxReq (const bool value) = 0;

  virtual bool CustTxReq () const = 0;
  */


  uint32_t retentionConstraints;

  Ptr<Packet> m_adu;

  // TODO don't assume block ordering
  //  bool bundleHdrPresent, ctebPresent, payloadHdrPresent;
  //  CTEB cteb;

  EventId expireEvent;

  std::list<std::pair<uint32_t, uint32_t>> acks;
};

} // namespace ns3

#endif /* BP_BUNDLE_H */