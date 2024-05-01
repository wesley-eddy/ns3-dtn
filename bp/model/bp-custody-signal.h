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
 * Authors: Wesley Eddy <wes@mti-systems.com>
 *          Dizhi Zhou <dizhi.zhou@gmail.com>
 */
#ifndef BP_CUSTODY_SIGNAL_H
#define BP_CUSTODY_SIGNAL_H

#include "ns3/header.h"
#include "sdnv.h"
#include "ns3/sequence-number.h"
#include "ns3/nstime.h"

namespace ns3 {

class CustodySignal : public Header {
public:
  CustodySignal() {};
  virtual ~CustodySignal() {};

  static TypeId GetTypeId() {
    static TypeId tid = TypeId("ns3::CustodySignal")
      .SetParent<Header> ()
      .AddConstructor<CustodySignal> ();
    return tid;
  }

  virtual TypeId GetInstanceTypeId() const { return GetTypeId(); }

  std::vector<uint8_t> Encode() const {
    std::vector<uint8_t> result;
    Sdnv sdnv;
    result.push_back(status);
    std::vector<uint8_t> sdnvBytes;
    sdnvBytes = sdnv.Encode(fragmentOffset);
    result.insert(result.end(), sdnvBytes.begin(), sdnvBytes.end());
    sdnvBytes = sdnv.Encode(fragmentLength);
    result.insert(result.end(), sdnvBytes.begin(), sdnvBytes.end());
    sdnvBytes = sdnv.Encode(timeOfSignal.GetSeconds()); // TODO not DTN epoch?
    result.insert(result.end(), sdnvBytes.begin(), sdnvBytes.end());
    sdnvBytes = sdnv.Encode(creationTimestamp);
    result.insert(result.end(), sdnvBytes.begin(), sdnvBytes.end());
    sdnvBytes = sdnv.Encode(seqNo.GetValue());
    result.insert(result.end(), sdnvBytes.begin(), sdnvBytes.end());
    sdnvBytes = sdnv.Encode(srcEidLen);
    result.insert(result.end(), sdnvBytes.begin(), sdnvBytes.end());
    result.insert(result.end(), srcEid.begin(), srcEid.end());
    return result;
  }

  virtual uint32_t GetSerializedSize() const { return Encode().size(); }
  virtual void Print(std::ostream &os) const {};
  virtual void Serialize(Buffer::Iterator start) const {
    std::vector<uint8_t> result = Encode();
    for (size_t i = 0; i < result.size(); i++) start.WriteU8(result.at(i));
  };
  virtual uint32_t Deserialize(Buffer::Iterator start) {
     Sdnv sdnv;
     status = start.ReadU8();
     fragmentOffset = (uint32_t)sdnv.Decode(start);
     fragmentLength = (uint32_t)sdnv.Decode(start);
     timeOfSignal = Seconds((uint64_t)sdnv.Decode(start)/1000.0);
     creationTimestamp = (uint32_t)sdnv.Decode(start);
     seqNo = (uint32_t)sdnv.Decode(start);
     srcEidLen = (uint32_t)sdnv.Decode(start);
     srcEid.clear();
     for (size_t j = 0; j < srcEidLen; j++) srcEid.push_back(start.ReadU8());
     return GetSerializedSize();
  };


  // below fields are specific to custody status
  uint8_t status;
  uint32_t fragmentOffset;
  uint32_t fragmentLength;
  Time timeOfSignal;
  uint32_t creationTimestamp;
  SequenceNumber32 seqNo;
  uint32_t srcEidLen;
  std::string srcEid;
};

} // namespace ns3

#endif /* BP_CUSTODY_SIGNAL_H */