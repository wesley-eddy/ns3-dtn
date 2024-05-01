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
 *          Shawn McGraw <shawn.mcgraw@nasa.gov>
 *          Dizhi Zhou <dizhi.zhou@gmail.com>
 */

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/socket.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"
#include "ns3/buffer.h"
#include "bp-tcp-cla.h"
#include "bp-udp-cla.h"
#include "bp-agent.h"
#include <algorithm>
#include <map>

NS_LOG_COMPONENT_DEFINE ("BpAgent");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (BpAgent);

TypeId
BpAgent::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpAgent")
    .SetParent<Object> ()
    .AddAttribute ("BundleSize", "Max size of a bundle in bytes",
           UintegerValue (512),
           MakeUintegerAccessor (&BpAgent::m_bundleSize),
           MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("StartTime", "Time at which the bundle protocol agent will start",
                   TimeValue (Seconds (0.0)),
                   MakeTimeAccessor (&BpAgent::m_startTime),
                   MakeTimeChecker ())
    .AddAttribute ("StopTime", "Time at which the bundle protocol agent will stop",
                   TimeValue (TimeStep (0)),
                   MakeTimeAccessor (&BpAgent::m_stopTime),
                   MakeTimeChecker ())

    .AddTraceSource ("SendOutgoing",
                     "A newly-generated bundle by this node is being queued"
                     "for transmission",
                     MakeTraceSourceAccessor (&BpAgent::m_sendOutgoingTrace),
                     "ns3::BundleProtocol::SentTracedCallback")
    .AddTraceSource ("UnicastForward",
                     "A unicast bundle is being forwarded through this node",
                     MakeTraceSourceAccessor (&BpAgent::m_unicastForwardTrace),
                     "ns3::BundleProtocol::UnicastForwardTracedCallback")
    .AddTraceSource ("LocalDeliver",
                     "A bundle is being delivered to the destination on this node",
                     MakeTraceSourceAccessor (&BpAgent::m_localDeliverTrace),
                     "ns3::BundleProtocol::LocalDeliverTracedCallback")
    .AddTraceSource ("Drop",
                     "A bundle is being dropped",
                     MakeTraceSourceAccessor (&BpAgent::m_dropTrace),
                     "ns3::BundleProtocol::DropTracedCallback")
  ;
  return tid;
}

BpAgent::BpAgent ()
  : m_node (0),
    m_seq (0),
    m_eid ("dtn:none"),
    m_bpRegInfo (),
    m_bpRoutingAgent (0),
    bundlesDelivered (0),
    bytesDelivered (0)
{ 
  NS_LOG_FUNCTION (this);
}

BpAgent::~BpAgent ()
{ 
  NS_LOG_FUNCTION (this);
}

void 
BpAgent::Open (Ptr<Node> node)
{ 
  NS_LOG_FUNCTION (this << " " << node);
  m_node = node;
}

Ptr<BpCla> BpAgent::AddCla(std::string l4type) {
  Callback<void, Ptr<Bundle>> processBundleCallback = MakeCallback(&BpAgent::ProcessBundle, this);
  if (l4type == "Tcp") {
    Ptr<BpTcpCla> cla = CreateObject<BpTcpCla>(processBundleCallback);
    m_clas.push_back(cla);
    return cla;
  }
  if (l4type == "Udp") {
    Ptr<BpUdpCla> cla = CreateObject<BpUdpCla>(processBundleCallback);
    m_clas.push_back(cla);
    return cla;
  }
  return Ptr<BpCla>(0);
}

BpEndpointId
BpAgent::BuildBpEndpointId (const std::string &scheme, const std::string &ssp)
{ 
  NS_LOG_FUNCTION (this << " " << scheme << " " << ssp);
  BpEndpointId eid (scheme, ssp); 
 
  m_eid = eid;

  return eid;
}

BpEndpointId
BpAgent::BuildBpEndpointId (const std::string &uri)
{ 
  NS_LOG_FUNCTION (this << " " << uri);
  BpEndpointId eid (uri);

  m_eid = eid;

  return eid;
}

void 
BpAgent::SetBpEndpointId (BpEndpointId eid)
{ 
  NS_LOG_FUNCTION (this << " " << eid.Uri ());
  m_eid = eid;
}

int 
BpAgent::Register (const BpEndpointId &eid, const struct BpRegisterInfo &info)
{ 
  NS_LOG_FUNCTION (this << " " << eid.Uri ());
  std::map<BpEndpointId, BpRegisterInfo>::iterator it = BpRegistration.end ();
  it = BpRegistration.find (eid);
  if (it == BpRegistration.end ())
    {
      // insert a registration of local endpoint id in the registration storage
      BpRegisterInfo rInfo;
      rInfo.lifetime = info.lifetime;
      rInfo.state = info.state;
      BpRegistration.insert (std::pair<BpEndpointId, BpRegisterInfo> (eid, rInfo));

      if (info.state)
        {
          // enable all of the CLAs to receive packets
          std::deque<Ptr<BpCla>>::iterator i = m_clas.begin();
          while (i != m_clas.end()) { 
            // TODO is it okay to ignore return value, indicating some CLA can't currenlty listen for the EID?
            (*i)->EnableReceive (eid, GetEidAddress(eid), m_node);
            i++;
          }
        }
    } 
  else
    {
      // duplicate registration
      NS_FATAL_ERROR ("BpAgent::Register (): duplicate registration with endpoint id ");
      return -1;
    }
  return 0;
}

