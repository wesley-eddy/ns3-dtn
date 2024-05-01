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
 *          Dizhi Zhou <dizhi.zhou@gmail.com>
 */

#include "ns3/log.h"
#include "ns3/node.h"
#include "bp-header-6.h"
#include <stdio.h>
#include <string>

#include "sdnv.h"
#include <vector>

NS_LOG_COMPONENT_DEFINE ("BpHeader6");

namespace ns3 {

BpHeader6::BpHeader6 (bool useCbhe)
  : BpHeader(6, useCbhe),
    m_blockLength (0),
    m_dictionary ("")
{ 
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG("BpHeader6 *** " << useCbhe);
}

BpHeader6::~BpHeader6 ()
{ 
  NS_LOG_FUNCTION (this);

  m_dictionary.clear();
}

TypeId 
BpHeader6::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpHeader6")
    .SetParent<BpHeader> ()
    .AddConstructor<BpHeader6> ()
  ;

  return tid;
}

TypeId 
BpHeader6::GetInstanceTypeId (void) const
{ 
  NS_LOG_FUNCTION (this);
  return GetTypeId ();
}

void 
BpHeader6::Print (std::ostream &os) const
{ 
  NS_LOG_FUNCTION (this);
}

uint32_t
BpHeader6::SerializeAndGetSize (Buffer::Iterator start, bool forReal) const
{
  NS_LOG_FUNCTION (this);
  Buffer::Iterator i = start;
  Sdnv s;
  std::vector<uint8_t> temp;
  uint32_t length = 0;
  size_t j;

  length = 1;
  if (forReal) i.WriteU8(m_version);

  temp = s.Encode(m_processingFlags);
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_blockLength);
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_dstSchemeOffset);
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_dstSspOffset);
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_srcSchemeOffset);
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_srcSspOffset);
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_reportSchemeOffset);
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_reportSspOffset);
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_custSchemeOffset);
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_custSspOffset);
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_createTimestamp);
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_timestampSeqNum.GetValue());
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_lifeTime.GetSeconds());
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_dictionary.length());
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));


  if (forReal)
    for (uint32_t k = 0; k < m_dictionary.length(); k++)
      i.WriteU8 (m_dictionary[k]);
  length += m_dictionary.length();

  temp = s.Encode(m_fragOffset);
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));

  temp = s.Encode(m_aduLength);
  length += temp.size();
  if (forReal) for (j = 0; j < temp.size(); j++) i.WriteU8(temp.at(j));
   
  return length;
}

uint32_t 
BpHeader6::GetSerializedSize (void) const
{
  Buffer b(300); 
  return SerializeAndGetSize(b.Begin(), false);
}

void 
BpHeader6::Serialize (Buffer::Iterator start) const
{
  SerializeAndGetSize(start, true);
}

uint32_t 
BpHeader6::Deserialize (Buffer::Iterator start)
{ 
  NS_LOG_FUNCTION (this);
  Buffer::Iterator i = start;
  Sdnv sdnv;

  m_version = i.ReadU8 ();
  m_processingFlags = (uint32_t) sdnv.Decode (i);
  NS_LOG_DEBUG("received processing flags: " << m_processingFlags);
  m_blockLength = (uint32_t) sdnv.Decode (i);

  m_dstSchemeOffset = (uint16_t) sdnv.Decode (i);
  m_dstSspOffset = (uint16_t) sdnv.Decode (i); 
  m_srcSchemeOffset = (uint16_t) sdnv.Decode (i);
  m_srcSspOffset = (uint16_t) sdnv.Decode (i);
  m_reportSchemeOffset = (uint16_t) sdnv.Decode (i); 
  m_reportSspOffset = (uint16_t) sdnv.Decode (i);
  m_custSchemeOffset = (uint16_t) sdnv.Decode (i);
  m_custSspOffset = (uint16_t) sdnv.Decode (i);

  m_createTimestamp = sdnv.Decode (i);
  m_timestampSeqNum = (uint32_t)sdnv.Decode (i);
  m_lifeTime = Seconds((uint64_t) sdnv.Decode (i));
  uint32_t m_dictLength = (uint32_t) sdnv.Decode (i);

  m_dictionary = std::string(" ", m_dictLength);
  for (uint32_t k = 0; k < m_dictLength; k++)
    m_dictionary[k] = i.ReadU8 ();

  m_fragOffset = (uint32_t) sdnv.Decode (i);
  m_aduLength = (uint32_t) sdnv.Decode (i);

  return i.GetDistanceFrom(start);
}

