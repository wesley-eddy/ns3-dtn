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
 */
#ifndef BP_AGENT_7_H
#define BP_AGENT_7_H

#include "bp-agent.h"
#include "bp-bundle-7.h"

namespace ns3 {

/**
 * \brief Bundle protocol 7 agent
 *
 * The API for bpv7 bundle agent
 *
 */

class BpAgent7 : public BpAgent {
    public:

        BpAgent7 ();
        
        virtual ~BpAgent7 ();

        static TypeId GetTypeId (void);
    
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
        int Send (Ptr<Packet> p, const BpEndpointId &src, const BpEndpointId &dst, 
            const Time &lifetime = Seconds(0), bool custody = false, uint32_t priority = 0);


    private:

        /**
         * \param b the bundle to be delivered
         * 
         * NOTE: the bundle "b" should be dynamically casted to a Bundle7
         */
        void Deliver(Bundle* b);

        /**
         * \param b the bundle to be forwarded
         * 
         * NOTE: NOTE: the bundle "b" should be dynamically casted to a Bundle7
         */
        void Forward(Bundle* b);

        /**
         * \param b the bundle to be enqueued for delivery
         * 
         * NOTE: NOTE: the bundle "b" should be dynamically casted to a Bundle7
         */
        int EnqueueForDeliveryToApplication(Bundle* b);

        //pTODO implement custody transfer
}; 

} // namespace ns3

#endif /* BP_AGENT_7_H */