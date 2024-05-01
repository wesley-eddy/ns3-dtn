#ifndef ACS_CTEB_H
#define ACS_CTEB_H

#include <stdint.h>
#include "ns3/header.h"
#include "ns3/buffer.h"

namespace ns3 {

class CTEB : public Header
{
public:
  CTEB () {};
  virtual ~CTEB () {};

  static TypeId GetTypeId (void) {
    static TypeId tid = TypeId("ns3::CTEB")
      .SetParent<Header> ()
      .AddConstructor<CTEB> ();
    return tid;
  }

  virtual TypeId GetInstanceTypeId (void) const { return GetTypeId(); }

  virtual void Print (std::ostream &os) const {};
  virtual uint32_t GetSerializedSize (void) const { return 1; } // TODO
  virtual void Serialize (Buffer::Iterator start) const { };
  virtual uint32_t Deserialize (Buffer::Iterator start) { return 1; } // TODO

  uint64_t custodyID;
  std::string creatorEID; 
};

class ACS : public Header
{
public:
  ACS () {}
  virtual ~ACS () {}

  static TypeId GetTypeId (void) {
    static TypeId tid = TypeId("ns3::ACS")
      .SetParent<Header> ()
      .AddConstructor<ACS> ();
    return tid;
  }

  virtual TypeId GetInstanceTypeId (void) const { return GetTypeId(); }

  virtual void Print (std::ostream &os) const {};
  virtual uint32_t GetSerializedSize (void) const { return 1; } // TODO;
  virtual void Serialize (Buffer::Iterator start) const { };
  virtual uint32_t Deserialize (Buffer::Iterator start) { return 1; } // TODO;

  uint8_t status;
  uint64_t leftEdgeFirstFill;
  uint64_t lenFirstFill;
  std::vector<std::pair<uint64_t, uint64_t>> nextEdgeLenPairs;
};



} // namespace ns3

#endif /* ACS_CTEB_H */
