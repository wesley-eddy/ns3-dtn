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
#include "bp-header.h"
#include <stdio.h>
#include <string>

#include "sdnv.h"
#include <vector>

NS_LOG_COMPONENT_DEFINE ("BpHeader");

namespace ns3 {

BpHeader::BpHeader (uint8_t version, bool useCbhe)
  : m_version (version),
    m_processingFlags (0),
    m_createTimestamp (0),
    m_timestampSeqNum (0),
    m_lifeTime (0),
    m_fragOffset (0),
    m_aduLength (0),
    m_cbhe (useCbhe)
{ 
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG("BpHeader *** " << useCbhe);
}

BpHeader::~BpHeader ()
{ 
  NS_LOG_FUNCTION (this);
}

void 
BpHeader::SetIsFragment (const bool value)
{ 
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingFlags |= BUNDLE_IS_FRAGMENT;
  else
    m_processingFlags &= (~(BUNDLE_IS_FRAGMENT));
}

void 
BpHeader::SetIsAdmin (const bool value)
{ 
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingFlags |= BUNDLE_IS_ADMIN;
  else
    m_processingFlags &= (~(BUNDLE_IS_ADMIN));
}

void
BpHeader::SetDonotFragment (const bool value)
{ 
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingFlags |= BUNDLE_DO_NOT_FRAGMENT;
  else
    m_processingFlags &= (~(BUNDLE_DO_NOT_FRAGMENT));

}

void
BpHeader::SetAckbyAppReq (const bool value)
{ 
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingFlags |= BUNDLE_ACK_BY_APP;
  else
    m_processingFlags &= (~(BUNDLE_ACK_BY_APP));

}

void 
BpHeader::SetReceptionReport (const bool value)
{ 
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingFlags |= REQ_REPORT_BUNDLE_RECEPTION;
  else
    m_processingFlags &= (~(REQ_REPORT_BUNDLE_RECEPTION));
}

void 
BpHeader::SetForwardReport (const bool value)
{ 
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingFlags |= REQ_REPORT_BUNDLE_FORWARD;
  else
    m_processingFlags &= (~(REQ_REPORT_BUNDLE_FORWARD));

}

void 
BpHeader::SetDeliveryReport (const bool value)
{ 
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingFlags |= REQ_REPORT_BUNDLE_DELIVERY;
  else
    m_processingFlags &= (~(REQ_REPORT_BUNDLE_DELIVERY));

}

void 
BpHeader::SetDeletionReport (const bool value)
{ 
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingFlags |= REQ_REPORT_BUNDLE_DELETION;
  else
    m_processingFlags &= (~(REQ_REPORT_BUNDLE_DELETION));

}

bool 
BpHeader::IsFragment () const
{ 
  NS_LOG_FUNCTION (this);
  return m_processingFlags & BUNDLE_IS_FRAGMENT;
}

bool 
BpHeader::IsAdmin () const
{ 
  NS_LOG_FUNCTION (this);
  return m_processingFlags & BUNDLE_IS_ADMIN;
}

bool 
BpHeader::DonotFragment () const
{ 
  NS_LOG_FUNCTION (this);
  return m_processingFlags & BUNDLE_DO_NOT_FRAGMENT;
}

bool 
BpHeader::AckbyAppReq () const
{ 
  NS_LOG_FUNCTION (this);
  return m_processingFlags & BUNDLE_ACK_BY_APP;
}

bool 
BpHeader::ReceptionReport () const
{ 
  NS_LOG_FUNCTION (this);
  return m_processingFlags & REQ_REPORT_BUNDLE_RECEPTION;
}

bool 
BpHeader::ForwardReport () const
{ 
  NS_LOG_FUNCTION (this);
  return m_processingFlags & REQ_REPORT_BUNDLE_FORWARD;
}

bool 
BpHeader::DeliveryReport () const
{ 
  NS_LOG_FUNCTION (this);
  return m_processingFlags & REQ_REPORT_BUNDLE_DELIVERY;
}

bool 
BpHeader::DeletionReport () const
{ 
  NS_LOG_FUNCTION (this);
  return m_processingFlags & REQ_REPORT_BUNDLE_DELETION;
}

void 
BpHeader::SetCreateTimestamp (const uint32_t timestamp)
{ 
  NS_LOG_FUNCTION (this << " " << timestamp);
  m_createTimestamp = timestamp;
}

void
BpHeader::SetSequenceNumber (const SequenceNumber32 &sequenceNumber)
{ 
  NS_LOG_FUNCTION (this << " " << sequenceNumber.GetValue ());
  m_timestampSeqNum = sequenceNumber;
}


uint32_t
BpHeader::GetCreateTimestamp () const
{ 
  NS_LOG_FUNCTION (this);
  return m_createTimestamp;
}

SequenceNumber32 
BpHeader::GetSequenceNumber () const
{ 
  NS_LOG_FUNCTION (this);
  return m_timestampSeqNum;
}

uint32_t 
BpHeader::IpnNode(std::string &ssp) {
  uint32_t n, s;
  sscanf(ssp.c_str(), "%d.%d", &n, &s);
  return n;
}

uint32_t 
BpHeader::IpnService(std::string &ssp) {
  uint32_t n, s;
  sscanf(ssp.c_str(), "%d.%d", &n, &s);
  return s;
}

std::string 
BpHeader::IpnSsp(uint32_t node, uint32_t service) const {
  char tmp[1024];
  NS_LOG_DEBUG("IpnSsp: " << node << " . " << service);
  sprintf(tmp, "%u.%u", node, service);
  return std::string(tmp);
}


void 
BpHeader::SetLifeTime (Time lifetime)
{ 
  NS_LOG_FUNCTION (this << " " << lifetime.GetSeconds());
  m_lifeTime = lifetime;
}

Time
BpHeader::GetLifeTime () const
{ 
  NS_LOG_FUNCTION (this);
  return m_lifeTime;
}


void 
BpHeader::SetFragOffset (uint32_t offset)
{ 
  NS_LOG_FUNCTION (this << " " << offset);
  m_fragOffset = offset;
}

void 
BpHeader::SetAduLength (uint32_t len)
{ 
  NS_LOG_FUNCTION (this << " " << len);
  m_aduLength = len;
}

uint32_t
BpHeader::GetFragOffset () const
{ 
  NS_LOG_FUNCTION (this);
  return m_fragOffset;
}

uint32_t
BpHeader::GetAduLength () const
{ 
  NS_LOG_FUNCTION (this);
  return m_aduLength;
}

void
BpHeader::SetVersion (uint8_t ver)
{ 
  NS_LOG_FUNCTION (this);
  m_version = ver;
}

uint8_t 
BpHeader::GetVersion () const
{ 
  NS_LOG_FUNCTION (this);
  return m_version;
}

} // namespace ns3
