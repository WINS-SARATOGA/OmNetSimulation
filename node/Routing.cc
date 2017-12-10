//
// This file is part of an OMNeT++/OMNEST simulation example.
//
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
using namespace omnetpp;
#include "behaviors.h"

/**
 * Demonstrates static routing, utilizing the cTopology class.
 */
class Routing : public cSimpleModule
{
  private:
    int myAddress;
    int myBehavior;

    typedef std::map<int,int> RoutingTable; // destaddr -> gateindex
    typedef std::map<int,cModule*> RoutingModule;
    typedef std::map<int,cModule*> NeighborTable;
    RoutingTable rtable;
    RoutingModule rtable_addresses;
    NeighborTable ntable;

    simsignal_t dropSignal;
    simsignal_t outputIfSignal;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Routing);

void Routing::initialize()
{
    myAddress = getParentModule()->par("address");
    myBehavior = (int) getParentModule()->par("behavior");

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

    for(int i = 0; i < thisNode->getNumOutLinks(); i++)
    {
        ntable[thisNode->getLinkOut(i)->getLocalGate()->getIndex()] = thisNode->getLinkOut(i)->getRemoteNode()->getModule();
    }

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
        rtable_addresses[address] = thisNode->getPath(0)->getRemoteNode()->getModule();
        EV << "  towards address " << address << " gateIndex is " << gateIndex << endl;
    }
    delete topo;
}

void Routing::handleMessage(cMessage *msg)
{
    //what is this node's myBehavior?
    Packet *pk = check_and_cast<Packet *>(msg);
    int destAddr = pk->getDestAddr();
    int nextHopBehavior;
    int numNeighbors = getParentModule()->gateSize("port");

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
                if(myAddress != pk->getNextHop())
                {
                    EV << "packet not meant for me, dropping..." << endl;
                    delete pk;
                    return;
                }
            default:
                break;
        }
    }

    RoutingTable::iterator it = rtable.find(destAddr);
    RoutingModule::iterator it2 = rtable_addresses.find(destAddr);
    if (it==rtable.end() || it2==rtable_addresses.end())
    {
        EV << "address " << destAddr << " unreachable, discarding packet " << pk->getName() << endl;
        emit(dropSignal, (long)pk->getByteLength());
        delete pk;
        return;
    }

    int outGateIndex = (*it).second;

    pk->setHopCount(pk->getHopCount()+1);
    EV << "forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
    emit(outputIfSignal, outGateIndex);

    switch(myBehavior)
    {
        case PEOPLE:
        case GRID:
            pk->setNextHop((*it2).second->par("address"));
            send(pk, "out", outGateIndex);
            break;
        case CONVERTER:

            pk->setNextHop((*it2).second->par("address"));
            nextHopBehavior = (*it2).second->par("behavior");

            if(CONVERTER == nextHopBehavior || RADIO == nextHopBehavior)
            {
                for(int i = 0; i < numNeighbors; i++)
                {
                    int neighborBehavior = ntable[i]->par("behavior");
                    if(CONVERTER == neighborBehavior || RADIO == neighborBehavior)
                        send(pk->dup(), "out", i);
                }
            }
            else
            {
                emit(outputIfSignal,outGateIndex);
                send(pk, "out", outGateIndex);
                break;
            }


            break;
        case RADIO:
            EV << "num neighbors " << numNeighbors << endl;
            pk->setNextHop((*it2).second->par("address"));

            for(int i = 0; i < numNeighbors; i++)
            {
                send(pk->dup(), "out", i);
            }

            break;
        default:
            EV << "UNKNOWN BEHAVIOR TYPE!" << endl;
            delete pk;
            return;
            //break;
    }
}

