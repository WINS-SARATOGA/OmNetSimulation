//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package networks;

import node.Node;
import ned.DatarateChannel;

//
// A ten-node network topology
//
network Net9
{
    types:
        channel C extends DatarateChannel //people and grid
        {
            delay = 1ms;//uniform(0.1ms, 1ms);
            datarate = 1Mbps;
        }
        channel C2 extends DatarateChannel //radios and converters
        {
            delay = 1.5ms;
            datarate = 0.5Mbps;
        }
    submodules:
        rte[9]: Node {
            address = index;
        }
    connections:
        rte[0].port++ <--> C <--> rte[1].port++;
        rte[0].port++ <--> C <--> rte[2].port++;
        
        rte[1].port++ <--> C <--> rte[2].port++;
        rte[1].port++ <--> C <--> rte[3].port++;
        
        rte[2].port++ <--> C <--> rte[3].port++;
        
        rte[3].port++ <--> C2 <--> rte[4].port++;
        rte[3].port++ <--> C2 <--> rte[5].port++;
        rte[3].port++ <--> C2 <--> rte[6].port++;
        
        rte[4].port++ <--> C2 <--> rte[7].port++;
        
        rte[5].port++ <--> C2 <--> rte[7].port++;
        
        rte[6].port++ <--> C2 <--> rte[7].port++;
        
        rte[7].port++ <--> C <--> rte[8].port++;
}
