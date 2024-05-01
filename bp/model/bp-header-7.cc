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
 *         Carter T. Edmond <carter.t.edmond1@gmail.com>
 */

#include "ns3/log.h"
#include "ns3/node.h"
#include "bp-header-7.h"
#include <stdio.h>
#include <string>

#include "codec.h"
#include <vector>

NS_LOG_COMPONENT_DEFINE ("BpHeader7");

namespace ns3 {

BpHeader7::BpHeader7 (bool useCbhe)
  : BpHeader(7, useCbhe)
{ 
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG("BpHeader7 *** " << useCbhe);
}

BpHeader7::~BpHeader7 ()
{ 
  NS_LOG_FUNCTION (this);
}

TypeId 
BpHeader7::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpHeader7")
    .SetParent<BpHeader> ()
    .AddConstructor<BpHeader7> ()
  ;

  return tid;
}

TypeId 
BpHeader7::GetInstanceTypeId (void) const
{ 
  NS_LOG_FUNCTION (this);
  return GetTypeId ();
}

void 
BpHeader7::Print (std::ostream &os) const
{ 
  NS_LOG_FUNCTION (this);
}

uint32_t
BpHeader7::SerializeAndGetSize (Buffer::Iterator start, bool forReal) const
{
  NS_LOG_FUNCTION (this);
  
  uint32_t length = 0;
  Buffer::Iterator i = start;
  std::vector<uint8_t> temp;

  if (IsFragment()) length = CborLite::encodeArraySize(temp, 10u);
  else length = CborLite::encodeArraySize(temp, 8u);

  length += CborLite::encodeInteger(temp, m_version);

  length += CborLite::encodeInteger(temp, m_processingFlags);

  length += CborLite::encodeInteger(temp, 0);   // CRC type place holder

  length += EncodeEID(temp, m_dstUriCode, m_dstSsp);
  
  length += EncodeEID(temp, m_srcUriCode, m_srcSsp);

  length += EncodeEID(temp, m_reportUriCode, m_reportSsp);

  length += CborLite::encodeArraySize(temp, 2u);
  length += CborLite::encodeInteger(temp, m_createTimestamp);
  length += CborLite::encodeInteger(temp, m_timestampSeqNum.GetValue());   
  length += CborLite::encodeInteger(temp, m_lifeTime.GetMilliSeconds());          

  if (IsFragment()){
      length += CborLite::encodeInteger(temp, m_fragOffset);
      length += CborLite::encodeInteger(temp, m_aduLength);
  }

  if (forReal) for(uint32_t k = 0; k < temp.size (); k++) i.WriteU8(temp.at(k));

  return length;
}

uint32_t 
BpHeader7::GetSerializedSize (void) const
{
  Buffer b(300); 
  return SerializeAndGetSize(b.Begin(), false);
}

void 
BpHeader7::Serialize (Buffer::Iterator start) const
{
  SerializeAndGetSize(start, true);
}

uint32_t 
BpHeader7::Deserialize (Buffer::Iterator start)
{ 
  NS_LOG_FUNCTION (this);

  Buffer::Iterator i = start;
  size_t j, size;
  std::vector<uint8_t> temp;
  size =  i.GetRemainingSize();

  // Creates buffer for cbor decoding
  for(j = 0; j < size; j++) temp.push_back(i.ReadU8());
  std::vector<uint8_t>::iterator pos = begin(temp);
  std::vector<uint8_t>::iterator stop = end(temp);

  size_t dataSize, tempNumeric;
  uint32_t seqNum, msLifeTime;
  
  uint32_t length = CborLite::decodeArraySize(pos, stop, dataSize);

  length += CborLite::decodeUnsigned(pos, stop, m_version); 
  length += CborLite::decodeUnsigned(pos, stop, m_processingFlags);
  length += CborLite::decodeUnsigned(pos, stop, tempNumeric); // Handles CRC type place holder value


  length += DecodeEID(pos, stop, m_dstUriCode, m_dstSsp);
  length += DecodeEID(pos, stop, m_srcUriCode, m_srcSsp);
  length += DecodeEID(pos, stop, m_reportUriCode, m_reportSsp);

  length += CborLite::decodeArraySize(pos, stop, tempNumeric);
  length += CborLite::decodeUnsigned(pos, stop, m_createTimestamp);
  length += CborLite::decodeUnsigned(pos, stop, seqNum);
  m_timestampSeqNum = (uint32_t)seqNum;

  length += CborLite::decodeUnsigned(pos, stop, msLifeTime);
  m_lifeTime = MilliSeconds(msLifeTime);

  if(dataSize > 8){
      length += CborLite::decodeUnsigned(pos, stop, m_fragOffset);
      length += CborLite::decodeUnsigned(pos, stop, m_aduLength);
  }
  return length;
}

void
BpHeader7::SetStatusTimeReq (const bool value)
{ 
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingFlags |= REQ_STATUS_TIME_IN_REPORTS;
  else
    m_processingFlags &= (~(REQ_STATUS_TIME_IN_REPORTS));
}

