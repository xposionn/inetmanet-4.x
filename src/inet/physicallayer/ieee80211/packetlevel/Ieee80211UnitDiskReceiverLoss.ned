//
// Copyright (C) 2013 OpenSim Ltd
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

package inet.physicallayer.ieee80211.packetlevel;

//
// See also ~Ieee80211UnitDiskTransmitter, ~Ieee80211UnitDiskRadio, and ~UnitDiskRadioMedium.
//
module Ieee80211UnitDiskReceiverLoss extends Ieee80211UnitDiskReceiver
{
    parameters:
        double perUniLinks = default(0);
        double perLosLinks = default(0);
        double errorProb = default(0);
        bool  forceUni = default(true);
        @class(Ieee80211UnitDiskReceiverLoss);
}

