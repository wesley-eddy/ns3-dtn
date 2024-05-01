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
#ifndef BP_BLOCK_HEADER_H
#define BP_BLOCK_HEADER_H

#include <stdint.h>
#include "ns3/header.h"
#include "ns3/buffer.h"

namespace ns3 {

/**
 * \brief Bundle payload block header
 *
 * The format of bundle payload block header, which is defined in section 4.5 of RFC 5050.
 *
 */
class BpBlockHeader : public Header
{
public:
  BpBlockHeader (uint8_t blockType);

  virtual ~BpBlockHeader ();

  /**
   * block processing control flags, section 4.3 of RFC 5050
   */
  typedef enum {
    BLOCK_REPLICATE            = 1 << 0,
    TX_STATUS_REPORT           = 1 << 1,
    DELETE_BLOCK               = 1 << 2,
    DISCARD_BLOCK              = 1 << 4
  } ProcessingControlFlags;

  /**
   * block types that are applicable to bpv6 & bpv7
   */
  typedef enum {
    BUNDLE_PAYLOAD_BLOCK       = 1
  } BlockType;

  // Setters

  //Configure bundle processing control flags

  /**
   * \brief Block must or mustn't be replicated in every fragment
   */
  virtual void SetBlockReplicate (bool value);

  /**
   * \brief Do or don't transmit status report if block can't be processed
   */
  virtual void SetTxStatusReport (bool value);

  /**
   * \brief Do or don't delete bundle if block can't be processed
   */
  virtual void SetDeleteBlock (bool value);

  /**
   * \brief Do or don't discard block if it can't be processed
   */
  virtual void SetDiscardBlock (bool value);

  // Getters

  //Get bundle processing control flags

  /**
   * \return Must block be replicated in every fragment?
   */
  virtual bool BlockReplicate () const;

  /**
   * \return Does the bundle node need to send tranmit status report if block can't be processed?
   */
  virtual bool TxStatusReport () const;

  /**
   * \return Does the bundle node need to delete bundle if block can't be processed?
   */
  virtual bool DeleteBlock () const;

  /**
   * \return Does the bundle node need to discard block if it can't be processed?
   */
  virtual bool DiscardBlock () const;


  // pTODO: add comments describing each
  virtual uint32_t GetSerializedSize (void) const = 0;
  virtual void Serialize (Buffer::Iterator start) const = 0;
  virtual uint32_t Deserialize (Buffer::Iterator start) = 0;


protected:
  uint16_t m_length;                 /// the length of header // pTODO: is this in both?
  uint8_t m_blockType;               /// block type
  uint8_t m_processingControlFlags;  /// block processing control flags
};



} // namespace ns3

#endif /* BP_BLOCK_HEADER_H */
