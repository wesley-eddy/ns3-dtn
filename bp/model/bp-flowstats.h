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
 */
#ifndef BP_FLOWSTATS_H
#define BP_FLOWSTATS_H

#include <ostream>

#include "ns3/ptr.h"
#include "ns3/object-factory.h"
#include "ns3/node.h"
#include "ns3/node-container.h"

#include "bp-endpoint-id.h"
#include "ns3/bp-bundle.h"

#include "ns3/flow-classifier.h"
#include "ns3/flow-probe.h"
#include "ns3/flow-monitor.h"


namespace ns3 {

class BpFlowClassifier : public FlowClassifier
{
public:
  struct BpFlowId
  {
    BpEndpointId srcEid;
    BpEndpointId dstEid;
  };

  BpFlowClassifier ();

  ~BpFlowClassifier ();

  void SerializeToXmlStream (std::ostream &os, uint16_t indent) const;

  bool Classify (const Ptr<Bundle> bundle, uint32_t *out_flowId, uint32_t *out_packetId);

private:
  std::map<BpFlowId, FlowId> m_flowMap;
  std::map<FlowId, FlowPacketId> m_flowPktIdMap;
};

bool operator < (const BpFlowClassifier::BpFlowId &f1, const BpFlowClassifier::BpFlowId &f2);
bool operator == (const BpFlowClassifier::BpFlowId &f1, const BpFlowClassifier::BpFlowId &f2);

class BpFlowstats  {
public:
  BpFlowstats (Ptr<FlowMonitor> flowmon);
  ~BpFlowstats ();

  void Install (Ptr<Node> node, Callback<bool, std::string, const CallbackBase &> bpAgentTraceConnectWithoutContext);

private:
  Ptr<FlowMonitor> m_flowmon;
  Ptr<BpFlowClassifier> m_flowclassifier;
};

class BpFlowProbe : public FlowProbe {
public:
  BpFlowProbe (Ptr<FlowMonitor> monitor, Ptr<BpFlowClassifier> classifier, Ptr<Node> node, 
    Callback<bool, std::string, const CallbackBase &> bpAgentTraceConnectWithoutContext);

  virtual ~BpFlowProbe ();

  static TypeId GetTypeId(void);

  enum DropReason {
    DROP_EXPIRED,
    DROP_INVALID_REASON,
  };

protected:
  virtual void DoDispose (void);

private:
  void SendLogger (void *bundleP);
  void ForwardLogger (void* bundleP);
  void DeliverLogger (void* bundleP);
  void DropLogger (void* bundleP, int reason);

  Ptr<BpFlowClassifier> m_classifier;
};

} // namespace ns3

#endif // BP_FLOWSTATS_H

