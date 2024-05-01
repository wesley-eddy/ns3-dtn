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
 * Author: Pierce Forte <pierceforte12@gmail.com>
 */
#ifndef BP_BUNDLE_7_H
#define BP_BUNDLE_7_H

#include "bp-bundle.h"
#include "bp-header-7.h"
#include "bp-block-header-7.h"

namespace ns3 {

class Bundle7 : public Bundle
{
public:
  Bundle7 (Ptr<Packet> adu);
  
  virtual ~Bundle7 ();

  static TypeId GetTypeId(void);

  void DoDispose(void);

  void SetPrimaryHeader(BpHeader* primaryHeader);

  void SetPayloadHeader(BpBlockHeader* payloadHeader);

  BpHeader7* GetPrimaryHeader();

  BpBlockHeader7* GetPayloadHeader();

  /*
  pTODO if unique events exist for v7, then override ClearEvents(), call the parent method, then clear these unique events
  void ClearEvents();
  */

  // pTODO: add member variables unique to v7 bundles

private:
  BpHeader7* m_primaryHeader;

  BpBlockHeader7* m_payloadHeader;

};

} // namespace ns3

#endif /* BP_BUNDLE_7 */