void
BpHeader6::SetCustTxReq (const bool value)
{ 
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingFlags |= REQ_BUNDLE_CUSTODY_XFER;
  else
    m_processingFlags &= (~(REQ_BUNDLE_CUSTODY_XFER));

}

void
BpHeader6::SetSingletonDest (const bool value)
{ 
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingFlags |= BUNDLE_SINGLETON_DESTINATION;
  else
    m_processingFlags &= (~(BUNDLE_SINGLETON_DESTINATION));

}

void
BpHeader6::SetPriority (const uint8_t pri)
{ 
  NS_LOG_FUNCTION (this << " " << (uint16_t)pri);
  NS_LOG_DEBUG("pre-priority setting: " << m_processingFlags);
  m_processingFlags |= (m_processingFlags & (~(uint32_t)BUNDLE_PRIORITY)) | (((uint32_t)pri)<<7);
  NS_LOG_DEBUG("post-priority setting: " << m_processingFlags);
}

void 
BpHeader6::SetCustAcceptReport (const bool value)
{ 
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingFlags |= REQ_REPORT_CUSTODY_ACCEPT;
  else
    m_processingFlags &= (~(REQ_REPORT_CUSTODY_ACCEPT));

}

bool 
BpHeader6::CustTxReq () const
{ 
  NS_LOG_FUNCTION (this);
  return m_processingFlags & REQ_BUNDLE_CUSTODY_XFER;
}

bool 
BpHeader6::SingletonDest () const
{ 
  NS_LOG_FUNCTION (this);
  return m_processingFlags & BUNDLE_SINGLETON_DESTINATION;
}

uint8_t 
BpHeader6::Priority () const   
{
  uint32_t retval = (m_processingFlags & (uint32_t)BUNDLE_PRIORITY)>>7;
  NS_LOG_FUNCTION (this << "procflags: " << m_processingFlags << " priority: " << retval);
  return (uint8_t)retval;
}

bool 
BpHeader6::CustAcceptReport () const
{ 
  NS_LOG_FUNCTION (this);
  return m_processingFlags & REQ_REPORT_CUSTODY_ACCEPT;
}

// TODO - the functions below don't allow compression of the dictionary if
// some strings are repeated, so result in larger-than-necessary BP headers.
// instead of the code in each function below that sets the offset to the current dictionary size and appends, there should be a dictionary search and set the offset to any current offset with the same EID

void 
BpHeader6::SetDestinationEid (const BpEndpointId &dst)
{ 
  NS_LOG_FUNCTION (this << " " << dst.Uri ());
  std::string scheme = dst.Scheme ();
  std::string ssp = dst.Ssp ();

  if (m_cbhe) {
    m_dstSchemeOffset = IpnNode(ssp);
    m_dstSspOffset = IpnService(ssp);
  } else {
    m_dstSchemeOffset = m_dictionary.size ();
    m_dictionary.append (scheme);
    m_dictionary.push_back('\0');

    m_dstSspOffset = m_dictionary.size ();
    m_dictionary.append (ssp);
    m_dictionary.push_back('\0');
  
NS_LOG_DEBUG("scheme off " << m_dstSchemeOffset << " ssp off " << m_dstSspOffset << " dictlen " << m_dictionary.length());
  }
}

void 
BpHeader6::SetSourceEid (const BpEndpointId &src)
{ 
  NS_LOG_FUNCTION (this << " " << src.Uri ());
  std::string scheme = src.Scheme ();
  std::string ssp = src.Ssp ();

  if (m_cbhe) {
    m_srcSchemeOffset = IpnNode(ssp);
    m_srcSspOffset = IpnService(ssp);
  } else {
    m_srcSchemeOffset = m_dictionary.size ();
    m_dictionary.append (scheme); 
    m_dictionary.push_back('\0');

    m_srcSspOffset = m_dictionary.size ();
    m_dictionary.append (ssp);
    m_dictionary.push_back('\0');

NS_LOG_DEBUG("scheme off " << m_srcSchemeOffset << " ssp off " << m_srcSspOffset << " dictlen " << m_dictionary.length());
  }
}