int 
BpAgent::Unregister (const BpEndpointId &eid)
{ 
  NS_LOG_FUNCTION (this << " " << eid.Uri ());
  std::map<BpEndpointId, BpRegisterInfo>::iterator it = BpRegistration.end ();
  it = BpRegistration.find (eid);
  if (it == BpRegistration.end ())
    {
      return -1;
    } 

  (*it).second.state = false;
  for (std::deque<Ptr<BpCla>>::iterator i = m_clas.begin(); i != m_clas.end(); i++) {
    // TODO probably ok to ignore return value?
    (*i)->DisableReceive (eid);
  }
  return 0;
}

// TODO - look into how this differs in any way from Register() ?
int
BpAgent::Bind (const BpEndpointId &eid)
{ 
  NS_LOG_FUNCTION (this << " " << eid.Uri ());
  std::map<BpEndpointId, BpRegisterInfo>::iterator it = BpRegistration.end ();
  it = BpRegistration.find (eid);
  if (it == BpRegistration.end ())
    {
      return -1;
    } 
  else
    {
      // set the registeration of this eid to active
      (*it).second.state = true;
      for (std::deque<Ptr<BpCla>>::iterator i = m_clas.begin(); i != m_clas.end(); i++) {
        // TODO probably ok to ignore return value?
        (*i)->EnableReceive(eid, GetEidAddress(eid), m_node);
      }
    }
  return 0;
}

bool BpAgent::IsNodeMember(const BpEndpointId &eid) {
  std::map<BpEndpointId, BpRegisterInfo>::iterator it = BpRegistration.end();
  it = BpRegistration.find(eid);
  return it != BpRegistration.end();
}

int
BpAgent::Close (const BpEndpointId &eid)
{
  NS_LOG_FUNCTION (this << " " << eid.Uri ());
  std::map<BpEndpointId, BpRegisterInfo>::iterator it = BpRegistration.end ();
  it = BpRegistration.find (eid);
  if (it == BpRegistration.end ())
    {
      return -1;
    } 
  else
    {
      // TBD: call cla to close transport layer connection

      BpRegistration.erase (it);
    }

  return 0;
}

void 
BpAgent::ProcessBundle (Ptr<Bundle> b)
{ 
  BpHeader *header = b->GetPrimaryHeader();
  NS_LOG_FUNCTION (this << " " << b << " seqno " << header->GetSequenceNumber() << " ADU size: " << b->m_adu->GetSize());

  // Follow Section 5.6 of RFC 5050.
 
  // Step 1 - set dispatch pending flag.
  b->retentionConstraints |= _BP_DISPATCH_PENDING;

  // Step 2 - send report, if requested.
  // TODO

  // Step 3 - Process extension blocks.
  // TODO

  // Step 4 - custody transfer
  //pTODO: handle this case for diff between v6 and v7 (maybe make a pure virtual method required by children called "processCustodyRequest()")
  /*if (b->custodyRequested) {
    // if same src EID, creat timestamp, frag off, payload len as one in store w/ custody acc. flag ... custody transfer redundancy must be handled ????? XXX if destination is a singleton, we're supposed to remove the dispatch pending constraint, generate failed custody signal w/ "redundant reception"
    // TODO
  }*/

//if (GetBpEndpointId().Uri() == "dtn:gateway") std::cout << m_bundleStore.GetMaxBundlesStored() << std::endl;
  m_bundleStore.Store(b);
//if (GetBpEndpointId().Uri() == "dtn:gateway") std::cout << m_bundleStore.GetMaxBundlesStored() << std::endl;

  // Step 5 - Dispatch
  Dispatch(b);
}

Ptr<Packet>
BpAgent::Receive (const BpEndpointId &eid)
{ 
  NS_LOG_FUNCTION (this << " " << eid.Uri ());
  Ptr<Packet> p = m_bundleStore.GetAndRemoveBundle(eid, false);
  if (p != NULL) {
    bundlesDelivered++;
    bytesDelivered += p->GetSize();
  }
  return p;
}

BpEndpointId 
BpAgent::GetBpEndpointId () const
{ 
  NS_LOG_FUNCTION (this);
  return m_eid;
}

void
BpAgent::SetRoutingAgent (Ptr<BpRoutingAgent> route)
{ 
  NS_LOG_FUNCTION (this << " " << route);
  m_bpRoutingAgent = route;
}

