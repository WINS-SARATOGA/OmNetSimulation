//
// This a variation of an OMNeT++/OMNEST simulation example.
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//
// Copyright (C) 2017 Tyler Marshall and Sara Mousavi
//
// This file was originally the OMNet++ Net5 example. It was
// changed to provide better test cases for our project.

package networks;

import node.Node;
import ned.DatarateChannel;

//
// A nine-node network topology
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
        people[3]: Node {
            address = index;
        }
        grid[1]: Node {
            address = index + 3;
        }
        converter[2]: Node {
            address = index + 4;
        }
        radio[3]: Node {
            address = index + 6;
        }
    connections:
        people[0].port++ <--> C <--> people[1].port++;
        people[0].port++ <--> C <--> people[2].port++;
        
        people[1].port++ <--> C <--> people[2].port++;
        people[1].port++ <--> C <--> converter[0].port++;
        
        people[2].port++ <--> C <--> converter[0].port++;
        
        converter[0].port++ <--> C2 <--> radio[0].port++;
        converter[0].port++ <--> C2 <--> radio[1].port++;
        converter[0].port++ <--> C2 <--> radio[2].port++;
        
        radio[0].port++ <--> C2 <--> radio[2].port++;
        
        radio[1].port++ <--> C2 <--> radio[2].port++;
        
        radio[2].port++ <--> C2 <--> converter[1].port++;
        
        converter[1].port++ <--> C <--> grid[0].port++; 
}
