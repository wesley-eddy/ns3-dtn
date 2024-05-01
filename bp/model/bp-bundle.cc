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
 *          Wesley Eddy <wes@mti-systems.com>
 */

#include "bp-bundle.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("Bundle");

namespace ns3 {

Bundle::Bundle(Ptr<Packet> adu) {
  NS_LOG_FUNCTION("bundle creation");
  m_adu = adu;
  retentionConstraints = 0;
}

Bundle::~Bundle() {}

void
Bundle::DoDispose(void) {
  NS_LOG_FUNCTION("bundle disposal");
  m_adu = NULL;
  if (GetPrimaryHeader()->GetLifeTime() != 0) Simulator::Remove(expireEvent);
  acks.clear();
  NS_LOG_DEBUG("refcnt: " << this->GetReferenceCount());
}

void
Bundle::ClearEvents() {
  if (GetPrimaryHeader()->GetLifeTime() != 0 && expireEvent.IsRunning()) {
    expireEvent.Cancel();
  }
}

} // namespace ns3