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

#include "bp-block-header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("BpBlockHeader");

namespace ns3 {

BpBlockHeader::BpBlockHeader (uint8_t blockType)
  : m_length (6),
    m_blockType (blockType),
    m_processingControlFlags (0)
{
  NS_LOG_FUNCTION (this);
}

BpBlockHeader::~BpBlockHeader ()
{
  NS_LOG_FUNCTION (this);
}

void
BpBlockHeader::SetBlockReplicate (bool value)
{
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingControlFlags |= BLOCK_REPLICATE;
  else
    m_processingControlFlags &= (~(BLOCK_REPLICATE));
}

void
BpBlockHeader::SetTxStatusReport (bool value)
{
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingControlFlags |= TX_STATUS_REPORT;
  else
    m_processingControlFlags &= (~(TX_STATUS_REPORT));
}

void
BpBlockHeader::SetDeleteBlock (bool value)
{
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingControlFlags |= DELETE_BLOCK;
  else
    m_processingControlFlags &= (~(DELETE_BLOCK));
}

void
BpBlockHeader::SetDiscardBlock (bool value)
{
  NS_LOG_FUNCTION (this << " " << value);
  if (value)
    m_processingControlFlags |= DISCARD_BLOCK;
  else
    m_processingControlFlags &= (~(DISCARD_BLOCK));
}

bool
BpBlockHeader::BlockReplicate () const
{
  NS_LOG_FUNCTION (this);
  return m_processingControlFlags & BLOCK_REPLICATE;
}

bool
BpBlockHeader::TxStatusReport () const
{
  NS_LOG_FUNCTION (this);
  return m_processingControlFlags & TX_STATUS_REPORT;
}

bool
BpBlockHeader::DeleteBlock () const
{
  NS_LOG_FUNCTION (this);
  return m_processingControlFlags & DELETE_BLOCK;
}

bool
BpBlockHeader::DiscardBlock () const
{
  NS_LOG_FUNCTION (this);
  return m_processingControlFlags & DISCARD_BLOCK;
}


} // namespace ns3
