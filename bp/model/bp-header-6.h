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
 *          Dizhi Zhou <dizhi.zhou@gmail.com>
 */
#ifndef BP_HEADER_6_H
#define BP_HEADER_6_H

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
 * \brief Bundle protocol 6 header
 *
 * The format of bpv6 primary bundle header, which is defined in section 4.5 of RFC 5050
 *
 */
class BpHeader6 : public BpHeader
{
public:
  BpHeader6 (bool useCbhe = false);
  
  virtual ~BpHeader6 ();

  // Setters

  // Configure bundle processing flags

  /**
   * \brief Custody transfer is/isn't requested
   */
  void SetCustTxReq (const bool value);

  /**
   * \brief Destination endpoint is/isn't a singleton
   */
  void SetSingletonDest (const bool value);

  /**
   * \brief Set priority field
   *
   * \param pri priority of bundle
   */
  void SetPriority (const uint8_t pri);

  /**
   * \brief Do or don't request report of bundle acceptance
   */
  void SetCustAcceptReport (const bool value);

  /**
   * \brief set the length of bundle block
   *
   * \param len the lengh of bundle block
   */
  void SetBlockLength (uint32_t len);

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

  /**
   * \brief set the custodian endpoint id and update the length
   *
   * \param cust the custodian endpoint id
   */
  void SetCustEid (const BpEndpointId &cust);

  // Getters

  // Get bundle processing flags

  /**
   * \brief Is Custody transfer requested?
   */
  bool CustTxReq () const;

  /**
   * \brief Is destination endpoint a singleton?
   */
  bool SingletonDest () const;

  /**
   * \brief Get priority of bundle
   *
   * \return priority of bundle
   */
  uint8_t Priority () const;  

  /**
   * \brief Does the bundle node need to request reporting of bundle acceptance?
   */
  bool CustAcceptReport () const;

  /**
   * \return the lengh of bundle block
   */
  uint32_t GetBlockLength () const;

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
   * \return the custodian endpoint id
   */
  BpEndpointId GetCustEid () const;

  /**
   * process flags
   */
  typedef enum {
    REQ_BUNDLE_CUSTODY_XFER        = 1 << 3,
    BUNDLE_SINGLETON_DESTINATION   = 1 << 4,

    BULK                           = ((0 << 8) | (0 << 7)),
    NORMAL                         = ((0 << 8) | (1 << 7)),
    EXPEDITED                      = ((1 << 8) | (0 << 7)),
    UNUSED                         = ((1 << 8) | (1 << 7)),
    BUNDLE_PRIORITY                = (0x00EF)<<7,

    REQ_REPORT_CUSTODY_ACCEPT      = 1 << 15
  } ProcessingFlags6;  

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;

  uint32_t GetSerializedSize (void) const;
  uint32_t SerializeAndGetSize(Buffer::Iterator start, bool forReal) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);

private:
  // unique to primary bundle block, section 4.5.1, RFC 5050
  uint32_t m_blockLength;                 /// block length (byte)
  uint16_t m_dstSchemeOffset;             /// scheme offset of destination endpoint id
  uint16_t m_dstSspOffset;                /// ssp offset of destination endpoint id
  uint16_t m_srcSchemeOffset;             /// scheme offset of source endpoint id
  uint16_t m_srcSspOffset;                /// ssp offset of source endpoint id
  uint16_t m_reportSchemeOffset;          /// scheme offset of report endpoint id 
  uint16_t m_reportSspOffset;             /// ssp offset of report endpoint id
  uint16_t m_custSchemeOffset;            /// scheme offset of custodian endpoint id
  uint16_t m_custSspOffset;               /// ssp offset of custodian endpoint id
  std::string m_dictionary;               /// dictionary
};


} // namespace ns3

#endif /* BP_HEADER_6_H */
