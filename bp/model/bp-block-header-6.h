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
#ifndef BP_BLOCK_HEADER_6_H
#define BP_BLOCK_HEADER_6_H

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
class BpBlockHeader6 : public BpBlockHeader
{
public:
  BpBlockHeader6 (uint8_t blockType = BUNDLE_PAYLOAD_BLOCK);

  virtual ~BpBlockHeader6 ();

  /**
   * block processing control flags, section 4.3 of RFC 5050
   */
  typedef enum {
    LAST_BLOCK                 = 1 << 3,
    FORWARD_WITHOUT_PROCESS    = 1 << 5,
    EID_REFERENCE              = 1 << 6
  } ProcessingControlFlags6;

  /**
   * block types specific to bpv6
   */
  typedef enum {
    BUNDLE_AUTHENTICATION_BLOCK   = 2,
    PAYLOAD_INTEGRITY_BLOCK       = 3,
    PAYLOAD_CONFIDENTIALITY_BLOCK = 4,
    PREVIOUS_HOP_INSERTION_BLOCK  = 5,
    METADATA_EXTENSION_BLOCK      = 8,
    EXTENSION_SECURITY_BLOCK      = 9
  } BlockType6;

  // Setters

  //Configure bundle processing control flags

  /**
   * \brief Is or isn't last block
   */
  void SetLastBlock (bool value);

  /**
   * \brief Block was or wasn't forwarded without being processed
   */
  void SetForwardWithoutProcess (bool value);

  /**
   * \brief Block does or doesn't contain an EID-reference field
   */
  void SetEidReference (bool value);

  /**
   * \brief set the block length in byte, which is the bundle payload size
   */
  void SetBlockLength (uint32_t len);

  // Getters

  //Get bundle processing control flags

  /**
   * \return Is this last block?
   */
  bool LastBlock () const;

  /**
   * \return Was block forwarded without being processed?
   */
  bool ForwardWithoutProcess () const;

  /**
   * \return Does block contain an EID-reference field?
   */
  bool EidReference () const;

  /**
   * \return the block length in byte, which is the bundle payload size
   */
  uint32_t GetBlockLength () const;

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

protected:
  uint32_t m_blockLength;            /// block length
};



} // namespace ns3

#endif /* BP_BLOCK_HEADER_6_H */
