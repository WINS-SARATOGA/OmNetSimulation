//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//
// Copyright (C) 2017 Tyler Marshall and Sara Mousavi
//
// This file was originally an OMNet++ example. It was
// changed to our project at https://github.com/WINS-SARATOGA/rbn


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

