#include "bp-flowstats.h"

#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("BpFlowstats");

namespace ns3 {

class BpFlowProbeTag : public Tag
{
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer buf) const;
  virtual void Deserialize (TagBuffer buf);
  virtual void Print (std::ostream &os) const;
  BpFlowProbeTag ();
  BpFlowProbeTag (uint32_t flowId, uint32_t packetId, uint32_t packetSize, BpEndpointId src, BpEndpointId dst);
  void SetFlowId (uint32_t flowId);
  void SetPacketId (uint32_t packetId);
  void SetPacketSize (uint32_t packetSize);
  uint32_t GetFlowId (void) const;
  uint32_t GetPacketId (void) const;
  uint32_t GetPacketSize (void) const;
private:
  uint32_t m_flowId;
  uint32_t m_packetId;
  uint32_t m_packetSize;
  BpEndpointId m_src;
  BpEndpointId m_dst;
};

TypeId
BpFlowProbeTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpFlowProbeTag")
    .SetParent<Tag> ()
    .SetGroupName ("FlowMonitor")
    .AddConstructor<BpFlowProbeTag> ()
  ;
  return tid;
}
TypeId
BpFlowProbeTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t
BpFlowProbeTag::GetSerializedSize (void) const
{
  return 4 + 4 + 4 + 4 + m_src.Uri().size() + 4 + m_dst.Uri().size();
}
void
BpFlowProbeTag::Serialize (TagBuffer buf) const
{
  buf.WriteU32 (m_flowId);
  buf.WriteU32 (m_packetId);
  buf.WriteU32 (m_packetSize);

  std::string src = m_src.Uri(), dst = m_dst.Uri();

  buf.WriteU32(src.size());
  buf.Write((const uint8_t *)src.c_str(), src.size());
  buf.WriteU32(dst.size());
  buf.Write((const uint8_t *)dst.c_str(), dst.size());
}
void
BpFlowProbeTag::Deserialize (TagBuffer buf)
{
  m_flowId = buf.ReadU32 ();
  m_packetId = buf.ReadU32 ();
  m_packetSize = buf.ReadU32 ();

  uint32_t sz = buf.ReadU32 ();
  uint8_t strbuf[1024] {0};
  buf.Read(strbuf, sz);
  std::string str((char*)strbuf);
  m_src = BpEndpointId(str);
  sz = buf.ReadU32 ();
  bzero(strbuf, 1024);
  buf.Read(strbuf, sz);
  std::string dstr((char*)strbuf);
  m_dst = BpEndpointId(dstr);
}
void
BpFlowProbeTag::Print (std::ostream &os) const
{
  os << "FlowId=" << m_flowId;
  os << " PacketId=" << m_packetId;
  os << " PacketSize=" << m_packetSize;
}
BpFlowProbeTag::BpFlowProbeTag() : Tag () {}

BpFlowProbeTag::BpFlowProbeTag (uint32_t flowId, uint32_t packetId, uint32_t packetSize, BpEndpointId src, BpEndpointId dst) : Tag(), m_flowId (flowId), m_packetId (packetId), m_packetSize (packetSize), m_src (src), m_dst (dst) {}

void
BpFlowProbeTag::SetFlowId (uint32_t id)
{
  m_flowId = id;
}
void
BpFlowProbeTag::SetPacketId (uint32_t id)
{
  m_packetId = id;
}
void
BpFlowProbeTag::SetPacketSize (uint32_t size)
{
  m_packetSize = size;
}
uint32_t
BpFlowProbeTag::GetFlowId (void) const
{
  return m_flowId;
}
uint32_t
BpFlowProbeTag::GetPacketId (void) const
{
  return m_packetId;
}
uint32_t
BpFlowProbeTag::GetPacketSize (void) const
{
  return m_packetSize;
}


bool operator < (const BpFlowClassifier::BpFlowId &f1, const BpFlowClassifier::BpFlowId &f2)
{
  if (f1.srcEid < f2.srcEid) return true;
  if (f1.srcEid != f2.srcEid) return false;
  if (f1.dstEid < f2.dstEid) return true;
  return false;
}

