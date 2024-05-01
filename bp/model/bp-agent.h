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
 *          Pierce Forte <pierceforte12@gmail.com>
 *          Shawn McGraw <shawn.mcgraw@nasa.gov>
 *          Dizhi Zhou <dizhi.zhou@gmail.com>
 */
#ifndef BP_AGENT_H
#define BP_AGENT_H

#include "bp-cla.h"
#include "bp-endpoint-id.h"
#include "bp-flowstats.h"
#include "bp-bundle.h"
#include "bp-bundle-store.h"
#include "bp-admin-record.h"
#include "bp-custody-signal.h"
#include "bp-routing-agent.h"
#include "bp-static-routing-agent.h"
#include "sdnv.h"
#include "bp-header-6.h"
// #include "bp-block-header-6.h"
#include "acs-cteb.h"
#include "bp-flowstats.h"
#include "ns3/sequence-number.h"
#include "ns3/object.h"
#include "ns3/event-id.h"
#include "ns3/nstime.h"
#include "ns3/header.h"
#include "ns3/traced-callback.h"
#include "ns3/simulator.h"
#include <string>
#include <map>
#include <queue>
#include "ns3/log.h"

namespace ns3 {

/**
 * \brief the bundle protocol agent register information of a endpoint id
 */
struct BpRegisterInfo {
  BpRegisterInfo () 
    : lifetime (0),
      state (true)
    {
    }

  double lifetime;   /// the lifetime of a registration in seconds
  bool state;        /// the register state of registration
};

/**
 * \ingroup bpagent
 *
 * \brief The implementation of bundle protocol agent
 *
 * BpAgent class contains two main components of the bundle protocol:
 * BP APIs and bundle processing. The names and functions of BP APIs are based
 * on DTN2 (http://www.dtnrg.org/docs/code/DTN2/doc/manual/arch.html).
 * The bundle processing procedures are based on section 4, RFC 5050. In addition,
 * BpAgent class have two persistant storages for bundles and 
 * registrations.
 */
class BpAgent : public Object
{
public:

  static TypeId GetTypeId (void);
 
  BpAgent (void);
  virtual ~BpAgent (void);

  // BP APIs

  /**
   * \brief  Install BP components to a bundle node
   * 
   *  This method installs the BpAgent and BpCla classes 
   *  into a bundle node.
   *
   * \param node the pointer of bundle node
   */
  virtual void Open (Ptr<Node> node);

  /**
   * \brief Register a local endpoint id in the bundle protocol agent
   *
   *  This method adds an entry in the registration storage. If the state field 
   *  in the BpRegisterInfo is “true” (active state),  which means that this 
   *  application desires to receive bundles, it triggers the convergence layer 
   *  (CLA) protocol to enable a transport layer connection to receive packets 
   *  (e.g., listen state in TCP).
   *
   * \param eid the local endpoint id of the application
   * \param info the registration information
   *
   * \return This method returns -1 if there is a duplicated registration, or 
   * the CLA fails to enable the transport layer connection to receive packets. 
   * Otherwise, it returns 0.
   */
  virtual int Register (const BpEndpointId &eid, const struct BpRegisterInfo &info);

  /**
   * Set the registration with eid to passive state 
   *
   * This method triggers the convergence layer(CLA) to disable the transport
   * layer to receive packets.
   *
   * \param eid endpoint id to be registered
   *
   * \return This method returns -1 if the endpoing id is not registered, or 
   * the CLA fails to disable the transport layer connection. Otherwise, it 
   * returns 0.
   */
  virtual int Unregister (const BpEndpointId &eid); 


  /**
   * \brief Set the registration into active state
   *
   * This method sets the state field of the registration into "true" (active
   * state) and triggers the convergence layer protocol (CLA) to enable a transport 
   * layer connection to receive packets for this registration (e.g., listen state 
   * in TCP).
   *
   * \param eid the endpoint id of application
   *
   * \return This method returns -1 if it cannot find the registration for this
   * endpoint id,  or the CLA fails to enable the transport layer connection to 
   * receive packets. Otherwise, it returns 0.
   */
  virtual int Bind (const BpEndpointId &eid);

  /**
   * \brief Build the endpoint id by scheme and ssp
   *
   * \param scheme the scheme part of the endopoint id
   *
   * \param ssp the scheme-specific part of the endpoint id
   *
   * \return the endpoint id
   */
  virtual BpEndpointId BuildBpEndpointId (const std::string &scheme, const std::string &ssp);

  /**
   * Build the endpoint id by a uri
   *
   * \param uri the user defined uri 
   *
   * \return the endpoint id
   */
  virtual BpEndpointId BuildBpEndpointId (const std::string &uri);

  /**
   * Set a endpoint id
   *
   * \param eid endpoint id
   */
  virtual void SetBpEndpointId (BpEndpointId eid);

  /**
   * Send the packet p from the source bundle node
   *
   * This methods fragments the data from application layer into several bundles and 
   * stores the bundles into persistent bundle storage. The bundles are sent in a FIFO
   * order once the transport layer connection is available to send packets.
   *
   * \param p the bundle to be sent
   * \param src source endpoint id
   * \param dst destination endpoint id
   *
   * \return returns -1 if the source endpoint id is not registered. Otherwise, it 
   * returns 0.
   */
  virtual int Send (Ptr<Packet> p, const BpEndpointId &src, const BpEndpointId &dst, const Time &lifetime = Seconds(0), bool custody = false, uint32_t priority = 0) = 0;

  /**
   * Remove the registration with the eid and close the transport layer connection
   *
   * \param eid endpoint id to be removed
   */
  virtual int Close (const BpEndpointId &eid);  

