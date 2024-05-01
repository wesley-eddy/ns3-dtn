/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 University of New Brunswick
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
 * Author: Dizhi Zhou <dizhi.zhou@gmail.com>
 */

#ifndef BP_ADMIN_RECORD_H
#define BP_ADMIN_RECORD_H

#include "ns3/header.h"

namespace ns3 {

class AdminRecord : public Header {
public:
  AdminRecord() {};
  virtual ~AdminRecord() {};

  static TypeId GetTypeId() {
    static TypeId tid = TypeId("ns3::BpAdminRecord")
      .SetParent<Header> ()
      .AddConstructor<AdminRecord> ();
    return tid;
  }

  virtual TypeId GetInstanceTypeId() const { return GetTypeId(); }

  virtual uint32_t GetSerializedSize() const { return 1; }
  virtual void Print(std::ostream &os) const {};
  virtual void Serialize(Buffer::Iterator start) const {
    start.WriteU8(typeFlags);
  };
  virtual uint32_t Deserialize(Buffer::Iterator start) {
    typeFlags = start.ReadU8();
    return 1;
  }

  uint8_t typeFlags;

  #define _BP_AR_CS 0x2
  #define _BP_AR_FRAG 0x1
  #define _BP_AR_ACS 0x4
};

} // namespace ns3

#endif /* BP_ADMIN_RECORD_H */
