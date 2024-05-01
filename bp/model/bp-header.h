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
#ifndef BP_HEADER_H
#define BP_HEADER_H

#include <stdint.h>
#include <string>
#include "ns3/header.h"
#include "ns3/nstime.h"
#include "ns3/buffer.h"
#include "ns3/sequence-number.h"
#include "bp-endpoint-id.h"

namespace ns3 {

/**
 * \brief Bundle protocol header
 *
 * Abstract primary bundle header class, containing common formatting among bpv6 and bpv7
 * 
 */
class BpHeader : public Header
{
public:
  BpHeader (uint8_t version, bool useCbhe = false);
  
  virtual ~BpHeader ();

  // Setters

  // Configure bundle processing flags
  
  /**
   * \brief Bundle is/isn't a fragment
   */
  virtual void SetIsFragment (const bool value);

  /**
   * \brief Application data unit is/isn't an administrative record
   */
  virtual void SetIsAdmin (const bool value);

  /**
   * \brief Bundle mustn't/must be fragmented
   */
  virtual void SetDonotFragment (const bool value);

  /**
   * \brief Acknowledgement by application is/isn't requested
   */
  virtual void SetAckbyAppReq (const bool value);

  /**
   * \brief Do or don't request report of bundle reception
   */
  virtual void SetReceptionReport (const bool value);

  /**
   * \brief Do or don't request report of bundle forwarding
   */
  virtual void SetForwardReport (const bool value);

  /**
   * \brief Do or don't request report of bundle delivery
   */
  virtual void SetDeliveryReport (const bool value);

  /**
   * \brief Do or don't request report of bundle deletion
   */
  virtual void SetDeletionReport (const bool value);

  /**
   * \brief set the version of bundle protocol
   *
   * \param ver the version of bundle protocol
   */
  virtual void SetVersion (uint8_t ver);

  /**
   * \brief set the destination endpoint id and update the length
   *
   * \param dst the destination endpoint id
   */
  virtual void SetDestinationEid (const BpEndpointId &dst) = 0;

  /**
   * \brief set the source endpoint id and update the length
   *
   * \param src the source endpoint id
   */
  virtual void SetSourceEid (const BpEndpointId &src) = 0;

  /**
   * \brief set the report endpoint id and update the length
   *
   * \param report the report endpoint id
   */
  virtual void SetReportEid (const BpEndpointId &report) = 0;

  /**
   * \brief set timestamp the creation timestamp time
   *
   * \param timestamp the creation timestamp time
   */
  virtual void SetCreateTimestamp (const uint32_t timestamp);

  /**
   * \brief set the sequence number
   *
   * \param sequenceNumber the sequence number
   */
  virtual void SetSequenceNumber (const SequenceNumber32 &sequenceNumber);

  /**
   * \brief set the lifetime of bundle
   *
   * \param lifetime the lifetime of bundle
   */
  virtual void SetLifeTime (Time lifetime);

  /**
   * \brief set the offset of fragment
   *
   * \param offset the offset of fragment
   */
  virtual void SetFragOffset (uint32_t offset);

  /**
   * \brief set the total length of application data unit
   *
   * \param len the total length of application data unit
   */
  virtual void SetAduLength (uint32_t len);


  // Getters

  // Get bundle processing flags

  /**
   * \brief Get priority of bundle
   *
   * \return priority of bundle
   */
  virtual uint8_t Priority () const = 0; 

  /**
   * \brief Is bundle a fragment?
   */
  virtual bool IsFragment () const;

  /**
   * \brief Is application data unit an administrative data?
   */
  virtual bool IsAdmin () const;

  /**
   * \brief Must bundle not be fragmented?
   */
  virtual bool DonotFragment () const;

  /**
   * \brief Is acknowledgement by application requested
   */
  virtual bool AckbyAppReq () const;

  /**
   * \brief Does the bundle node need to request reporting of bundle reception?
   */
  virtual bool ReceptionReport () const;

  /**
   * \brief Does the bundle node need to request reporting of bundle forwarding?
   */
  virtual bool ForwardReport () const;

  /**
   * \brief Does the bundle node need to request reporting of bundle delivery?
   */
  virtual bool DeliveryReport () const; 

  /**
   * \brief Does the bundle node need to request reporting of bundle deletion?
   */
  virtual bool DeletionReport () const;

  /**
   * \return the version of bundle protocol
   */
  virtual uint8_t GetVersion () const;

  /**
   * \return the creation timestamp time
   */
  virtual uint32_t GetCreateTimestamp () const;

  /**
   * \return the creation timestamp sequence number  
   */
  virtual SequenceNumber32 GetSequenceNumber () const;

  /**
   * \return the destination endpoint id
   */
  virtual BpEndpointId GetDestinationEid () const = 0;

  /**
   * \return the source endpoint id
   */
  virtual BpEndpointId GetSourceEid () const = 0;

  /**
   * \return the report endpoint id
   */
  virtual BpEndpointId GetReportEid () const = 0;

  /**
   * \return the lifetime of bundle
   */
  virtual Time GetLifeTime () const;

  /**
   * \return the offset of fragment
   */
  virtual uint32_t GetFragOffset () const;

  /**
   * \return the total length of application data unit
   */
  virtual uint32_t GetAduLength () const;

  //pTODO add comments describing each
  virtual uint32_t GetSerializedSize (void) const = 0;
  virtual uint32_t SerializeAndGetSize(Buffer::Iterator start, bool forReal) const = 0;
  virtual void Serialize (Buffer::Iterator start) const = 0;
  virtual uint32_t Deserialize (Buffer::Iterator start) = 0;

  /**
   * process flags
   */
  typedef enum {
    BUNDLE_IS_FRAGMENT             = 1 << 0,
    BUNDLE_IS_ADMIN                = 1 << 1,
    BUNDLE_DO_NOT_FRAGMENT         = 1 << 2,
    BUNDLE_ACK_BY_APP              = 1 << 5,
    REQ_REPORT_BUNDLE_RECEPTION    = 1 << 14,
    REQ_REPORT_BUNDLE_FORWARD      = 1 << 16,
    REQ_REPORT_BUNDLE_DELIVERY     = 1 << 17,
    REQ_REPORT_BUNDLE_DELETION     = 1 << 18,
  } ProcessingFlags;  


protected:
  // primary bundle block fields common among bpv6 and bpv7
  uint8_t m_version;                      /// the version of bundle protocol,
  uint32_t m_processingFlags;             /// bundle processing control flags, section 4.2 of RFC 5050
  uint32_t m_createTimestamp;             /// creation time //pTODO create struct w/ timestamp and sequence # fields
  SequenceNumber32 m_timestampSeqNum;     /// sequence number
  Time m_lifeTime;                        /// default bundle lifetime
  uint32_t m_fragOffset;                  /// fragementation offset
  uint32_t m_aduLength;                   /// application data unit length
  bool m_cbhe;                            /// use CBHE compression //pTODO remove cbhe from here

  //pTODO remove w/ cbhe
  uint32_t IpnNode(std::string &ssp);
  uint32_t IpnService(std::string &ssp);
  std::string IpnSsp(uint32_t node, uint32_t service) const;
};


} // namespace ns3

#endif /* BP_HEADER_H */
