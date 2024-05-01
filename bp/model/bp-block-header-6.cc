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
 * Authors: Shawn McGraw <shawn.mcgraw@nasa.gov>
 *          Pierce Forte <pierceforte12@gmail.com>
 *          Dizhi Zhou <dizhi.zhou@gmail.com>
 */

#include "ns3/log.h"
#include "bp-block-header-6.h"
#include "sdnv.h"
#include <stdio.h>
#include <vector>

NS_LOG_COMPONENT_DEFINE ("BpBlockHeader6");

namespace ns3 {

BpBlockHeader6::BpBlockHeader6 (uint8_t blockType)
  : BpBlockHeader (blockType),
    m_blockLength (0)
{
  NS_LOG_FUNCTION (this);
}

BpBlockHeader6::~BpBlockHeader6 ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
BpBlockHeader6::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpBlockHeader6")
    .SetParent<BpBlockHeader> ()
    .AddConstructor<BpBlockHeader6> ()
  ;

  return tid;
}

TypeId
BpBlockHeader6::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);
  return GetTypeId ();
}

uint32_t
BpBlockHeader6::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return m_length;
}

void
BpBlockHeader6::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this);
}

void
BpBlockHeader6::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this);
  Buffer::Iterator i = start;
  Sdnv sdnv;
  std::vector<uint8_t> result; // store encoded results

  std::vector<uint8_t> processingControlFlags = sdnv.Encode (m_processingControlFlags);
  std::vector<uint8_t> blockLength = sdnv.Encode (m_blockLength);
  result.insert (result.end (), processingControlFlags.begin (), processingControlFlags.end ());
  result.insert (result.end (), blockLength.begin (), blockLength.end ());

  i.WriteU8 (m_blockType);

  for (uint32_t k = 0; k < result.size (); k++)
    i.WriteU8 (result.at (k));

  // pTODO: modify encode, maybe return end of iterator so the serilization can be added to
}

uint32_t
BpBlockHeader6::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this);
  Buffer::Iterator i = start;
  Sdnv sdnv;

  m_blockType = i.ReadU8 ();
  m_processingControlFlags = (uint8_t) sdnv.Decode (i);
  m_blockLength = (uint32_t) sdnv.Decode (i);

  return GetSerializedSize ();
}

void
BpBlockHeader6::SetLastBlock (bool value)
{
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingControlFlags |= LAST_BLOCK;
  else
    m_processingControlFlags &= (~(LAST_BLOCK));
}

void
BpBlockHeader6::SetForwardWithoutProcess (bool value)
{
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingControlFlags |= FORWARD_WITHOUT_PROCESS;
  else
    m_processingControlFlags &= (~(FORWARD_WITHOUT_PROCESS));
}

void
BpBlockHeader6::SetEidReference (bool value)
{
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingControlFlags |= EID_REFERENCE;
  else
    m_processingControlFlags &= (~(EID_REFERENCE));
}

void
BpBlockHeader6::SetBlockLength (uint32_t len)
{
  NS_LOG_FUNCTION (this << " " << len);
  m_blockLength = len;
}

bool
BpBlockHeader6::LastBlock () const
{
  NS_LOG_FUNCTION (this);
  return m_processingControlFlags & LAST_BLOCK;
}

bool
BpBlockHeader6::ForwardWithoutProcess () const
{
  NS_LOG_FUNCTION (this);
  return m_processingControlFlags & FORWARD_WITHOUT_PROCESS;
}

bool
BpBlockHeader6::EidReference () const
{
  NS_LOG_FUNCTION (this);
  return m_processingControlFlags & EID_REFERENCE;
}

uint32_t
BpBlockHeader6::GetBlockLength () const
{
  NS_LOG_FUNCTION (this);
  return m_blockLength;
}


} // namespace ns3