Ptr<BpRoutingAgent> 
BpAgent::GetRoutingAgent ()
{ 
  NS_LOG_FUNCTION (this);
  return m_bpRoutingAgent;
}

Ptr<Node> 
BpAgent::GetNode () const
{ 
  NS_LOG_FUNCTION (this);
  return m_node;
}

Ptr<Packet> 
BpAgent::GetBundle (const BpEndpointId &src)
{ 
  NS_LOG_FUNCTION (this << " " << src.Uri ());
  return m_bundleStore.GetBundleADU(src);
}

void 
BpAgent::SetBpRegisterInfo (struct BpRegisterInfo info)
{ 
  NS_LOG_FUNCTION (this);
  m_bpRegInfo = info;
}

void 
BpAgent::DoInitialize (void)
{ 
  NS_LOG_FUNCTION (this);
  m_startEvent = Simulator::Schedule (m_startTime, &BpAgent::StartBpAgent, this);
  if (m_stopTime != TimeStep (0))
    {
      m_stopEvent = Simulator::Schedule (m_stopTime, &BpAgent::StopBpAgent, this);
    }
  Object::DoInitialize ();
}

void
BpAgent::StartBpAgent ()
{ 
  NS_LOG_FUNCTION (this);
// TODO(weddy): Likely delete this old code below.
//  if (m_clas.empty())
//    NS_FATAL_ERROR (this << " BpAgent::DoInitialize (): did not define convergence layer! ");   
//
//  if (!GetRoutingAgent ())
//    NS_FATAL_ERROR (this << " BpAgent::DoInitialize (): did not define bundle routing age t! ");   
//
  Register (m_eid, m_bpRegInfo);
}

void
BpAgent::StopBpAgent ()
{ 
  NS_LOG_FUNCTION (this);
  Close (m_eid);
}

void BpAgent::Dispatch(Ptr<Bundle> b) {
  // Follows section 5.3 of RFC 5050.
  BpEndpointId dstEid = b->GetPrimaryHeader()->GetDestinationEid();
  if (IsNodeMember(dstEid)) {
    Deliver(GetPointer(b));
  }
  else Forward(GetPointer(b));
}

Ptr<BpCla> BpAgent::OutgoingCla(BpEndpointId dstEid) {
  BpEndpointId nextHop = m_bpRoutingAgent->NextHopEid(dstEid);
  NS_LOG_DEBUG("*** nextHop " << nextHop.Uri());
  if (nextHop == defaultEid) return Ptr<BpCla>(0);
  return m_bpRoutingAgent->NextHopCla(nextHop);
}

void BpAgent::ClaReady(Ptr<BpCla> cla) {
  NS_LOG_FUNCTION("cla ready");
  // Loop across the store, retrying forwarding for anything that has FORWARD_PENDING set, and routing to the indicated CLA.
  std::list<Ptr<Bundle>> bundles;
  m_bundleStore.GetForwardPendingBundles(&bundles, cla, MakeCallback(&BpAgent::OutgoingCla, this));
  for (std::list<Ptr<Bundle>>::iterator it = bundles.begin(); it != bundles.end(); it++) {
    NS_LOG_DEBUG("-- cla ready - forwarding bundle");
    Forward(GetPointer(*it));
  }
  bundles.clear();
}

void
BpAgent::SetStartTime (Time start)
{ 
  NS_LOG_FUNCTION (this << " " << start.GetSeconds ());
  m_startTime = start;
}

void
BpAgent::SetStopTime (Time stop)
{ 
  NS_LOG_FUNCTION (this << stop.GetSeconds ());
  m_stopTime = stop;
}

void
BpAgent::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_node = NULL;
  m_clas.clear();
  BpRegistration.clear();
  m_bpRoutingAgent = NULL;
  m_startEvent.Cancel ();
  m_stopEvent.Cancel ();
  Object::DoDispose ();
}

void BpAgent::AddCla(Ptr<BpCla> cla) {
  m_clas.push_back(cla);
}

void BpAgent::RemoveCla(Ptr<BpCla> cla) {
  std::deque<Ptr<BpCla>>::iterator i = m_clas.begin();
  while (i != m_clas.end()) {
    if ((*i) == cla) break;
    i++;
  }
  if (i == m_clas.end()) return; // TODO failure!
  m_clas.erase(i);
}

Ptr<BpCla> BpAgent::GetCla(size_t n) {
  return m_clas.at(n);
}

InetSocketAddress BpAgent::GetEidAddress(const BpEndpointId &eid) {
  NS_LOG_FUNCTION("get eid address");
  // TBD: do not use dynamicast here
  // check route for endpoint id
  Ptr<BpStaticRoutingAgent> route = DynamicCast <BpStaticRoutingAgent> (m_bpRoutingAgent);
  return route->GetRoute (eid);
}
} // namespace ns3