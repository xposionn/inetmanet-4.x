//
// Copyright (C) 2011 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
// @author Zoltan Bojthe
//

#include "inet/common/ResultFilters.h"

#include "inet/common/geometry/common/Coord.h"
#include "inet/common/NotifierConsts_m.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/applications/base/ApplicationPacket_m.h"

namespace inet {

namespace utils {

namespace filters {

Register_ResultFilter("messageAge", MessageAgeFilter);

void MessageAgeFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (auto msg = dynamic_cast<cMessage *>(object))
        fire(this, t, t - msg->getCreationTime(), details);
}

Register_ResultFilter("messageTSAge", MessageTSAgeFilter);

void MessageTSAgeFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (auto msg = dynamic_cast<cMessage *>(object))
        fire(this, t, t - msg->getTimestamp(), details);
}

Register_ResultFilter("appPkSeqNo", ApplicationPacketSequenceNumberFilter);

void ApplicationPacketSequenceNumberFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (auto msg = dynamic_cast<ApplicationPacket*>(object))
        fire(this, t, (long)msg->getSequenceNumber(), details);
}


Register_ResultFilter("mobilityPos", MobilityPosFilter);

void MobilityPosFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    IMobility *module = dynamic_cast<IMobility *>(object);
    if (module) {
        Coord coord = module->getCurrentPosition();
        fire(this, t, &coord, details);
    }
}

Register_ResultFilter("xCoord", XCoordFilter);

void XCoordFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (dynamic_cast<Coord *>(object))
        fire(this, t, ((Coord *)object)->x, details);
}

Register_ResultFilter("yCoord", YCoordFilter);

void YCoordFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (dynamic_cast<Coord *>(object))
        fire(this, t, ((Coord *)object)->y, details);
}

Register_ResultFilter("zCoord", ZCoordFilter);

void ZCoordFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (dynamic_cast<Coord *>(object))
        fire(this, t, ((Coord *)object)->z, details);
}

Register_ResultFilter("sourceAddr", MessageSourceAddrFilter);

void MessageSourceAddrFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (cMessage *msg = dynamic_cast<cMessage *>(object)) {
        L3AddressTagBase *addresses = msg->getTag<L3AddressReq>();
        if (!addresses)
            addresses = msg->getTag<L3AddressInd>();
        if (addresses != nullptr) {
            fire(this, t, addresses->getSrcAddress().str().c_str(), details);
        }
    }
}

Register_ResultFilter("throughput", ThroughputFilter);

void ThroughputFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (auto packet = dynamic_cast<cPacket *>(object)) {
        const simtime_t now = simTime();
        packets++;
        if (packets >= packetLimit) {
            bytes += packet->getByteLength();
            double throughput = 8 * bytes / (now - lastSignal).dbl();
            fire(this, now, throughput, details);
            lastSignal = now;
            bytes = 0;
            packets = 0;
        }
        else if (now - lastSignal >= interval) {
            double throughput = 8 * bytes / interval.dbl();
            fire(this, lastSignal + interval, throughput, details);
            lastSignal = lastSignal + interval;
            bytes = 0;
            packets = 0;
            if (emitIntermediateZeros) {
                while (now - lastSignal >= interval) {
                    fire(this, lastSignal + interval, 0.0, details);
                    lastSignal = lastSignal + interval;
                }
            }
            else {
                if (now - lastSignal >= interval) { // no packets arrived for a long period
                    // zero should have been signaled at the beginning of this packet (approximation)
                    fire(this, now - interval, 0.0, details);
                    lastSignal = now - interval;
                }
            }
            bytes += packet->getByteLength();
        }
        else
            bytes += packet->getByteLength();
    }
}

Register_ResultFilter("elapsedTime", ElapsedTimeFilter);

ElapsedTimeFilter::ElapsedTimeFilter()
{
    startTime = time(nullptr);
}

double ElapsedTimeFilter::getElapsedTime()
{
    long t = time(nullptr);
    return t - startTime;
}

void PacketDropReasonFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (check_and_cast<PacketDropDetails *>(details)->getReason() == reason)
        fire(this, t, object, details);
}

// TODO: replace these filters with a single filter that supports parameters when it becomes available in omnetpp
#define Register_PacketDropReason_ResultFilter(NAME, CLASS, REASON) class CLASS : public PacketDropReasonFilter { public: CLASS() { reason = REASON; } }; Register_ResultFilter(NAME, CLASS);
Register_PacketDropReason_ResultFilter("packetDropReasonIsUndefined", UndefinedPacketDropReasonFilter, -1);
Register_PacketDropReason_ResultFilter("packetDropReasonIsAddressResolutionFailed", AddressResolutionFailedPacketDropReasonFilter, ADDRESS_RESOLUTION_FAILED);
Register_PacketDropReason_ResultFilter("packetDropReasonIsForwardingDisabled", ForwardingDisabledPacketDropReasonFilter, FORWARDING_DISABLED);
Register_PacketDropReason_ResultFilter("packetDropReasonIsHopLimitReached", HopLimitReachedPacketDropReasonFilter, HOP_LIMIT_REACHED);
Register_PacketDropReason_ResultFilter("packetDropReasonIsIncorrectlyReceived", IncorrectlyReceivedPacketDropReasonFilter, INCORRECTLY_RECEIVED);
Register_PacketDropReason_ResultFilter("packetDropReasonIsInterfaceDown", InterfaceDownPacketDropReasonFilter, INTERFACE_DOWN);
Register_PacketDropReason_ResultFilter("packetDropReasonIsNoInterfaceFound", NoInterfaceFoundPacketDropReasonFilter, NO_INTERFACE_FOUND);
Register_PacketDropReason_ResultFilter("packetDropReasonIsNoRouteFound", NoRouteFoundPacketDropReasonFilter, NO_ROUTE_FOUND);
Register_PacketDropReason_ResultFilter("packetDropReasonIsNotAddressedToUs", NotAddressedToUsPacketDropReasonFilter, NOT_ADDRESSED_TO_US);
Register_PacketDropReason_ResultFilter("packetDropReasonIsQueueOverflow", QueueOverflowPacketDropReasonFilter, QUEUE_OVERFLOW);
Register_PacketDropReason_ResultFilter("packetDropReasonIsRetryLimitReached", RetryLimitReachedPacketDropReasonFilter, RETRY_LIMIT_REACHED);
Register_PacketDropReason_ResultFilter("packetDropReasonIsLifetimeExpired", LifetimeExpiredPacketDropReasonFilter, LIFETIME_EXPIRED);
Register_PacketDropReason_ResultFilter("packetDropReasonIsCongestion", CongestionPacketDropReasonFilter, CONGESTION);

} // namespace filters

} // namespace utils

} // namespace inet

