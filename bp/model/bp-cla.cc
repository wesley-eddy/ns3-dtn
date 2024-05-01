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
 *          Carter T. Edmond <carter.t.edmond1@gmail.com>
 */

#include "bp-cla.h"
#include "bp-header-6.h"
#include "bp-block-header-6.h"
#include "codec.h"

NS_LOG_COMPONENT_DEFINE ("BpCla");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (BpCla);

TypeId 
BpCla::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpCla")
    .SetParent<Object> ()
  ;
  return tid;
}

BpCla::BpCla (Callback<void, Ptr<Bundle>> processBundleCallback)
: m_ready(false),
  m_cbhe(false),
  m_processBundleCallback(processBundleCallback)
{
  NS_LOG_FUNCTION (this);
}

BpCla::~BpCla ()
{
  NS_LOG_FUNCTION (this);
}

int 
BpCla::EnableSend (const BpEndpointId &src, const BpEndpointId &dst, InetSocketAddress dstAddress, Ptr<Node> bpNode)
{ 
  NS_LOG_FUNCTION (this << " " << src.Uri () << " " << dst.Uri ());

  InetSocketAddress defaultAddr ("127.0.0.1", 0);
  if (dstAddress == defaultAddr)
    {
      NS_LOG_DEBUG ("BpCla::EnableSend (): cannot find route for destination endpoint id " << dst.Uri ());
      return -1;
    }

  // create socket
  Ptr<Socket> socket = Socket::CreateSocket (bpNode, GetSocketTypeId());
  if (socket->Bind () < 0) {
    NS_LOG_DEBUG ("BpCla::EnableSend (): Bind");
    return -1;
  }
  if (socket->Connect (dstAddress) < 0) {
    NS_LOG_DEBUG ("BpCla::EnableSend (): Connect");
    return -1;
  }
  if (socket->ShutdownRecv () < 0) {
    NS_LOG_DEBUG ("BpCla::EnableSend (): ShutdownRecv");
    return -1;
  }

  socket->SetIpTos(m_tos);
  SetL4SocketCallbacks (socket);

  // store the sending socket so that the convergence layer can dispatch the bundles to different sockets
  std::map<BpEndpointId, Ptr<Socket> >::iterator it = m_l4SendSockets.end ();
  it = m_l4SendSockets.find (src);
  if (it == m_l4SendSockets.end ())
    m_l4SendSockets.insert (std::pair<BpEndpointId, Ptr<Socket> >(src, socket));  
  else
    return -1;

  return 0;
}

Ptr<Socket>
BpCla::GetL4Socket (Ptr<Packet> packet, InetSocketAddress dstAddress, Ptr<Node> bpNode)
{ 
  NS_LOG_FUNCTION (this << " " << packet);
  BpHeader6 bph (m_cbhe);
  packet->PeekHeader (bph);
  BpEndpointId dst = bph.GetDestinationEid ();
  BpEndpointId src = bph.GetSourceEid ();

  std::map<BpEndpointId, Ptr<Socket> >::iterator it = m_l4SendSockets.end ();
  it = m_l4SendSockets.find (src);
  if (it == m_l4SendSockets.end ())
    {
      // enable send out from the src endpoint id to the dst endpoint id
      if (EnableSend (src, dst, dstAddress, bpNode) < 0)
        return NULL;
    }

  // update because EnableSend () add new socket into m_l4SendSockets
  it = m_l4SendSockets.find (src);

  return ((*it).second);
}


int
BpCla::SendPacket (Ptr<Packet> packet, InetSocketAddress dstAddress, Ptr<Node> bpNode)
{ 
  NS_LOG_FUNCTION (this << " " << packet);
  Ptr<Socket> socket = GetL4Socket (packet, dstAddress, bpNode);
 
  if ( socket == NULL)
    return -1;

  socket->Send (packet);
  return 0;
}