  /**
   *  \brief Receive bundle with dst eid
   *
   *  This methods stores the received bundles in a persistent bundle storage. The 
   *  application can use this method to get the bundles in a FIFO order.
   *
   *  \param eid destination endpoint id
   *
   *  \return the bundle payload
   */
  virtual Ptr<Packet> Receive (const BpEndpointId &eid);

  // interfaces to convergence layer (CLA)

  /**
   * Get and delete a bundle from the persistant storage
   *
   * This method is called by BpCla to get the first stored bundle 
   * from the persistant bundle storage for the source endpoint id
   *
   * \param src the source endpoint id
   *
   * \return the bundle with the source endpoint id
   */
  virtual Ptr<Packet> GetBundle (const BpEndpointId &src);

  /**
   * Get node of this bundle protocol agent
   *
   * \return node
   */
  Ptr<Node> GetNode () const;

  /**
   * Get the endpoint id of this bundle protocol agent
   *
   * \return local endpoint id
   */
  BpEndpointId GetBpEndpointId () const;

  // routing operations
  
  /**
   * Set the routing agent 
   *
   * \param route routing agent
   */
  void SetRoutingAgent (Ptr<BpRoutingAgent> route);

  /**
   * Get the routing agent
   *
   * \return routing agent
   */
  Ptr<BpRoutingAgent> GetRoutingAgent ();

  /**
   * Set the register information
   *
   * \param info register information
   */
  void SetBpRegisterInfo (struct BpRegisterInfo info);

  /**
   * \brief Specify bundle protocol start time
   *
   * \param start Start time for this bundle protocol,
   *        relative to the current simulation time.
   */
  void SetStartTime (Time start);

  /**
   * \brief Specify bundle protocol agent stop time
   * \param stop Stop time for this bundle protocol agent, relative to the
   *        current simulation time.
   */
  void SetStopTime (Time stop);

  ssize_t GetStoredByteCount() { return m_bundleStore.GetStoredByteCount(); };
  ssize_t GetMaxBundlesStored() { return m_bundleStore.GetMaxBundlesStored(); };

  Ptr<BpCla> AddCla(std::string l4type);
  void AddCla(Ptr<BpCla> cla);
  void RemoveCla(Ptr<BpCla> cla);
  Ptr<BpCla> GetCla(size_t n);
  Ptr<BpCla> OutgoingCla(BpEndpointId dstEid);
  void ClaReady(Ptr<BpCla> cla);

  virtual void Forward(Bundle* b) = 0;

  uint64_t GetBytesDelivered() { return bytesDelivered; }
  uint64_t GetBundlesDelivered() { return bundlesDelivered; }
 
  void ProcessBundle (Ptr<Bundle> bundle);

  void SetCustodyTransferRTO(Time timeout) { ct_rto = timeout; }

  typedef void (* SentTracedCallback)(void *bundleP);
  typedef void (* UnicastForwadTracedCallback)(void *bundleP);
  typedef void (* LocalDeliverTracedCallback)(void *bundleP);
  typedef void (* DropTracedCallback)(void *bundleP, int reason);

protected:

  virtual void DoDispose (void);
  virtual void DoInitialize (void);

  /**
   * Retreive bundle from rx buffer
   */
  void RetreiveBundle ();

  /**
   * \brief Bundle protocol agent specific startup code
   *
   * The StartBpAgent method calls the Register () method 
   * to register the m_eid into the bundle protocol agent
   */
  void StartBpAgent ();

  /**
   * \brief Bundle protocol agent specific shutdown code
   *
   * The StopBpAgent method calls the Close () method to remove
   * registration and close the transport layer connection of this 
   * bundle protocol agent
   */
  void StopBpAgent ();

  void Dispatch(Ptr<Bundle> b);

  virtual void Deliver(Bundle* b) = 0;

  bool IsNodeMember(const BpEndpointId &eid);
  virtual int EnqueueForDeliveryToApplication(Bundle* b) = 0;

  InetSocketAddress GetEidAddress(const BpEndpointId &eid);

  Ptr<Node>           m_node;  /// bundle node
  std::deque<Ptr<BpCla>> m_clas;

  uint32_t m_bundleSize;       /// bundle size

  BundleStore m_bundleStore; // local bundle storage

  std::map<BpEndpointId, BpRegisterInfo> BpRegistration; /// persistant storage of registrations: map (local endpoint id, registration information)

  std::vector<Bundle> m_bundleRxBuffer; /// a buffer for all packets received from the CLA; bundles are retreived from this buffer

  SequenceNumber32 m_seq;         /// the bundle sequence number

  BpEndpointId m_eid;             /// unique id for endpoint id
  BpRegisterInfo m_bpRegInfo;     /// register information
  Ptr<BpRoutingAgent> m_bpRoutingAgent; /// bundle routing agent

  Time m_startTime;         /// The simulation time that the bundle protocol agent will start
  Time m_stopTime;          /// The simulation time that the bundle protocol agent will end
  EventId m_startEvent;     /// The event that will fire at m_startTime to start the bundle protocol agent
  EventId m_stopEvent;      /// The event that will fire at m_stopTime to end the bundle protocol agent

  uint64_t bundlesDelivered,
           bytesDelivered;   // Does not include any bundle headers.

  Time ct_rto; // Retransmission timer for custody transfer.

  TracedCallback<void*> m_sendOutgoingTrace;
  TracedCallback<void*> m_unicastForwardTrace;
  TracedCallback<void*> m_localDeliverTrace;
  TracedCallback<void*,int> m_dropTrace;
};

} // namespace ns3

#endif /* BP_AGENT_H */