bool 
BpHeader7::StatusTimeReq () const {
  NS_LOG_FUNCTION (this);
  uint32_t flag = m_processingFlags & REQ_STATUS_TIME_IN_REPORTS;
  return flag;
}

void 
BpHeader7::SetDestinationEid (const BpEndpointId &dst)
{ 
  NS_LOG_FUNCTION (this << " " << dst.Uri ());
  
  std::string scheme = dst.Scheme ();
  std::string ssp = dst.Ssp ();

  if (scheme == "ipn") {
    m_dstUriCode = 2u;
    m_dstSsp = IpnSsp(IpnNode(ssp), IpnService(ssp));
  } else {
    m_dstUriCode = 1u;
    m_dstSsp = ssp;
  }
}

void 
BpHeader7::SetSourceEid (const BpEndpointId &src)
{ 
  NS_LOG_FUNCTION (this << " " << src.Uri ());
  
  std::string scheme = src.Scheme ();
  std::string ssp = src.Ssp ();

  if (scheme == "ipn") {
    m_srcUriCode = 2u;
    m_srcSsp = IpnSsp(IpnNode(ssp), IpnService(ssp));
  } else {
    m_srcUriCode = 1u;
    m_srcSsp = ssp;
  }

}

void 
BpHeader7::SetReportEid (const BpEndpointId &report)
{ 
  NS_LOG_FUNCTION (this << " " << report.Uri ());
  
  std::string scheme = report.Scheme ();
  std::string ssp = report.Ssp ();

  if (scheme == "ipn") {
    m_reportUriCode = 2u;
    m_reportSsp = IpnSsp(IpnNode(ssp), IpnService(ssp));
  } else {
    m_reportUriCode = 1u;
    m_reportSsp = ssp;
  }
}

uint8_t 
BpHeader7::Priority () const   
{
  NS_LOG_FUNCTION (this << "procflags: " << m_processingFlags << " priority: " << 0);
  return 0; // no priority field in v7 so priority is 0
}

BpEndpointId 
BpHeader7::GetDestinationEid () const
{ 
  NS_LOG_FUNCTION (this);
  
  //pTODO implement
  BpEndpointId eid ("", "");
  return eid;
}

BpEndpointId 
BpHeader7::GetSourceEid () const
{ 
  NS_LOG_FUNCTION (this);
  
  //pTODO implement
  BpEndpointId eid ("", "");
  return eid;
}

BpEndpointId
BpHeader7::GetReportEid () const
{ 
  NS_LOG_FUNCTION (this);
  
  //pTODO implement
  BpEndpointId eid ("", "");
  return eid;
}

void 
BpHeader7::SetBlockLength (uint32_t len)
{ 
  NS_LOG_FUNCTION (this << " " << len);
  m_blockLength = len;
}

uint32_t
BpHeader7::GetBlockLength () const 
{ 
  NS_LOG_FUNCTION (this);
  return m_blockLength;
}

uint8_t
BpHeader7::EncodeEID(std::vector<uint8_t> &buffer, uint16_t uriCode, std::string ssp) const
{       
  uint8_t length = CborLite::encodeArraySize(buffer, 2u);
  if (uriCode == 1){
    length += CborLite::encodeInteger(buffer, 1);
    if (ssp == "none") length += CborLite::encodeInteger(buffer, 0);
    else length += CborLite::encodeBytes(buffer, ssp);
  }
  else{ 
    uint32_t n, s;
    sscanf(ssp.c_str(), "%d.%d", &n, &s);
    length += CborLite::encodeInteger(buffer, 2);
    length += CborLite::encodeArraySize(buffer, 2u);
    length += CborLite::encodeInteger(buffer, n);
    length += CborLite::encodeInteger(buffer, s);
  }
  return length;
}

uint32_t 
BpHeader7::DecodeEID(std::vector<uint8_t>::iterator &pos, std::vector<uint8_t>::iterator end, uint16_t &uri, std::string &ssp)
{
  uint32_t length;
  size_t tempNumeric;

  length = CborLite::decodeArraySize(pos, end, tempNumeric);
  length += CborLite::decodeUnsigned(pos, end, uri);
  if (uri == 2 ){
    length += CborLite::decodeArraySize(pos, end, tempNumeric);
    uint32_t node, service;
    length += CborLite::decodeUnsigned(pos, end, node);
    length += CborLite::decodeUnsigned(pos, end, service);
    char tmp[1024];
    sprintf(tmp, "%u.%u", node, service);
    ssp = tmp;
  }
  else if ((int)(*pos) == 00) {
      length += CborLite::decodeUnsigned(pos, end, tempNumeric);
      ssp = std::to_string((int)tempNumeric);
  }
  else length += CborLite::decodeBytes(pos, end, ssp);  
  return length;
}

} // namespace ns3
