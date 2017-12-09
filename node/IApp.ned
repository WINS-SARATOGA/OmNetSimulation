//
// This file is part of an OMNeT++/OMNEST simulation example.
//
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package node;

//
// Interface for network traffic generator modules that
// can be used in Node.
//
moduleinterface IApp
{
    parameters:
        int address;  // local node address
    gates:
        input in;
        output out;
}


