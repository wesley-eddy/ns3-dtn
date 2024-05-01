/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 * Modified by Dizhi Zhou <dizhi.zhou@gmail.com>
 */

#ifndef BP_AGENT_CONTAINER_H
#define BP_AGENT_CONTAINER_H

#include <stdint.h>
#include <vector>
#include "ns3/bp-agent-6.h"

namespace ns3 {

/**
 * \brief holds a vector of ns3::BpAgent pointers.
 *
 * Typically ns-3 BpAgents are installed on nodes using an BpAgent
 * helper.  The helper Install method takes a NodeContainer which holds 
 * some number of Ptr<Node>.  For each of the Nodes in the NodeContainer
 * the helper will instantiate an application, install it in a node and
 * add a Ptr<BpAgent> to that application into a Container for use
 * by the caller.  This is that container used to hold the Ptr<BpAgent>
 * which are instantiated by the BpAgent helper.
 */
class BpAgentContainer
{
public:
  /**
   * Create an empty BpAgentContainer.
   */
  BpAgentContainer ();

  /**
   * Create an BpAgentContainer with exactly one BpAgent which has
   * been previously instantiated.  The single application is specified
   * by a smart pointer.
   *
   * \param bp application The Ptr<BpAgent> to add to the container.
   */
  BpAgentContainer (Ptr<BpAgent6> bpAgent);

  /**
   * Create an BpAgentContainer with exactly one BpAgent which has
   * been previously instantiated and assigned a name using the Object Name
   * Service.  This BpAgent is then specified by its assigned name. 
   *
   * \param name The name of the BpAgent Object to add to the container.
   */
  BpAgentContainer (std::string name);

  /**
   * iterator for BpAgent
   */
  typedef std::vector<Ptr<BpAgent6> >::const_iterator Iterator;

  /**
   * \brief Get an iterator which refers to the first BpAgent in the 
   * container.
   *
   * BpAgents can be retrieved from the container in two ways.  First,
   * directly by an index into the container, and second, using an iterator.
   * This method is used in the iterator method and is typically used in a 
   * for-loop to run through the BpAgents
   *
   * \code
   *   BpAgentContainer::Iterator i;
   *   for (i = container.Begin (); i != container.End (); ++i)
   *     {
   *       (*i)->method ();  // some BpAgent method
   *     }
   * \endcode
   *
   * \returns an iterator which refers to the first BpAgent in the container.
   */
  Iterator Begin (void) const;

  /**
   * \brief Get an iterator which indicates past-the-last BpAgent in the 
   * container.
   *
   * BpAgents can be retrieved from the container in two ways.  First,
   * directly by an index into the container, and second, using an iterator.
   * This method is used in the iterator method and is typically used in a 
   * for-loop to run through the BpAgents
   *
   * \code
   *   BpAgentContainer::Iterator i;
   *   for (i = container.Begin (); i != container.End (); ++i)
   *     {
   *       (*i)->method ();  // some BpAgent method
   *     }
   * \endcode
   *
   * \returns an iterator which indicates an ending condition for a loop.
   */
  Iterator End (void) const;

  /**
   * \brief Get the number of Ptr<BpAgent> stored in this container.
   *
   * BpAgents can be retrieved from the container in two ways.  First,
   * directly by an index into the container, and second, using an iterator.
   * This method is used in the direct method and is typically used to
   * define an ending condition in a for-loop that runs through the stored
   * BpAgents
   *
   * \code
   *   uint32_t nBpAgents = container.GetN ();
   *   for (i = container.Begin (); i < nBpAgents; ++i)
   *     {
   *       Ptr<BpAgent> p = container.Get (i)
   *       (*i)->method ();  // some BpAgent method
   *     }
   * \endcode
   *
   * \returns the number of Ptr<BpAgent> stored in this container.
   */
  uint32_t GetN (void) const;

  /**
   * \brief Get the Ptr<BpAgent> stored in this container at a given
   * index.
   *
   * BpAgents can be retrieved from the container in two ways.  First,
   * directly by an index into the container, and second, using an iterator.
   * This method is used in the direct method and is used to retrieve the
   * indexed Ptr<Appliation>.
   *
   * \code
   *   uint32_t nBpAgents = container.GetN ();
   *   for (uint32_t i = 0 i < nBpAgents; ++i)
   *     {
   *       Ptr<BpAgent> p = container.Get (i)
   *       i->method ();  // some BpAgent method
   *     }
   * \endcode
   *
   * \param i the index of the requested application pointer.
   * \returns the requested application pointer.
   */
  Ptr<BpAgent6> Get (uint32_t i) const;

  /**
   * \brief Append the contents of another BpAgent to the end of
   * this container.
   *
   * \param other The BpAgent to append.
   */
  void Add (BpAgentContainer other);

  /**
   * \brief Append a single Ptr<BpAgent> to this container.
   *
   * \param application The Ptr<BpAgent> to append.
   */
  void Add (Ptr<BpAgent6> application);

  /**
   * \brief Append to this container the single Ptr<BpAgent> referred to
   * via its object name service registered name.
   *
   * \param name The name of the BpAgent Object to add to the container.
   */
  void Add (std::string name);

  /**
   * \brief Arrange for all of the BpAgent in this container to Start()
   * at the Time given as a parameter.
   *
   * \param start The Time at which each of the BpAgent should start.
   */
  void Start (Time start);

  /**
   * \brief Arrange for all of the BpAgent in this container to Stop()
   * at the Time given as a parameter.
   *
   * \param stop The Time at which each of the BpAgent should stop.
   */
  void Stop (Time stop);

private:
  std::vector<Ptr<BpAgent6> > m_bpAgents; /// vector of bundle protocol agents
};

} // namespace ns3

#endif /* BP_AGENT_CONTAINER_H */
