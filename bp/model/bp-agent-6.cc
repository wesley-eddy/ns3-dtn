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

#include "bp-agent-6.h"
#include "bp-bundle-6.h"
#include "acs-cteb.h"
#include "bp-tcp-cla.h"
#include "bp-udp-cla.h"

NS_LOG_COMPONENT_DEFINE ("BpAgent6");

namespace ns3 {

BpAgent6::BpAgent6 ()
  : BpAgent(),
    m_acsEnabled (false)
{ 
  NS_LOG_FUNCTION (this);
}

BpAgent6::~BpAgent6 ()
{ 
  NS_LOG_FUNCTION (this);
}

TypeId
BpAgent6::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpAgent6")
    .SetParent<BpAgent> ()
    .AddConstructor<BpAgent6> ();
  return tid;
}

int 
BpAgent6::Send (Ptr<Packet> p, const BpEndpointId &src, const BpEndpointId &dst, const Time &lifetime, bool custody, uint32_t priority)
{ 
  NS_LOG_FUNCTION (this << " " << src.Uri () << " " << dst.Uri ());

  // The steps below follow RFC 5050 section 5.2 Bundle Transmission.
  // Step 1 - custody transfer.
  // NOTE: RFC 5050 does not describe any specific reason not to accept custody, so we always do.

  // Step 2 - create outbound bundle.
  if (src != defaultEid) {
    if (!IsNodeMember(src))
      {
        NS_LOG_ERROR("the source eid " << src.Uri() << " is not registered");
        return -1;
      } 
  }
  
  Ptr<Bundle6> bundle = Create<Bundle6>(p);
  BpHeader6 *bph = bundle->GetPrimaryHeader();  
  uint32_t size = p->GetSize();
  
  bph->SetSourceEid(src);
  bph->SetDestinationEid(dst);
  bph->SetCustTxReq(custody);
  bph->SetIsFragment(false);
  bph->SetFragOffset(0);
  bph->SetCreateTimestamp(Simulator::Now().GetSeconds());
  bph->SetSequenceNumber(m_seq++);
  bph->SetLifeTime(lifetime);
  bph->SetPriority(priority);
  bph->SetAduLength(size);

  bundle->retentionConstraints = _BP_DISPATCH_PENDING;
  if (custody) {
    bph->SetCustEid(GetBpEndpointId());
    bundle->retentionConstraints |= _BP_CUSTODY_ACCEPTED;
  }

  m_bundleStore.Store(bundle);

  // Step 3 
  Forward(GetPointer(bundle));

  return 0;
}

void BpAgent6::Forward(Bundle* b) { 
  Bundle6 *bundle = dynamic_cast<Bundle6*>(b);
  BpHeader6 *header = bundle->GetPrimaryHeader();
  BpBlockHeader6 *payload_header = bundle->GetPayloadHeader();
  // Follows section 5.4 of RFC 5050.
  // Step 1 - modify retention constraints.
  bundle->retentionConstraints |= _BP_FORWARD_PENDING;
  bundle->retentionConstraints &= ~(_BP_DISPATCH_PENDING);
  NS_LOG_DEBUG(" fwd - retention " << bundle->retentionConstraints);

  // Step 2 - select endpoints for forwarding.
  BpEndpointId destEid = header->GetDestinationEid();
  Ptr<BpCla> cla = OutgoingCla(destEid);

  // Step 3 - fowarding contraindicated procedure (section 5.4.1).
  if (!cla || !cla->IsReady()) {
    NS_LOG_INFO("no CLA found for next hop");
    // Step 1 - determine whether to declare failure?
    bool failure = false;
    // TODO

    // Step 2 - if failure, follow Section 5.4.2
    if (failure) {
      // TODO
      return;
    }
    // Check if custody transfer is requested, and follow Section 5.10.
    // TODO
    // Note: According to RFC 5050, we should restart at Step 5, but we actually do at Step 1.

    // If the bundle has not been removed from the store, it will still be found there with the FORWARD_PEINDING flag set.
    NS_LOG_DEBUG("forwarding contraindicated");
    return;
  }

  m_unicastForwardTrace((void*)bundle);

  NS_LOG_DEBUG("  fwd - CT is " << ((header->CustTxReq()) ? "requested" : "not requested"));
  // Step 4 - custody transfer release procedure (seciont 5.10.2).
  //pTODO implement this part (that deals with custody) in children classes and put the rest of the function in bp-agent parent
  if (header->CustTxReq()) {
    NS_LOG_DEBUG("  fwd - Custodian is " << header->GetCustEid().Uri());
    // bundle->retentionConstraints &= ~(_BP_CUSTODY_ACCEPTED);
    // TODO cancel custody transfer timer XXX
    // NOTE: this makes no sense, because it's releasing custody based just on forwarding, not on any signal that the agent we're forwarding to either got the bundle or accepted custody.  RFC is backwards?
    // This is where we instead will actually schedule next retransmission.
    bundle->nextRetrans = Simulator::Schedule(ct_rto, &BpAgent::Forward, this, bundle);
  }

  // Step 5 - for each endpoint, trigger CLA.
  // NOTE: This is where we trigger fragmentation.  It is very unclear in RFC 5050 about when
  // and where in the processing logic the right time to trigger fragmentation is.
  // TODO - completely fix this once >1 CLA.
  uint32_t bytesLeft = bundle->m_adu->GetSize();
  bool fragment = (bundle->m_adu->GetSize() > m_bundleSize) ? true : false;  // TODO - should base on CLA "MTU" not just m_bundleSize

  while (bytesLeft > 0) {
    uint32_t size = std::min(bytesLeft, m_bundleSize); // TODO - should base on CLA not m_bundleSize
    uint32_t offset = (fragment) ? bundle->m_adu->GetSize() - bytesLeft : 0;
    bytesLeft -= size;

    header->SetBlockLength(size);
    header->SetIsFragment(fragment);
    header->SetFragOffset(offset);

    payload_header->SetBlockLength(size);
    
    NS_LOG_DEBUG("   sending " << size << " bytes from offset " << offset);
    m_sendOutgoingTrace((void*)bundle);
    cla->SendBundle(bundle, GetEidAddress(destEid), GetNode());
  }

  // Step 6 - wrap up.
  // TODO wait for CLA notifications?  we need to add a new event handler for this, not block
  bundle->retentionConstraints &= ~(_BP_FORWARD_PENDING);

  NS_LOG_DEBUG("  retention constraints " << bundle->retentionConstraints);
  if (!(bundle->retentionConstraints)) {
    m_bundleStore.Remove(bundle);
    bundle->DoDispose();
    NS_LOG_DEBUG("removed without retention constraints");
  }
}