bool operator == (const BpFlowClassifier::BpFlowId &f1, const BpFlowClassifier::BpFlowId &f2)
{
  return f1.srcEid == f2.srcEid && f1.dstEid == f2.dstEid;
}


BpFlowClassifier::BpFlowClassifier()
{
  NS_LOG_DEBUG("--Created BpFlowClassifier");
}

BpFlowClassifier::~BpFlowClassifier()
{
}

void
BpFlowClassifier::SerializeToXmlStream (std::ostream &os, uint16_t indent) const
{
  Indent (os, indent); os << "<BpFlowClassifier>\n";

  indent += 2;
  for (std::map<BpFlowId, FlowId>::const_iterator
       iter = m_flowMap.begin (); iter != m_flowMap.end (); iter++)
  {
    Indent (os, indent);
    os << "<Flow flowId=\"" << iter->second << "\""
       << " sourceEndpointId=\"" << iter->first.srcEid.Uri() << "\""
       << " destinationEndpointId=\"" << iter->first.dstEid.Uri() << "\">\n";
    Indent (os, indent);
    os << "</Flow>\n";
  }

  indent -= 2;
  Indent (os, indent); os << "</BpFlowClassifier>\n";
}

bool
BpFlowClassifier::Classify (const Ptr<Bundle> bundle, uint32_t *out_flowId, uint32_t *out_packetId) {
  BpHeader *bph = bundle->GetPrimaryHeader();
  BpFlowId tuple;
  tuple.srcEid = bph->GetSourceEid();
  tuple.dstEid = bph->GetDestinationEid();
  std::pair<std::map<BpFlowId, FlowId>::iterator, bool> insert
    = m_flowMap.insert (std::pair<BpFlowId, FlowId> (tuple, 0));

  if (insert.second)
    {
      FlowId newFlowId = GetNewFlowId ();
      insert.first->second = newFlowId;
      m_flowPktIdMap[newFlowId] = 0;
    }
  else
    {
      m_flowPktIdMap[insert.first->second] ++;
    }

  *out_flowId = insert.first->second;
  *out_packetId = m_flowPktIdMap[*out_flowId];

  return true;
}

BpFlowstats::BpFlowstats(Ptr<FlowMonitor> flowmon)
{
  NS_LOG_DEBUG("--Creating BpFlowstats object");
  m_flowmon = flowmon;
  m_flowclassifier = Create<BpFlowClassifier> ();
  m_flowmon->AddFlowClassifier (m_flowclassifier);
}

void
BpFlowstats::Install(Ptr<Node> node, Callback<bool, std::string, const CallbackBase &> bpAgentTraceConnectWithoutContext) {
  NS_LOG_DEBUG("--Installing BpFlowstats probe");
  Ptr<BpFlowProbe> probe = Create<BpFlowProbe> (m_flowmon, DynamicCast<BpFlowClassifier> (m_flowclassifier), node, bpAgentTraceConnectWithoutContext);
}

BpFlowstats::~BpFlowstats() {}


BpFlowProbe::BpFlowProbe (Ptr<FlowMonitor> monitor, Ptr<BpFlowClassifier> classifier, Ptr<Node> node, 
  Callback<bool, std::string, const CallbackBase &> bpAgentTraceConnectWithoutContext)
  : FlowProbe (monitor), m_classifier (classifier)
{
  NS_LOG_DEBUG("--Creating BpFlowProbe");

  NS_LOG_DEBUG("--Connecting BpFlowProbe to Bundle Agent callbacks");
  if (!bpAgentTraceConnectWithoutContext ("SendOutgoing",
                                           MakeCallback (&BpFlowProbe::SendLogger, Ptr<BpFlowProbe> (this))))
    {
      NS_FATAL_ERROR ("send outgoing trace fail");
    }
  if (!bpAgentTraceConnectWithoutContext ("UnicastForward",
                                            MakeCallback (&BpFlowProbe::ForwardLogger, Ptr<BpFlowProbe> (this))))
    {
      NS_FATAL_ERROR ("unicast forward trace fail");
    }
  if (!bpAgentTraceConnectWithoutContext ("LocalDeliver",
                                            MakeCallback(&BpFlowProbe::DeliverLogger, Ptr<BpFlowProbe> (this))))
    {
      NS_FATAL_ERROR ("local deliver trace fail");
    }

  if (!bpAgentTraceConnectWithoutContext ("Drop",
                                            MakeCallback (&BpFlowProbe::DropLogger, Ptr<BpFlowProbe> (this))))
    {
      NS_FATAL_ERROR ("drop trace fail");
    }
}

