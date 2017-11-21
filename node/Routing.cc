//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <map>
#include <omnetpp.h>
#include "Packet_m.h"
#include "behaviors.h"


/**
 * Demonstrates static routing, utilizing the cTopology class.
 */
class Routing : public cSimpleModule
{
  private:
    int myAddress;
    Behavior myBehavior;

    typedef std::map<int,int> RoutingTable; // destaddr -> gateindex
    RoutingTable rtable;

    simsignal_t dropSignal;
    simsignal_t outputIfSignal;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Routing);


void Routing::initialize()
{
    myAddress = getParentModule()->par("address");
    myBehavior = (Behavior) getParentModule()->par("behavior").longValue();

    dropSignal = registerSignal("drop");
    outputIfSignal = registerSignal("outputIf");

    //
    // Brute force approach -- every node does topology discovery on its own,
    // and finds routes to all other nodes independently, at the beginning
    // of the simulation. This could be improved: (1) central routing database,
    // (2) on-demand route calculation
    //
    cTopology *topo = new cTopology("topo");

    std::vector<std::string> nedTypes;
    nedTypes.push_back(getParentModule()->getNedTypeName());
    topo->extractByNedTypeName(nedTypes);
    EV << "cTopology found " << topo->getNumNodes() << " nodes\n";

    cTopology::Node *thisNode = topo->getNodeFor(getParentModule());

    // find and store next hops
    for (int i=0; i<topo->getNumNodes(); i++)
    {
        if (topo->getNode(i)==thisNode) continue; // skip ourselves
        topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(i));

        if (thisNode->getNumPaths()==0) continue; // not connected

        cGate *parentModuleGate = thisNode->getPath(0)->getLocalGate();
        int gateIndex = parentModuleGate->getIndex();
        int address = topo->getNode(i)->getModule()->par("address");
        rtable[address] = gateIndex;
        EV << "  towards address " << address << " gateIndex is " << gateIndex << endl;
    }
    delete topo;
}

void Routing::handleMessage(cMessage *msg)
{
    //what is this node's myBehavior?
    Packet *pk = check_and_cast<Packet *>(msg);
    int destAddr = pk->getDestAddr();
    int srcAddr = pk->getSrcAddr();

    if (destAddr == myAddress)
    {
        EV << "local delivery of packet " << pk->getName() << endl;
        send(pk, "localOut");
        emit(outputIfSignal, -1); // -1: local
        return;
    }
    else
    {
        switch(myBehavior)
        {
            case RADIO:
            case CONVERTER:
                if(pk->getSrcAddr() == myAddress || myAddress != pk->getNextHop())
                {
                    delete pk;
                    return;
                }
            default:
                break;
        }
    }

    RoutingTable::iterator it = rtable.find(destAddr);
    if (it==rtable.end())
    {
        EV << "address " << destAddr << " unreachable, discarding packet " << pk->getName() << endl;
        emit(dropSignal, (long)pk->getByteLength());
        delete pk;
        return;
    }

    /*
      int outGateIndex = (*it).second;
      EV << "forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
      pk->setHopCount(pk->getHopCount()+1);
      emit(outputIfSignal, outGateIndex);

      send(pk, "out", outGateIndex);
      */
    int outGateIndex = (*it).second;

    switch(myBehavior)
    {
        case PEOPLE:
        case GRID:
            //TODO
            //pk->setNextHop()

            EV << "forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
            pk->setHopCount(pk->getHopCount()+1);
            emit(outputIfSignal, outGateIndex);

            send(pk, "out", outGateIndex);
            break;
        case CONVERTER:
            //TODO
            //pk->setNextHop()
            //get the neighbors
            //only broadcast to radios & converters

            EV << "forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
            pk->setHopCount(pk->getHopCount()+1);
            emit(outputIfSignal, outGateIndex);

            send(pk, "out", outGateIndex);
            break;
        case RADIO:
            //TODO
            //pk->setNextHop()

            EV << "forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
            pk->setHopCount(pk->getHopCount()+1);
            emit(outputIfSignal, outGateIndex);

            send(pk, "out", outGateIndex);
            break;
        default:
            EV << "UNKNOWN BEHAVIOR TYPE!" << endl;
            delete pk;
            return;
            //break;
    }
}