int BpAgent6::EnqueueForDeliveryToApplication(Bundle* b) {
  Bundle6 *bundle = dynamic_cast<Bundle6*>(b);

  m_localDeliverTrace((void*)bundle);
  BpHeader6 *header = bundle->GetPrimaryHeader();
  // First check if this if for the local agent, rather than an application.
  if (header->GetDestinationEid() == GetBpEndpointId()) {
    NS_LOG_DEBUG("local agent delivery");
    // Assume the bundle contents is an admin record and try to process it.
    AdminRecord ar;
    if (bundle->m_adu->RemoveHeader(ar) != 1) {
      NS_LOG_DEBUG("FAILED TO REMOVE ADMIN RECORD HEADER");
      return 1;
    }

    NS_LOG_DEBUG("->AR: " << (uint32_t)ar.typeFlags);

    if (ar.typeFlags & (_BP_AR_CS<<4)) {
      CustodySignal cs;
      bundle->m_adu->RemoveHeader(cs);
      NS_LOG_DEBUG(" ->CS: " << " " << cs.fragmentOffset << " " << cs.fragmentLength << " " << cs.timeOfSignal << " " << cs.creationTimestamp << " " << cs.seqNo << " " << cs.srcEidLen << " " << cs.srcEid);
      if (cs.status == 0x80) {
        NS_LOG_DEBUG("    success");
        // pTODO maybe use template in bundle store instead of requring dynamic cast here
        Ptr<Bundle6> s = DynamicCast<Bundle6, Bundle>(m_bundleStore.GetBundle(cs.srcEid, cs.creationTimestamp, cs.seqNo.GetValue()));
        if (s != NULL) {
          BpHeader6 *sHeader = s->GetPrimaryHeader();
          // Attempt to combine w/ existing ACKs before adding as new.
          std::list<std::pair<uint32_t, uint32_t>>::iterator ackit;
          std::pair<uint32_t,uint32_t> thisack(cs.fragmentOffset, cs.fragmentOffset + cs.fragmentLength);
          for (ackit = s->acks.begin(); ackit != s->acks.end(); ackit++) {
            NS_LOG_DEBUG(" comparing " << thisack.first << " - " << thisack.second << " to " << ackit->first << " through " << ackit->second);
            if (ackit->second == thisack.first) ackit->second = thisack.second;
            if (thisack.second == ackit->first) ackit->first = thisack.first;
            // TODO - the above logic does not catch all possibilities of overlap, fill-in, etc.
          }
          if (s->acks.size() == 0) s->acks.push_back(thisack);
          if (s->acks.size() == 1 && s->acks.begin()->first == 0 && s->acks.begin()->second == sHeader->GetAduLength()) {
            NS_LOG_DEBUG("fully acked - removing");
            m_bundleStore.Remove(s);
            s->DoDispose();
          } else NS_LOG_DEBUG("acklist size " << s->acks.size() << " first " << s->acks.begin()->first << " - " << s->acks.begin()->second);
        } else {
          NS_LOG_DEBUG("FAILED TO FIND BUNDLE FOR CUSTODY SIGNAL");
          m_bundleStore.DebugDump();
        }
      } else {
        NS_LOG_DEBUG("    failure");
      }
    } else if (ar.typeFlags & (_BP_AR_ACS<<4)) {
      ACS acs;
      bundle->m_adu->RemoveHeader(acs);
      NS_LOG_DEBUG(" ->ACS");
      if (acs.status == 0x80) {
        NS_LOG_DEBUG("    success");
      } else {
        NS_LOG_DEBUG("    failure");
      } 
    } else {
      NS_LOG_DEBUG("  unhandled type+flags combination in admin record");
    }
    m_bundleStore.Remove(bundle);
    bundle->DoDispose();
    NS_LOG_DEBUG("removed custody signal");
  }

  // Otherwise the bundle is already in the store and ready for an application
  // to call Receive() and get it.

  // If the bundle is a fragment, we should check to see if it coincides with
  // be beginning or end of any other fragments stored, and coalesce them at
  // this point.
  if (header->IsFragment()) {
    NS_LOG_DEBUG("matching new fragment " << header->GetFragOffset() << " sz " << bundle->m_adu->GetSize());
    std::list<Ptr<Bundle>> prevFrags;
    m_bundleStore.GetBundles(header->GetSourceEid(), header->GetCreateTimestamp(), header->GetSequenceNumber().GetValue(), prevFrags);
    std::list<Ptr<Bundle>>::iterator it = prevFrags.begin();
    while (it != prevFrags.end()) {
      Ptr<Bundle6> it6 = DynamicCast<Bundle6, Bundle>(*it);
      BpHeader6 *itHeader = it6->GetPrimaryHeader();
      if (header->GetFragOffset() == itHeader->GetFragOffset() + it6->m_adu->GetSize()) {
        NS_LOG_DEBUG("combining new fragment at tail of previous");
        it6->m_adu->AddAtEnd(bundle->m_adu);
        m_bundleStore.Remove(bundle);
        if (it6->m_adu->GetSize() == itHeader->GetAduLength()) {
          itHeader->SetIsFragment(false);
        }
      } else if (header->GetFragOffset() + bundle->m_adu->GetSize() == itHeader->GetFragOffset()) {
        NS_LOG_DEBUG("combining new fragment at head of previous");
        bundle->m_adu->AddAtEnd(it6->m_adu);
        m_bundleStore.Remove(it6);
        if (bundle->m_adu->GetSize() == header->GetAduLength()) {
          header->SetIsFragment(false);
        }
      } else {
        NS_LOG_DEBUG("new fragment does not match head or tail of previous " << itHeader->GetFragOffset() << " sz " << it6->m_adu->GetSize());
      }
      it++;
    }
  }
  return 0;
}

