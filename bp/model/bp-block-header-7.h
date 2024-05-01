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
#ifndef BP_BLOCK_HEADER_7_H
#define BP_BLOCK_HEADER_7_H

#include <stdint.h>
#include "bp-block-header.h"
#include "ns3/header.h"
#include "ns3/buffer.h"

namespace ns3 {

/**
 * \brief Bundle payload block header
 *
 * The format of bundle payload block header, which is defined in section 4.5 of RFC 5050.
 *
 */
class BpBlockHeader7 : public BpBlockHeader
{
public:
  BpBlockHeader7 (uint8_t blockType = BUNDLE_PAYLOAD_BLOCK);

  virtual ~BpBlockHeader7 ();

  // no block processing control flags are unique to bpv7.

  /**
   * block types specific to bpv7
   */
  typedef enum {
    PREVIOUS_NODE   = 6,
    BUNDLE_AGE      = 7,
    HOP_COUNT       = 10
  } BlockType7;

  /**
  * \brief set the block CRC type
  */
  void SetBlockCrcType (uint8_t crcType);

  /**
  * \brief set the block CRC
  */
 void SetBlockCrc (uint64_t crc);

  /**
   * \brief set the block number
   */
  void SetBlockNumber (uint8_t blockNumber);

  /**
   * \return the block CRC type
   */
  uint8_t BlockCrcType () const;

  /**
   * \return the block CRC
   */
  uint64_t BlockCrc () const;

  /**
   * \return the block number
   */
  uint8_t BlockNumber () const;

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;

  virtual uint32_t GetSerializedSize (void) const;
  uint32_t SerializeAndGetSize(Buffer::Iterator start, bool forReal) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  uint8_t m_crcType;                 /// CRC type in block header
  uint64_t m_crc;                    /// CRC of block
  uint8_t m_blockNumber;             /// block number
};



} // namespace ns3

#endif /* BP_BLOCK_HEADER_7_H */
