//
// This abstract class implements the base of a power supply unit.
//
//  Abstract methods to be implemented at concrete power supply units
//      - virtual double getConsumptionLoss();
//      - virtual double calculateConsumptionLoss(double instantConsumption);
//      - virtual double getNodeConsumption();
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2013-11-07
//

#ifndef AbstractPSU_H
#define AbstractPSU_H

#include "inet/icancloud/EnergySystem/EnergyMeter/EnergyMeterUnit/AbstractMeterUnit.h"
#include <omnetpp.h>
#include <sstream>
#include <vector>
#include <string>
#include <map>
using std::string;
using std::pair;
using std::vector;

#define  PSU_OFF    "0"
#define  PSU_ON     "1"
#define  DEBUG_ENERGY false

namespace inet {

namespace icancloud {



class AbstractMeterUnit;

class AbstractPSU : public omnetpp::cSimpleModule{

    string state;                           // Node state */
    double wattage;                         // PSU Output rated power */
    double scale;                           // Time scale to get new measures of energy from node components */
    double nodeEnergyConsumed;              // The amount of energy consumed by node
    double psuConsumptionLoss;              // This parameter accumulates the quantity of energy wasted by psu as heat.

    omnetpp::cMessage* alarm;                        // The message used by psu to get measureas each simtime + scale

    AbstractMeterUnit* memory;              // Pointer to memory meter
    AbstractMeterUnit* cpu;                 // Pointer to cpu meter
    AbstractMeterUnit* network;             // Pointer to network meter
    AbstractMeterUnit* storage;             // Pointer to storage meter


protected:


	virtual ~AbstractPSU();

	/*
	*  Module initialization.
	*/
    virtual void initialize() override;
    

	/*
	 * The method only receives the messages generated by itself. When it is activated
	 * the method get the node consumption and add the amount of energy to the node energy consumed.
	 */
	void handleMessage(cMessage *msg) override;


public:

    /*
    *  Module finalization
    */
    virtual void finish() override;

    /*
    *  This method returns the value of the instant power loss by psu
    */
    virtual double getConsumptionLoss() = 0;

    /*
    * This method returns the value of the instant consumption depending on the instant consumption of node components
    */
    virtual double calculateConsumptionLoss(double instantConsumption) = 0;

    /*
    *  This method returns the value of the instant power consumed by all components of the node and the
    *  consumption loss by psu
    */
    virtual double getNodeConsumption() = 0;

    /*
    *  This method returns the total amount of energy loss by psu
    */
    virtual double getEnergyLoss(){return psuConsumptionLoss;};

    /*
    *  This method returns the instant component of the cpu + nic+ hdd + storage
    */
    double getNodeSubsystemsConsumption();

    /*
    * This method returns the consumption recorded from cpu + nic+ hdd + storage + energy loss by psu.
    */
    double getNodeEnergyConsumed(){return  (getIntervalEnergyConsumed() + (psuConsumptionLoss * scale));};

    /*
    * This method initializes the veriable nodeEnergyConsumed with 0.0 value.
    */
    void resetNodeEnergyConsumed ();

    /*
    * This method will be invoked by the subsystems of the node to "plug" them with the psu
    */
    void registerComponent (AbstractMeterUnit* element);

    /*
    * This method activates an alarm for getting the consumption each 'scale' interval
    * The consumption will be accumulated at nodeEnergyConsumed
    */
    void switchOn ();

    /*
    * This method deactivates the alarm.
    */
    void switchOff ();

private:

   double getIntervalEnergyConsumed();
   double getIntervalEnergyCalculus(string memoryState, vector<string> cpuState, string networkState, vector<string> diskState);
   double getInstantNodeConsumption(string memoryState, vector<string> cpuState, string networkState, vector<string> diskState);

};

} // namespace icancloud
} // namespace inet

#endif