void BpAgent6::Deliver(Bundle* b) {
  NS_LOG_FUNCTION(this);
  Bundle6 *bundle = dynamic_cast<Bundle6*>(b);
  BpHeader6 *header = bundle->GetPrimaryHeader();

  // Follows section 5.7 of RFC 5050.
  // Step 1 - fragment processing.
  // TODO

  // Step 2 - check registration state.
  std::map<BpEndpointId, BpRegisterInfo>::iterator it = BpRegistration.end();
  it = BpRegistration.find(header->GetDestinationEid());
  if (it == BpRegistration.end()) {
    NS_LOG_ERROR("registration not found for " << header->GetDestinationEid().Uri());
    return;
  }
  if (((*it).second.state == false) || (EnqueueForDeliveryToApplication(bundle) != 0)) {
    // Delivery failure action.
    // TODO
  } 

  // Step 3 - reporting and custody transfer
  // TODO - if request reporting of delivery is set, ... generate admin record for that
  NS_LOG_DEBUG("  del - CT is " << ((header->CustTxReq()) ? "requested" : "not requested"));
  if (header->CustTxReq()) {
    NS_LOG_DEBUG("  del - Custodian is " << header->GetCustEid().Uri()); 
    // Generate a succeeded custody signal.
    SendCustodySignal(bundle, true, header->GetLifeTime());
  }
}

void BpAgent6::SendCustodySignal(Ptr<Bundle6> b, bool success, const Time& lifetime) {
  NS_LOG_FUNCTION(this);
  BpHeader6 *header = b->GetPrimaryHeader();
  Ptr<Packet> s = Create<Packet>();

  CustodySignal cs;
  if (success) cs.status = 0x80;
  cs.fragmentOffset = header->GetFragOffset();
  cs.fragmentLength = header->IsFragment() ? b->m_adu->GetSize() : 0;
  cs.timeOfSignal = Simulator::Now();
  cs.creationTimestamp = header->GetCreateTimestamp();
  cs.seqNo = header->GetSequenceNumber();
  cs.srcEidLen = header->GetSourceEid().Uri().length();
  cs.srcEid = header->GetSourceEid().Uri();
  s->AddHeader(cs);

  AdminRecord ar;
  ar.typeFlags = _BP_AR_CS<<4;
  if (header->IsFragment()) ar.typeFlags |= _BP_AR_FRAG;
  s->AddHeader(ar);

  NS_LOG_DEBUG("  sending custody signal to " << header->GetCustEid().Uri() << " for seq " << cs.seqNo.GetValue() 
    << " AR type flags: " << (uint32_t)ar.typeFlags);
  if (Send(s, GetBpEndpointId(), header->GetCustEid(), lifetime) != 0)
    NS_LOG_WARN("failed to send custody signal");
}

void BpAgent6::EnableACS() {
  m_acsEnabled = true;
}

void BpAgent6::DisableACS() {
  m_acsEnabled = false;
}

} // namespace ns3
