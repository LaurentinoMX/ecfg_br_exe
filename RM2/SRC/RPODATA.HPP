#ifndef _RPODATA_H_
#define _RPODATA_H_

#include <iostream.h>
#include <fstream.h>

#include <istring.hpp>
#include <iksset.h>

#include "rpo2l.hpp"

typedef enum { Leasing = 1, PurchaseMaintenance = 2, PurchasePerCall = 3 } MachineStatus;

class RPOForwardingData {
  public:
    IString       userName;
    IString       tieLine;
    IString       vmUserId;
    IString       vmNodeId;
    IString       specialContractNum;
    Boolean       federalCustomer;
    Boolean       educationAllowance;
    Boolean       customerOwning;
    MachineStatus machineStatus;
};

class RPOFeature {
  public:
    IString        featureCode;
    unsigned int   qty;

    RPOFeature() : featureCode(), qty(0) {};
    RPOFeature(IString const & aFeatureCode,
                unsigned int   aQty) :
      featureCode(aFeatureCode), qty(aQty) {};
};

inline IString const & key (RPOFeature const & elem) { return elem.featureCode; }

typedef IKeySortedSet<RPOFeature,IString> RPOFeatureSet;

class RPOMachine {
  public:
    IString        machineType;
    IString        machineModel;
    IString        serialNum;
    IString        key;
    RPOFeatureSet  addFeatures;
    RPOFeatureSet  deleteFeatures;

    RPOMachine() {};
    RPOMachine(RPO2 const & rpo2Struct);
};

inline IString const & key (RPOMachine const & elem)
{
  return (((RPOMachine &)elem).key = elem.machineType +
                                     elem.machineModel +
                                     elem.serialNum);
}

typedef IKeySortedSet<RPOMachine,IString> RPOMachineSet;

Boolean RPO2ListToRPOMachineSet(RPOMachineSet &, RPO2List &);

#define RPO_BUILD_DATAFORM_OK                0
#define RPO_BUILD_DATAFORM_FILE_ERROR        1
#define RPO_BUILD_DATAFORM_LEFTOVER_FEATURES 2

int BuildRPODataform(ofstream & file,
                     int stemNum,
                     RPOMachine & rpoMachine,
                     RPOForwardingData & forwardData);

#define RPO_UPLOAD_OK           '0'
#define RPO_UPLOAD_HLLAPI_ERROR '1'
#define RPO_UPLOAD_FILE_ERROR   '2'

IString uploadRPOs(RPO2List & rpoList,
               RPOForwardingData & forwardData);

#endif
