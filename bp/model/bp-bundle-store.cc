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

#include "bp-bundle-store.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE ("BundleStore");

namespace ns3 {

void BundleStore::Store(Ptr<Bundle> b) {
  BpHeader *header = b->GetPrimaryHeader();
  Time lifetime = header->GetLifeTime();
  // Setting lifetime to 0 is a special way in the simulator to never expire.
  if (lifetime != 0) {
    // Create an event to destroy the bundle when it expires.
    Time timeLeft = (Seconds(header->GetCreateTimestamp()) + lifetime) - Simulator::Now();
    if (timeLeft.IsPositive()) {
      EventId evid = Simulator::Schedule (timeLeft, &BundleStore::Expire, this, b);
      b->expireEvent = evid;
    } else {
      NS_LOG_DEBUG("bundle is already expired!");
      return;
    }
  }
  m_store.insert(b);

  if (m_store.size() > maxBundlesStored) maxBundlesStored = m_store.size();

  m_storedBytes += b->m_adu->GetSize();
}

ssize_t BundleStore::GetMaxBundlesStored() { return maxBundlesStored; }

ssize_t BundleStore::GetStoredByteCount() {
  return m_storedBytes;
}

void BundleStore::Expire(Ptr<Bundle> b) {
  BpHeader *header = b->GetPrimaryHeader();
  NS_LOG_DEBUG(" bundle expired --source: " << header->GetSourceEid().Uri() << " time: " << header->GetCreateTimestamp()
    << " seq: " << header->GetSequenceNumber().GetValue() << " life: " << header->GetLifeTime());
  Remove(b);
  b->DoDispose();
}

void BundleStore::Remove(Ptr<Bundle> b) {
  NS_LOG_DEBUG("cancelling events and removing from store");
  b->ClearEvents();
  storeType::iterator it = m_store.find(b);
  if (it != m_store.end()) {
    m_store.erase(it);
    m_storedBytes -= b->m_adu->GetSize();
  } else {
    NS_LOG_DEBUG("NOT FOUND IN STORE!");
  }
}

Ptr<Packet> BundleStore::GetBundleADU(const BpEndpointId &eid) {
  storeType::iterator it = m_store.begin();
  while (it != m_store.end()) {
    BpHeader *header = (*it)->GetPrimaryHeader();
    if (header->GetDestinationEid() == eid) return (*it)->m_adu;
    it++;
  }
  return Ptr<Packet>(0);
}

Ptr<Packet> BundleStore::GetAndRemoveBundle(const BpEndpointId &eid, bool fragOk) {
  storeType::iterator it = m_store.begin();
  for (; it != m_store.end(); it++) {
    BpHeader *header = (*it)->GetPrimaryHeader();
    if (header->GetDestinationEid() == eid) {
      Ptr<Bundle> b = (*it);
      NS_LOG_DEBUG("matching bundle found " << (header->IsFragment() ? "frag":"unfrag") 
        << " TS " << header->GetCreateTimestamp() << " seqno " << header->GetSequenceNumber().GetValue());
      if (header->IsFragment() && !fragOk) continue;
      Remove(b);
      Ptr<Packet> adu = b->m_adu;
      b->DoDispose();
      NS_LOG_DEBUG("got and removed");
      return adu;
    }
  }
  return Ptr<Packet>(0);
}

Ptr<Bundle> BundleStore::GetBundle(const BpEndpointId &src, uint32_t ts, uint32_t seqno) {
  storeType::iterator it = m_store.begin();
  while (it != m_store.end()) {
    BpHeader *header = (*it)->GetPrimaryHeader();
    if ((header->GetSourceEid() == src) &&
        (header->GetCreateTimestamp() == ts) &&
        (header->GetSequenceNumber().GetValue() == seqno)) return (*it);
    it++;
  }
  return Ptr<Bundle>(0);
}

void BundleStore::GetBundles(const BpEndpointId &src, uint32_t ts, uint32_t seqno, std::list<Ptr<Bundle>> &bundles) {
  storeType::iterator it = m_store.begin();
  while (it != m_store.end()) {
    BpHeader *header = (*it)->GetPrimaryHeader();
    if ((header->GetSourceEid() == src) &&
        (header->GetCreateTimestamp() == ts) &&
        (header->GetSequenceNumber().GetValue() == seqno)) bundles.push_back(*it);
    it++;
  }
}

void BundleStore::GetForwardPendingBundles(std::list<Ptr<Bundle>> *bundles, Ptr<BpCla> cla, Callback<Ptr<BpCla>, BpEndpointId> outgoingClaCallback) {
  storeType::iterator it = m_store.begin();
  while (it != m_store.end()) {
    BpEndpointId destEid = (*it)->GetPrimaryHeader()->GetDestinationEid();
    if (((*it)->retentionConstraints & _BP_FORWARD_PENDING) == _BP_FORWARD_PENDING && outgoingClaCallback(destEid) == cla)
        bundles->push_back(*it);
    it++;
  }
}

void BundleStore::DebugDump() {
  storeType::iterator it = m_store.begin();
  while (it != m_store.end()) {
    BpHeader *header = (*it)->GetPrimaryHeader();
    NS_LOG_DEBUG("--source: " << header->GetSourceEid().Uri() << " time: " << header->GetCreateTimestamp()
      << " seq: " << header->GetSequenceNumber().GetValue() << " life: " << header->GetLifeTime() 
      << " priority: " << header->Priority()
      );
    it++;
  }
}

} // namespace ns3