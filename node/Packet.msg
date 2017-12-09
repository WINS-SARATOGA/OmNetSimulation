//
// This file is part of an OMNeT++/OMNEST simulation example.
//
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Represents a packet in the network.
//
packet Packet
{
    int srcAddr;
    int destAddr;
    int hopCount;
    int nextHop;
}