void 
BpHeader6::SetReportEid (const BpEndpointId &report)
{ 
  NS_LOG_FUNCTION (this << " " << report.Uri ());
  std::string scheme = report.Scheme ();
  std::string ssp = report.Ssp ();

  if (m_cbhe) {
    m_reportSchemeOffset = IpnNode(ssp);
    m_reportSspOffset = IpnService(ssp);
  } else {
    m_reportSchemeOffset = m_dictionary.size ();
    m_dictionary.append (scheme); 
    m_dictionary.push_back('\0');

    m_reportSspOffset = m_dictionary.size ();
    m_dictionary.append (ssp);
    m_dictionary.push_back('\0');
  }
}

void 
BpHeader6::SetCustEid (const BpEndpointId &cust)
{ 
  NS_LOG_FUNCTION (this << " " << cust.Uri ());
  std::string scheme = cust.Scheme ();
  std::string ssp = cust.Ssp ();

  if (m_cbhe) {
    m_custSchemeOffset = IpnNode(ssp);
    m_custSspOffset = IpnService(ssp);
  } else {
    m_custSchemeOffset = m_dictionary.size ();
    m_dictionary.append (scheme); 
    m_dictionary.push_back('\0');

    m_custSspOffset = m_dictionary.size ();
    m_dictionary.append (ssp);
    m_dictionary.push_back('\0');
  }
NS_LOG_DEBUG("cust scheme off " << m_srcSchemeOffset << " ssp off " << m_srcSspOffset << " dictlen " << m_dictionary.length());
}

BpEndpointId 
BpHeader6::GetDestinationEid () const
{ 
  NS_LOG_FUNCTION (this);
NS_LOG_DEBUG("cbhe:" << m_cbhe);
  std::string scheme, ssp;
  if (m_cbhe) {
    scheme = "ipn";
    ssp = IpnSsp(m_dstSchemeOffset, m_dstSspOffset);
  } else {
    scheme = m_dictionary.substr(m_dstSchemeOffset).c_str();
    ssp = m_dictionary.substr(m_dstSspOffset).c_str();
  }
  NS_LOG_DEBUG("DST scheme offset " << m_dstSchemeOffset << " SSP offset " << m_dstSspOffset << " parts: " << scheme << " AND " << ssp);
  NS_LOG_DEBUG("DST SSP is " << ssp);

  BpEndpointId eid (scheme, ssp);
  NS_LOG_DEBUG("DST URI IS " << eid.Uri());
  return eid;
}

BpEndpointId 
BpHeader6::GetSourceEid () const
{ 
  NS_LOG_FUNCTION (this);
NS_LOG_DEBUG("cbhe:" << m_cbhe);
  std::string scheme, ssp;
  if (m_cbhe) {
    scheme = "ipn";
    ssp = IpnSsp(m_srcSchemeOffset, m_srcSspOffset);
  } else {
    scheme = m_dictionary.substr(m_srcSchemeOffset).c_str();
    ssp = m_dictionary.substr(m_srcSspOffset).c_str();
  }
  NS_LOG_DEBUG(scheme << " " << ssp);
  BpEndpointId eid (scheme, ssp);
  return eid;
}

BpEndpointId 
BpHeader6::GetCustEid () const
{ 
  NS_LOG_FUNCTION (this);
  std::string scheme, ssp;
  if (m_cbhe) {
    scheme = "ipn";
    ssp = IpnSsp(m_custSchemeOffset, m_custSspOffset);
  } else {
    scheme = m_dictionary.substr(m_custSchemeOffset).c_str();
    ssp = m_dictionary.substr(m_custSspOffset).c_str();
  }
  BpEndpointId eid (scheme, ssp);
  return eid;
}

BpEndpointId
BpHeader6::GetReportEid () const
{ 
  NS_LOG_FUNCTION (this);
  std::string scheme, ssp;
  if (m_cbhe) {
    scheme = "ipn";
    ssp = IpnSsp(m_reportSchemeOffset, m_reportSspOffset);
  } else {
    scheme = m_dictionary.substr(m_reportSchemeOffset).c_str();
    ssp = m_dictionary.substr(m_reportSspOffset).c_str();
  } 
  BpEndpointId eid (scheme, ssp);
  return eid;
}

void 
BpHeader6::SetBlockLength (uint32_t len)
{ 
  NS_LOG_FUNCTION (this << " " << len);
  m_blockLength = len;
}

uint32_t
BpHeader6::GetBlockLength () const 
{ 
  NS_LOG_FUNCTION (this);
  return m_blockLength;
}

} // namespace ns3
