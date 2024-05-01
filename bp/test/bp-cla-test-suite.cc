/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 *  Copyright (c) 2013 University of New Brunswick
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
 * Authors: Carter T. Edmond <carter.t.edmond1@gmail.com>
 *         Dizhi Zhou <dizhi.zhou@gmail.com>
 *         
 */

#include <iostream>
#include <string>
#include <fstream>
#include <tgmath.h>
#include "ns3/bp-endpoint-id.h"
#include "ns3/bp-agent-7.h"
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/bp-endpoint-id.h"
#include "ns3/bp-static-routing-agent.h"
#include "ns3/bp-agent-helper.h"
#include "ns3/bp-agent-container.h"
#include "ns3/bp-bundle-7.h"
#include "ns3/bp-cla.h"
#include "ns3/test.h"

NS_LOG_COMPONENT_DEFINE ("BpClaTestSuite");

using namespace ns3;

/**
 * Class for a specific test case 
 * Can have multiple test cases within a test suit 
 */ 
class Bp7SerializeTestCase : public TestCase
{
public:
  Bp7SerializeTestCase (uint32_t payloadSize, bool fragment);
  virtual ~Bp7SerializeTestCase ();

private:
  /**
   * Method that every test case must implement from TestCase class.
   * Implements test case porcedure 
   */
  virtual void DoRun (void);

  // Example helper function used within test case procedure 
  void CompareBundles(Ptr<Bundle7> sentBundle, Ptr<Bundle7> recievedBundle);
   

protected:
  uint8_t p_size;
  uint32_t m_encodedBundleSize;
  uint32_t m_decodedBundleSize;
  bool isFragment;
};

/**
 * TestSuite class names the test and identifies the type of test
 * Enables specific test cases to run
 */
class BpClaTestSuite : public TestSuite
{
public:
  BpClaTestSuite ()
  : TestSuite ("bp-cla", UNIT)
    {
      NS_LOG_INFO ("creating BpClaTestSuite");
      AddTestCase(new Bp7SerializeTestCase(1, false), TestCase::QUICK);
      AddTestCase(new Bp7SerializeTestCase(0, false), TestCase::QUICK);
      AddTestCase(new Bp7SerializeTestCase(1, true), TestCase::QUICK);
      AddTestCase(new Bp7SerializeTestCase(0, true), TestCase::QUICK);
      AddTestCase(new Bp7SerializeTestCase(32, true), TestCase::QUICK);
      AddTestCase(new Bp7SerializeTestCase(1024, true), TestCase::QUICK);
    }
}g_bpClaTestSuite;

Bp7SerializeTestCase::Bp7SerializeTestCase (uint32_t payloadSize, bool fragment)
  : TestCase ("Test that bundles are correctly encoded and decoded according to BPv7 "),
    p_size (payloadSize),
    m_encodedBundleSize (0),
    isFragment(fragment)
{
}

Bp7SerializeTestCase::~Bp7SerializeTestCase ()
{
}

void
Bp7SerializeTestCase::DoRun (void)
{
  Ptr<Packet> adu = Create<Packet>(p_size);
  Ptr<Bundle7> sentBundle = Create<Bundle7>(adu);
  Ptr<Bundle7> recievedBundle;
  BpHeader7 *bph = sentBundle->GetPrimaryHeader();

  bph->SetIsFragment(false);
  bph->SetSourceEid(defaultEid);
  bph->SetDestinationEid(defaultEid);
  bph->SetReportEid(defaultEid);
  bph->SetCreateTimestamp(13);
  bph->SetSequenceNumber(SequenceNumber32(15));
  bph->SetLifeTime(Time("128ms"));
  bph->SetAduLength(adu->GetSize());
  bph->SetBlockLength(p_size);

  Ptr<Packet> serialized = BpCla::SerializeBundle(sentBundle); 
  m_encodedBundleSize = serialized->GetSize();
  recievedBundle = BpCla::DeserializeBundle(serialized);

  NS_TEST_EXPECT_MSG_EQ(sentBundle->m_adu->GetSize(), recievedBundle->m_adu->GetSize(), "Inconsistent adu");
  CompareBundles(sentBundle, recievedBundle);
}

void 
Bp7SerializeTestCase::CompareBundles (Ptr<Bundle7> sentBundle, Ptr<Bundle7> recBundle){
  BpHeader7 *sentBph = sentBundle->GetPrimaryHeader();
  BpBlockHeader7 *sentBpph = sentBundle->GetPayloadHeader();
  BpHeader7 *recBph = recBundle->GetPrimaryHeader();
  BpBlockHeader7 *recBpph = recBundle->GetPayloadHeader();

  // use '-v' option to print standard output to terminal 
  std::cout << "Recieved version: " <<(int)recBph->GetVersion() << std::endl;
  std::cout << "Recieved Time stamp: " <<(int)recBph->GetCreateTimestamp() << std::endl;
  std::cout << "Recieved Sequence Number: " <<(int)recBph->GetSequenceNumber().GetValue() << std::endl;
  std::cout << "Recieved Lifetime ms: " <<(int)recBph->GetLifeTime().GetMilliSeconds() << std::endl;

  // Test macros determine whether tests is considered Pass or Fail
  // A wide variety of test macros are available in src/core/test.h
  NS_TEST_EXPECT_MSG_EQ(sentBph->GetVersion(), recBph->GetVersion(), "Inconsistent Versions");
  NS_TEST_EXPECT_MSG_EQ(sentBph->GetCreateTimestamp(), recBph->GetCreateTimestamp(), "Inconsistent Time stamp");
  NS_TEST_EXPECT_MSG_EQ(sentBph->GetSequenceNumber(), recBph->GetSequenceNumber(), "Inconsistent sequence numbers");
  NS_TEST_EXPECT_MSG_EQ(sentBph->GetLifeTime(), recBph->GetLifeTime(), "Inconsistent Lifetime");
  NS_TEST_EXPECT_MSG_EQ(sentBph->IsFragment(), recBph->IsFragment(), "Inconsistent fragmentation");
  if(recBph->IsFragment()){
    NS_TEST_EXPECT_MSG_EQ(sentBph->GetFragOffset(), recBph->GetFragOffset(), "Inconsistent fragment offset");
    NS_TEST_EXPECT_MSG_EQ(sentBph->GetAduLength(), recBph->GetAduLength(), "Inconsistent ADU length in PH");
  }

  // TODO check EID serilization after 

  NS_TEST_EXPECT_MSG_EQ(sentBpph->BlockNumber(), recBpph->BlockNumber(), "Inconsistent Payload block number");
  NS_TEST_EXPECT_MSG_EQ(sentBpph->BlockCrcType(), recBpph->BlockCrcType(), "Inconsistent Payload CRC type");
  NS_TEST_EXPECT_MSG_EQ(sentBpph->BlockCrc(), recBpph->BlockCrc(), "Inconsistent Payload CRC");

  // TODO Complete header comparison check after v7 headers functions are implemented
}