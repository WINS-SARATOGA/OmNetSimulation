//
// This file is part of an OMNeT++/OMNEST simulation example.
//
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package builder;

//
// Dynamically sets up a network, with the topology coming from text files
//
simple NetBuilder
{
    parameters:
        @display("i=block/cogwheel_s");
        string nodesFile;
        string connectionsFile;
}
