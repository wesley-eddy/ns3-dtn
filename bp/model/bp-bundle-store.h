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
 *          Dizhi Zhou <dizhi.zhou@gmail.com>
 */
#ifndef BP_BUNDLE_STORE_H
#define BP_BUNDLE_STORE_H

#include "bp-bundle.h"
#include "bp-header.h"
#include "bp-cla.h"
#include "bp-endpoint-id.h"

namespace ns3 {

// pTODO: maybe move these flags (esp custody_accepted because it's unique to v6)
// Define retention constraint flags.
#define _BP_DISPATCH_PENDING 0x01
#define _BP_FORWARD_PENDING 0x02
#define _BP_CUSTODY_ACCEPTED 0x04

struct BundlePriorityCompare {
  bool operator()(const Ptr<Bundle>& lhs, const Ptr<Bundle>& rhs) {
    BpHeader *lhsHdr = lhs->GetPrimaryHeader();
    BpHeader *rhsHdr = rhs->GetPrimaryHeader();
    if (lhsHdr->Priority() != rhsHdr->Priority())
      return lhsHdr->Priority() < rhsHdr->Priority();
    if (lhsHdr->GetCreateTimestamp() != rhsHdr->GetCreateTimestamp())
      return lhsHdr->GetCreateTimestamp() < rhsHdr->GetCreateTimestamp();
    if (lhsHdr->GetSequenceNumber() != rhsHdr->GetSequenceNumber())
      return lhsHdr->GetSequenceNumber() < rhsHdr->GetSequenceNumber();
    if (lhsHdr->GetFragOffset() != rhsHdr->GetFragOffset())
      return lhsHdr->GetFragOffset() < rhsHdr->GetFragOffset();
    if (lhsHdr->GetSourceEid().Uri() != rhsHdr->GetSourceEid().Uri())
      return lhsHdr->GetSourceEid().Uri() < rhsHdr->GetSourceEid().Uri();
    return lhs < rhs;
  }
};

typedef std::set<Ptr<Bundle>, BundlePriorityCompare> storeType;

class BundleStore {
public:
  BundleStore(void) { maxBundlesStored = 0; }
  ~BundleStore(void) { m_store.clear(); }

  void Store(Ptr<Bundle> b);
  ssize_t GetMaxBundlesStored();
  ssize_t GetStoredByteCount();
  void Expire(Ptr<Bundle> b);
  void Remove(Ptr<Bundle> b);

  Ptr<Packet> GetBundleADU(const BpEndpointId &eid);
  Ptr<Packet> GetAndRemoveBundle(const BpEndpointId &eid, bool fragOk = true);
  Ptr<Bundle> GetBundle(const BpEndpointId &src, uint32_t ts, uint32_t seqno);
  void GetBundles(const BpEndpointId &src, uint32_t ts, uint32_t seqno, std::list<Ptr<Bundle>> &bundles);
  void GetForwardPendingBundles(std::list<Ptr<Bundle>> *bundles, Ptr<BpCla> cla, Callback<Ptr<BpCla>, BpEndpointId> outgoingClaCallback);

  void DebugDump();

private:
  //x std::deque<Ptr<Bundle>> m_store;
  storeType m_store;
  uint32_t maxBundlesStored;
  ssize_t m_storedBytes;
};

} // namespace ns3

#endif /* BP_BUNDLE_STORE_H */