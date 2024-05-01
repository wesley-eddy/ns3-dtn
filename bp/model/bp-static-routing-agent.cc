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
 *          Dizhi Zhou <dizhi.zhou@gmail.com>
 */

#include "bp-static-routing-agent.h"
#include "bp-cla.h"
#include "bp-udp-cla.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("BpStaticRoutingAgent");

namespace ns3 {

TypeId 
BpStaticRoutingAgent::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpStaticRoutingAgent")
    .SetParent<BpRoutingAgent> ()
    .AddConstructor<BpStaticRoutingAgent> ()
  ;
  return tid;
}

BpStaticRoutingAgent::BpStaticRoutingAgent ()
{ 
  NS_LOG_FUNCTION (this);
}

BpStaticRoutingAgent::~BpStaticRoutingAgent ()
{ 
  NS_LOG_FUNCTION (this);
};

int BpStaticRoutingAgent::AddRoute (BpEndpointId &dst, BpEndpointId &nxt, bool up, Ipv4Address addr, uint16_t port, Ptr<BpCla> cla, std::string *note) {
  struct dtnRoute r;
  r.dstEid = dst;
  r.nxtHop = nxt;
  r.up = up;
  r.addr = addr;
  r.port = port;
  r.cla = cla;
  if (note != NULL) r.note = *note; else r.note = "(none)";

  if (m_routes.find(dst) == m_routes.end()) {
    std::list<struct dtnRoute> *routes = new std::list<struct dtnRoute>();
    m_routes[dst] = routes;
  } 
  m_routes[dst]->push_back(r);

  return 0;
}

InetSocketAddress 
BpStaticRoutingAgent::GetRoute (BpEndpointId eid)
{ 
  NS_LOG_FUNCTION (this << " " << eid.Uri ());
  std::map<BpEndpointId, std::list<dtnRoute>*>::iterator h = m_routes.find(eid);
  if (h == m_routes.end()) return InetSocketAddress("127.0.0.1", 0);
  for (std::list<struct dtnRoute>::iterator i = (*h).second->begin(); i != (*h).second->end(); i++) {
    if ((*i).up) return InetSocketAddress((*i).addr, (*i).port);
  }
  return InetSocketAddress("127.0.0.1", 0);
}

BpEndpointId BpStaticRoutingAgent::NextHopEid(BpEndpointId &dst) {
  NS_LOG_FUNCTION(this << " " << dst.Uri());
  std::map<BpEndpointId, std::list<dtnRoute>*>::iterator h = m_routes.find(dst);
  if (h == m_routes.end()) return BpEndpointId("dtn:none");
  for (std::list<struct dtnRoute>::iterator i = (*h).second->begin(); i != (*h).second->end(); i++) {
    NS_LOG_DEBUG("route " << (((*i).up==true)?"UP":"DOWN") << " via " << (*i).nxtHop.Uri() << " " << (*i).note);
    if ((*i).up) return (*i).nxtHop;
  }
  return BpEndpointId("dtn:none");
}

Ptr<BpCla> BpStaticRoutingAgent::NextHopCla(BpEndpointId &eid) {
  NS_LOG_FUNCTION(this << " " << eid.Uri());
  std::map<BpEndpointId, std::list<dtnRoute>*>::iterator h = m_routes.find(eid);
  if (h == m_routes.end()) return NULL;
  for (std::list<struct dtnRoute>::iterator i = (*h).second->begin(); i != (*h).second->end(); i++) {
    if ((*i).up) return (*i).cla;
  }
  return NULL;
}

void BpStaticRoutingAgent::UpRoute (BpEndpointId &dst, BpEndpointId &nxt, std::string *note) {
  NS_LOG_FUNCTION(this);
  NS_LOG_DEBUG("route UP to " << dst.Uri() << " via " << nxt.Uri());
  std::map<BpEndpointId, std::list<dtnRoute>*>::iterator i = m_routes.find(dst);
  for (std::list<dtnRoute>::iterator j = (*i).second->begin(); j != (*i).second->end(); j++) {
    if ((*j).nxtHop == nxt && ((note == NULL) || (*j).note == *note)) {
      (*j).up = true;
      NS_LOG_DEBUG("--- success");
      return;
    }
  }
  NS_LOG_DEBUG("--- failure");
}
void BpStaticRoutingAgent::DownRoute (BpEndpointId &dst, BpEndpointId &nxt, std::string *note) {
  NS_LOG_FUNCTION(this);
  NS_LOG_DEBUG("route DOWN to " << dst.Uri() << " via " << nxt.Uri());
  std::map<BpEndpointId, std::list<dtnRoute>*>::iterator i = m_routes.find(dst);
  for (std::list<dtnRoute>::iterator j = (*i).second->begin(); j != (*i).second->end(); j++) {
    if ((*j).nxtHop == nxt && ((note == NULL) || (*j).note == *note)) {
      (*j).up = false;
      NS_LOG_DEBUG("--- success");
      return;
    }
  }
  NS_LOG_DEBUG("--- failure");
}

} // namespace ns3
