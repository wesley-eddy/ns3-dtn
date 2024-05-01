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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1306  USA
 *
 * Author: Pierce Forte <pierceforte12@gmail.com>
 */
#ifndef BP_BUNDLE_6_H
#define BP_BUNDLE_6_H

#include "bp-bundle.h"
#include "bp-header-6.h"
#include "bp-block-header-6.h"

namespace ns3 {

class Bundle6 : public Bundle
{
public:
  Bundle6 (Ptr<Packet> adu);
  
  virtual ~Bundle6 ();

  static TypeId GetTypeId(void);

  void DoDispose(void);

  void SetPrimaryHeader(BpHeader* primaryHeader);

  void SetPayloadHeader(BpBlockHeader* payloadHeader);

  BpHeader6* GetPrimaryHeader();

  BpBlockHeader6* GetPayloadHeader();

  void ClearEvents();

  // pTODO maybe put this in parent
  EventId nextRetrans;

private:
  BpHeader6* m_primaryHeader;

  BpBlockHeader6* m_payloadHeader;

};

} // namespace ns3

#endif /* BP_BUNDLE_6 */
