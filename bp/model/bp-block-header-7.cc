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
 *          Carter T. Edmond <carter.t.edmond1@gmail.com>
 *          Dizhi Zhou <dizhi.zhou@gmail.com>
 */

#include "ns3/log.h"
#include "bp-block-header-7.h"
#include "codec.h"
#include <stdio.h>
#include <vector>

NS_LOG_COMPONENT_DEFINE ("BpBlockHeader7");

namespace ns3 {

BpBlockHeader7::BpBlockHeader7 (uint8_t blockType)
  : BpBlockHeader (blockType),
    m_crcType (0),
    m_crc (0),
    m_blockNumber (0)
{
  NS_LOG_FUNCTION (this);
}

BpBlockHeader7::~BpBlockHeader7 ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
BpBlockHeader7::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpBlockHeader7")
    .SetParent<BpBlockHeader> ()
    .AddConstructor<BpBlockHeader7> ()
  ;

  return tid;
}

TypeId
BpBlockHeader7::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);
  return GetTypeId ();
}

void
BpBlockHeader7::SetBlockCrcType (uint8_t crcType)
{
  // pTODO: implement method
}

void
BpBlockHeader7::SetBlockCrc (uint64_t crc)
{
  NS_LOG_FUNCTION (this << " " << crc);
  m_crc = crc;
}

void
BpBlockHeader7::SetBlockNumber(uint8_t blockNumber)
{
  // pTODO: implement method
}

uint8_t
BpBlockHeader7::BlockCrcType () const
{
  // pTODO: implement method
  return 0;
}

uint64_t
BpBlockHeader7::BlockCrc() const
{
  // pTODO: implement method
  return 0;
}

uint8_t
BpBlockHeader7::BlockNumber() const
{
  //pTODO: implement method
  return 0;
}

uint32_t
BpBlockHeader7::SerializeAndGetSize (Buffer::Iterator start, bool forReal) const
{
  NS_LOG_FUNCTION (this);

  uint32_t length = 0;
  Buffer::Iterator i = start;
  std::vector<uint8_t> temp;

  if (m_crcType != 0) length += CborLite::encodeArraySize(temp, 6u);
  else length += CborLite::encodeArraySize(temp, 5u);
  
  length += CborLite::encodeInteger(temp, m_blockType);
  length += CborLite::encodeInteger(temp, m_blockNumber);
  length += CborLite::encodeInteger(temp, m_processingControlFlags);
  length += CborLite::encodeInteger(temp, m_crcType);

  if (forReal) for(uint32_t k = 0; k < temp.size (); k++) i.WriteU8(temp.at(k));
  return length;
}

void
BpBlockHeader7::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this);
}

uint32_t
BpBlockHeader7::GetSerializedSize (void) const
{
  Buffer b(300); 
  return SerializeAndGetSize(b.Begin(), false);
}

void
BpBlockHeader7::Serialize (Buffer::Iterator start) const
{  
  SerializeAndGetSize(start, true);
}

uint32_t
BpBlockHeader7::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  size_t j, size;
  std::vector<uint8_t> temp;
  size =  i.GetRemainingSize();

  // Creates buffer for cbor decoding
  for(j = 0; j < size; j++) temp.push_back(i.ReadU8());
  
  auto pos = begin(temp);
  auto stop = end(temp);
  size_t length, tempNumeric;

  length = CborLite::decodeArraySize(pos, stop, tempNumeric);
  length += CborLite::decodeUnsigned(pos, stop, m_blockType);
  length += CborLite::decodeUnsigned(pos, stop, m_blockNumber);
  length += CborLite::decodeUnsigned(pos, stop, m_processingControlFlags);
  length += CborLite::decodeUnsigned(pos, stop, m_crcType);

  return length;
}

} // namespace ns3