int
BpCla::SendBundle (Ptr<Bundle6> bundle, InetSocketAddress dstAddress, Ptr<Node> bpNode)
{
  // pTODO cbhe encode here

  BpHeader6 *bph = bundle->GetPrimaryHeader();
  BpBlockHeader6 *bpph = bundle->GetPayloadHeader();
  uint32_t size = bph->GetBlockLength();

  NS_LOG_FUNCTION (this << " " << bundle << " size " << size);

  Ptr<Packet> p = bundle->m_adu->CreateFragment(bph->GetFragOffset(), size);  
  p->AddHeader(*bpph);
  NS_LOG_FUNCTION("size after payload header: " << p->GetSize());
  p->AddHeader(*bph);
  NS_LOG_FUNCTION("size after BP header: " << p->GetSize());

  NS_LOG_DEBUG("Send bundle" << " seq " << bph->GetSequenceNumber().GetValue() <<
               " src eid " << bph->GetSourceEid().Uri() <<
               " dst eid " << bph->GetDestinationEid().Uri() <<
               " size " << p->GetSize() << " bytes with " << size << " payload bytes");
  NS_LOG_DEBUG(" fragment: " << ((bph->IsFragment())?"yes":"no"));

  SendPacket(p, dstAddress, bpNode);
  return 0;
}

int
BpCla::DisableReceive (const BpEndpointId &local)
{ 
  NS_LOG_FUNCTION (this << " " << local.Uri ());
   std::map<BpEndpointId, Ptr<Socket> >::iterator it = m_l4RecvSockets.end ();
  it = m_l4RecvSockets.find (local);
  if (it == m_l4RecvSockets.end ())
    {
      return -1;
    }
  else
    {
      // close the conenction
      return ((*it).second)->Close ();
    }

  return 0;
}

void 
BpCla::NormalClose (Ptr<Socket> socket)
{ 
  NS_LOG_FUNCTION (this << " " << socket);
}

void 
BpCla::ErrorClose (Ptr<Socket> socket)
{ 
  NS_LOG_FUNCTION (this << " " << socket);
}

void 
BpCla::DataSent (Ptr<Socket> socket, uint32_t size)
{ 
  NS_LOG_FUNCTION (this << " " << socket << " " << size);
}

void 
BpCla::Sent (Ptr<Socket> socket, uint32_t size)
{ 
  NS_LOG_FUNCTION (this << " " << socket << " " << size);
}

void 
BpCla::DataRecv (Ptr<Socket> socket)
{ 
  // pTODO cbhe decode here

  NS_LOG_FUNCTION (this << " " << socket);
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
  {
    NS_LOG_DEBUG ("DataRecv size (before header removal) " << packet->GetSize());
    // In this CLA there is one bundle per packet.
    BpHeader6 *bpHeader = new BpHeader6(m_cbhe);
    BpBlockHeader6 *bppHeader = new BpBlockHeader6(BpBlockHeader6::BUNDLE_PAYLOAD_BLOCK);

    packet->RemoveHeader(*bpHeader);
    NS_LOG_DEBUG ("DataRecv size (after BP header removal) " << packet->GetSize());
    packet->RemoveHeader(*bppHeader);
    NS_LOG_DEBUG ("DataRecv size (after payload header removal) " << packet->GetSize());

    NS_LOG_DEBUG ("Recv bundle:" << " seq " << bpHeader->GetSequenceNumber().GetValue() <<
                  " src eid " << bpHeader->GetSourceEid().Uri() <<
                  " dst eid " << bpHeader->GetDestinationEid().Uri() <<
                  " packet size " << packet->GetSize ());

    // pTODO maybe use a template in the CLA class so instead of "Bundle6" we use "T"
    // then when we create a CLA we tell it to use either v6 or v7 bundles
    // note: may need to also specificy the Header and BlockHeader as v6 or v7 in template
    Ptr<Bundle6> b = Create<Bundle6>(packet);
    b->SetPrimaryHeader(bpHeader);
    b->SetPayloadHeader(bppHeader);

    NS_LOG_DEBUG(" fragment: " << ((bpHeader->IsFragment())?"yes":"no") << " offset " << bpHeader->GetFragOffset());

    // pTODO: maybe make callback to a process packet call in bp-agent (this would be an alternative to 
    // using a template here to build bundle from packet)
    m_processBundleCallback(b);
  }
}