BpFlowProbe::~BpFlowProbe() {}

TypeId
BpFlowProbe::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpFlowProbe")
    .SetParent<FlowProbe> ()
    .SetGroupName ("FlowMonitor");
  return tid;
}

void
BpFlowProbe::DoDispose ()
{
  m_classifier = 0;
  FlowProbe::DoDispose ();
}

void
BpFlowProbe::SendLogger (void *bundleP)
{
  NS_LOG_DEBUG("++SEND LOGGER");
  Ptr<Bundle> bundle = (Bundle*)bundleP;
  FlowId flowId;
  FlowPacketId packetId;

  BpFlowProbeTag fTag;
  bool found = bundle->m_adu->FindFirstMatchingByteTag (fTag);
  if (found) return;

  if (m_classifier->Classify(bundle, &flowId, &packetId))
    {
      NS_LOG_DEBUG("@@CLASSIFIED");
      uint32_t size = bundle->m_adu->GetSize ();
      m_flowMonitor->ReportFirstTx (this, flowId, packetId, size);

      BpHeader *bph = bundle->GetPrimaryHeader();
      BpFlowProbeTag fTag (flowId, packetId, size, bph->GetSourceEid(), bph->GetDestinationEid());
      bundle->m_adu->AddByteTag(fTag);
    }
}

void
BpFlowProbe::ForwardLogger(void *bundleP)
{
  NS_LOG_DEBUG("++FORWARD LOGGER");
  Ptr<Bundle> bundle = (Bundle*)bundleP;
  BpFlowProbeTag fTag;
  bool found = bundle->m_adu->FindFirstMatchingByteTag (fTag);
  if (!found) return;

  FlowId flowId = fTag.GetFlowId ();
  FlowPacketId packetId = fTag.GetPacketId ();
  uint32_t size = bundle->m_adu->GetSize ();
  m_flowMonitor->ReportForwarding (this, flowId, packetId, size);
}

void
BpFlowProbe::DeliverLogger(void *bundleP)
{
  NS_LOG_DEBUG("++DELIVER LOGGER");
  Ptr<Bundle> bundle = (Bundle*)bundleP;
  BpFlowProbeTag fTag;
  bool found = bundle->m_adu->FindFirstMatchingByteTag (fTag);
  if (!found) return;

  FlowId flowId = fTag.GetFlowId ();
  FlowPacketId packetId = fTag.GetPacketId ();
  uint32_t size = bundle->m_adu->GetSize ();
  m_flowMonitor->ReportLastRx (this, flowId, packetId, size);
}

void
BpFlowProbe::DropLogger (void *bundleP, int reason)
{
  NS_LOG_DEBUG("++DROP LOGGER");
  Ptr<Bundle> bundle = (Bundle*)bundleP;
  BpFlowProbeTag fTag;
  bool found = bundle->m_adu->FindFirstMatchingByteTag (fTag);
  if (!found) return;

  uint32_t flowId = fTag.GetFlowId();
  uint32_t packetId = fTag.GetPacketId();
  uint32_t size = fTag.GetPacketSize();

  m_flowMonitor->ReportDrop (this, flowId, packetId, size, reason);
}

}
