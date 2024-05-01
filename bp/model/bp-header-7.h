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
 *         Carter T. Edmond <carter.t.edmond1@gmail.com>
 */
#ifndef BP_HEADER_7_H
#define BP_HEADER_7_H

#include <stdint.h>
#include <string>
#include "bp-header.h"
#include "ns3/header.h"
#include "ns3/nstime.h"
#include "ns3/buffer.h"
#include "ns3/sequence-number.h"
#include "bp-endpoint-id.h"

namespace ns3 {

/**
 * \brief Bundle protocol 7 header
 *
 * The format of bpv7 primary bundle header, which is defined in https://datatracker.ietf.org/doc/html/draft-ietf-dtn-bpbis-30#section-4.3.1
 *
 */
class BpHeader7 : public BpHeader
{
public:
  BpHeader7 (bool useCbhe = false);
  
  virtual ~BpHeader7 ();

  // Setters

  // Configure bundle processing flags

  /**
   * \brief Status time in reports is/isn't requested
   */
  void SetStatusTimeReq (const bool value);

  /**
   * \brief set the destination endpoint id and update the length
   *
   * \param dst the destination endpoint id
   */
  void SetDestinationEid (const BpEndpointId &dst);

  /**
   * \brief set the source endpoint id and update the length
   *
   * \param src the source endpoint id
   */
  void SetSourceEid (const BpEndpointId &src);

  /**
   * \brief set the report endpoint id and update the length
   *
   * \param report the report endpoint id
   */
  void SetReportEid (const BpEndpointId &report);

  // Getters

  // Get bundle processing flags

  /**
   * \brief Is status time in reports requested?
   */
  bool StatusTimeReq () const;

  /**
   * \brief Get priority of bundle
   *
   * \return priority of bundle
   */
  uint8_t Priority () const; 

  /**
   * \return the destination endpoint id
   */
  BpEndpointId GetDestinationEid () const;

  /**
   * \return the source endpoint id
   */
  BpEndpointId GetSourceEid () const;

  /**
   * \return the report endpoint id
   */
  BpEndpointId GetReportEid () const;

  /**
   * \brief Helper function to encode EID
   * 
   * \param buffer the buffer to store encoded EID
   * \param uriCode the EID Uris
   * \param ssp the EID ssp
   * 
   * \return the expected size in bytes of the encoded EID
   */
  uint8_t EncodeEID(std::vector<uint8_t> &buffer, uint16_t uriCode, std::string ssp) const;

  /**
   * \brief Helper function to decode EIDs
   * 
   * \param pos an iterator pointing to start of encoded EID
   * \param end an iterator pointing to end of encoded data 
   * 
   * \return the number of bytes decoded
   */
  uint32_t DecodeEID(std::vector<uint8_t>::iterator &pos, std::vector<uint8_t>::iterator end, uint16_t &uri, std::string &ssp);


  /**
   * process flags
   */
  typedef enum {
    REQ_STATUS_TIME_IN_REPORTS                  = 1 << 6 
  } ProcessingFlags7;  

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;

  uint32_t GetSerializedSize (void) const;
  uint32_t SerializeAndGetSize(Buffer::Iterator start, bool forReal) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);

  void SetBlockLength (uint32_t len);
  uint32_t GetBlockLength () const;

private:
  // unique to primary bundle block https://datatracker.ietf.org/doc/html/draft-ietf-dtn-bpbis-30#section-4.3.1
  uint16_t m_dstUriCode;
  std::string m_dstSsp; 
  uint16_t m_srcUriCode;
  std::string m_srcSsp; 
  uint16_t m_reportUriCode;
  std::string m_reportSsp; 

  // Not included in bpv7; strictly for simulator purposes
  uint32_t m_blockLength;
};


} // namespace ns3

#endif /* BP_HEADER_H */