void 
BpCla::SetReady(bool ready) {
  m_ready = ready; 
}

void
BpCla::UseCbhe() {
  m_cbhe = true;
}

bool
BpCla::UsesCbhe() {
  return m_cbhe;
}

Ptr<Packet>
BpCla::SerializeBundle(Ptr<Bundle7> bundle){

  BpHeader7 *bph = bundle->GetPrimaryHeader();
  BpBlockHeader7 *bpph = bundle->GetPayloadHeader();
  uint32_t size  = bph->GetBlockLength();
  uint32_t headerSize = 1 + bph->GetSerializedSize() + bpph->GetSerializedSize(); // Byte lenght of  headers + initial byte for encoding
  
  Buffer data(0);
  data.AddAtStart(headerSize);
  Buffer::Iterator i = data.Begin();

  i.WriteU8(0x9f);

  // Encode Headers
  bph->Serialize(i);
  i.Next(bph->GetSerializedSize());
  bpph->Serialize(i);
  i.Next(bpph->GetSerializedSize());

  std::vector<uint8_t> temp;

  // Encode Adu
  Ptr<Packet> p = bundle->m_adu->CreateFragment(bph->GetFragOffset(), size); 
  uint8_t buf[size];
  std::string adu;
  p->CopyData(&buf[0],size);
  for (auto elem : buf) adu.push_back(elem);
  size_t length = CborLite::encodeBytes(temp, adu);

  // Encode CRC
  if (bpph->BlockCrcType() != 0){
    std::string crcStr = std::to_string(bpph->BlockCrc());
    length += CborLite::encodeBytes(temp, crcStr);       
  }

  //Increase buffer size by byte lenght of (adu + block CRC + final byte for encoding)
  data.AddAtEnd(length+1); 
  i = data.Begin();
  i.Next(headerSize);

  for(uint32_t k = 0; k < temp.size (); k++)  i.WriteU8(temp.at(k));
  i.WriteU8(0xff);

  // Initialize Packet
  length = data.GetSize();
  uint8_t buffer[length];
  data.CopyData(&buffer[0], length);
  return Create<Packet>(&buffer[0], length);
}

Ptr<Bundle7>
BpCla::DeserializeBundle(Ptr<Packet> packet){

  BpHeader7 *bpHeader = new BpHeader7(false);  //TODO use bool var m_cbhe
  BpBlockHeader7 *bppHeader = new BpBlockHeader7(BpBlockHeader7::BUNDLE_PAYLOAD_BLOCK);  

  packet->RemoveAtStart(1);
  packet->RemoveAtEnd(1);
  packet->RemoveHeader(*bpHeader);
  packet->RemoveHeader(*bppHeader);

  // Copy adu and CRC for decoding
  size_t size = packet->GetSize();
  uint8_t buffer[size];
  packet->CopyData(buffer, size);
  std::vector<uint8_t> encodedContent;
  for (auto elem : buffer) encodedContent.push_back(elem);

  auto pos = begin(encodedContent);
  auto stop = end(encodedContent);
  std::string strAdu, strCrc;
  size_t len = CborLite::decodeBytes(pos, stop, strAdu);
  if(bppHeader->BlockCrc() != 0){
    len += CborLite::decodeBytes(pos, stop, strCrc);
    bppHeader->SetBlockCrc(stoi(strCrc, 0, 16));
  }

  // Bundle Reconstruction
  uint8_t aduBuf[strAdu.length()];
  for(uint32_t i = 0; i < strAdu.length(); i++) aduBuf[i] = strAdu.at(i);
  Ptr<Packet> p = Create<Packet>(aduBuf, strAdu.length());
  Ptr<Bundle7> b = Create<Bundle7>(p);
  b->SetPrimaryHeader(bpHeader);
  b->SetPayloadHeader(bppHeader);
  
  return b;
}

} // namespace ns3

