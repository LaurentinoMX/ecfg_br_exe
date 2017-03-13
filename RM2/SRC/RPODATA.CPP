#include <fstream.h>
#include <idate.hpp>
#include <itime.hpp>
#include <iprofile.hpp>
#define  INCL_DOSERRORS
#define  INCL_DOSNLS
#define  INCL_DOSPROCESS
#include <os2.h>

#include "prf.hpp" // RM2 profile
#include "rpodata.hpp"

// 02-27-1995 Changed "TESTING" to "RM2" for identification in RPO Processing [DJL]
// 02-22-1996 Al Appended the User Tie Line to Line 1 of the RPO Upload File
// 02-22-1996 Al Appended a hard-coded 'A' Channel Indicator to Line 2 of the
//               RPO Upload File

RPOMachine :: RPOMachine (RPO2 const & rpo2) :
  machineType(rpo2.machineType()),
  machineModel(rpo2.machineModel()),
  serialNum(rpo2.serialNum()),
  key(machineType+machineModel+serialNum),
  addFeatures(),
  deleteFeatures()
{
  RPOFeature rpoFeature(rpo2.featureCode(),rpo2.qty());
  switch(rpo2.action())
    {
    case RPO_CODE_ADD: addFeatures.add(rpoFeature); break;
    case RPO_CODE_DELETE: deleteFeatures.add(rpoFeature); break;
    default: break;
    }
}

Boolean RPO2ListToRPOMachineSet(RPOMachineSet & set, RPO2List & list)
{
  RPO2List::Cursor listCur(list);
  /* for each element in the RPO2List, process and put in the           */
  /* RPOMachineSet                                                      */
  for (list.setToFirst(listCur);listCur.isValid();list.setToNext(listCur))
    {
    RPO2 rpo2 = list.elementAt(listCur);
    RPOMachineSet::Cursor setCur(set);
    RPOMachine rpoMachine;
    RPOFeature rpoFeature;
    IString tmpKey = rpo2.machineType()+rpo2.machineModel()+rpo2.serialNum();
    if (set.locateElementWithKey(tmpKey,setCur))
      /* found element in RPOMachineSet with same key, use that element */
      /* instead of adding a new one.                                   */
      {
      rpoMachine = set.elementAt(setCur);
      RPOFeatureSet::Cursor addFeatCur(rpoMachine.addFeatures);
      RPOFeatureSet::Cursor delFeatCur(rpoMachine.deleteFeatures);
      if (rpoMachine.addFeatures.locateElementWithKey(rpo2.featureCode(),addFeatCur))
        /* the feature code is already in addFeatures */
        {
        switch(rpo2.action())
          {
          case RPO_CODE_ADD:
            rpoMachine.addFeatures.elementAt(addFeatCur).qty += rpo2.qty();
            break;
          case RPO_CODE_DELETE:
            if (rpoMachine.addFeatures.elementAt(addFeatCur).qty > rpo2.qty())
              rpoMachine.addFeatures.elementAt(addFeatCur).qty -= rpo2.qty();
            else if (rpoMachine.addFeatures.elementAt(addFeatCur).qty == rpo2.qty())
              rpoMachine.addFeatures.removeAt(addFeatCur);
            else
              {
              unsigned tempQty = rpo2.qty() - rpoMachine.addFeatures.elementAt(addFeatCur).qty;
              rpoMachine.addFeatures.removeAt(addFeatCur);
              rpoFeature = RPOFeature(rpo2.featureCode(),tempQty);
              rpoMachine.deleteFeatures.add(rpoFeature);
              }
          default: break;
          }
        }
      else if (rpoMachine.deleteFeatures.locateElementWithKey(rpo2.featureCode(),delFeatCur))
        /* the feature code is already in deleteFeatures */
        {
        switch(rpo2.action())
          {
          case RPO_CODE_DELETE:
            rpoMachine.deleteFeatures.elementAt(delFeatCur).qty += rpo2.qty();
            break;
          case RPO_CODE_ADD:
            if (rpoMachine.deleteFeatures.elementAt(delFeatCur).qty > rpo2.qty())
              rpoMachine.deleteFeatures.elementAt(delFeatCur).qty -= rpo2.qty();
            else if (rpoMachine.deleteFeatures.elementAt(delFeatCur).qty == rpo2.qty())
              rpoMachine.deleteFeatures.removeAt(delFeatCur);
            else
              {
              unsigned tempQty = rpo2.qty() - rpoMachine.deleteFeatures.elementAt(delFeatCur).qty;
              rpoMachine.deleteFeatures.removeAt(delFeatCur);
              rpoFeature = RPOFeature(rpo2.featureCode(),tempQty);
              rpoMachine.addFeatures.add(rpoFeature);
              }
          default: break;
          }
        }
      else
        {
        rpoFeature = RPOFeature(rpo2.featureCode(),rpo2.qty());
        switch(rpo2.action())
          {
          case RPO_CODE_ADD: rpoMachine.addFeatures.add(rpoFeature); break;
          case RPO_CODE_DELETE: rpoMachine.deleteFeatures.add(rpoFeature); break;
          default: break;
          }
        }
      set.replaceAt(setCur,rpoMachine);
      }
    else
      {
      rpoMachine = RPOMachine(rpo2);
      set.add(rpoMachine);
      }
    }
return True;
}

int BuildRPODataform(ofstream & out,
                     int stemNum,
                     RPOMachine & rpoMachine,
                     RPOForwardingData & forwardData)
{
  if (out.fail())
    return RPO_BUILD_DATAFORM_FILE_ERROR;

  // line 0
  out << "File" << stemNum << ".0 = 9" << endl;

  // line 1
// 02-22-1996 Al Appended the User Tie Line to Line 1 of the RPO Upload File
  out << "File" << stemNum << ".1 = '";
  out << "B NBHWMES ";
  IDate theDate;
  out << theDate.asString("%m/%d/%y") << ' ';
  ITime theTime;
  out << theTime.asString("%H:%M:%S") << ' ';
  out << forwardData.vmUserId.subString(1,8) << ' ';
  out << forwardData.vmNodeId.subString(1,8) << ' ';
  out << forwardData.userName;
  out << ' ' << forwardData.tieLine;
  out << "'" << endl;

  // line 2
// 02-22-1996 Al Appended a hard-coded 'A' Channel Indicator to Line 2 of the
//               RPO Upload File
  out << "File" << stemNum << ".2 = '";
  if (rpoMachine.addFeatures.numberOfElements() > 0)
    out << '1';
  else
    out << '0';
  out << ' ';
  if (rpoMachine.deleteFeatures.numberOfElements() > 0)
    out << '1';
  else
    out << '0';
  out << " 0 0 0";
  out << " A";
  out << "'" << endl;

  // line 3
  out << "File" << stemNum << ".3 = '";
  if (forwardData.federalCustomer)
    out << 'Y';
  else
    out << 'N';
  out << ' ';
  if (forwardData.educationAllowance)
    out << 'Y';
  else
    out << 'N';
  out << ' ';
  out << theDate.asString("%m/%d/%y") << ' ';
  out << theDate.asString("%m/%d/%y") << ' ';
  out << theDate.asString("%m/%d/%y") << ' ';
  // theDate += 1; The install date should not be greater than todays date!
  out << theDate.asString("%m/%d/%y") << ' ';
  out << rpoMachine.machineType.subString(1,4) << ' ';
  out << (int)(forwardData.machineStatus) << ' ';
  out << (forwardData.customerOwning ? 1 : 2) << ' ';
  out << forwardData.specialContractNum.subString(1,5) << ' ';
  out << "RM2";
  out << "'" << endl;

  // line 4
  out << "File" << stemNum << ".4 = '";
  unsigned length = rpoMachine.serialNum.size();
  out << rpoMachine.serialNum.subString((length > 5 ? length - 4 : 1),5);
  out << " '" << endl;

  // line 5
  out << "File" << stemNum << ".5 = '";
  int numFeatures = rpoMachine.addFeatures.numberOfElements();
  RPOFeatureSet::Cursor cur(rpoMachine.addFeatures);
  int x = 1;
  for (x = 1, rpoMachine.addFeatures.setToFirst(cur);
        (x <= 10) && (x <= numFeatures);
          x++, rpoMachine.addFeatures.setToNext(cur))
    {
    RPOFeature feat = rpoMachine.addFeatures.elementAt(cur);
    out << feat.featureCode.subString(1,6) << ' ';
    out << IString(feat.qty).subString(1,3) << ' ';
    }
  for (x--; x >= 1; x--)
    rpoMachine.addFeatures.removeFirst();
  out << " '" << endl;

  // line 6
  out << "File" << stemNum << ".6 = '";
  numFeatures = rpoMachine.deleteFeatures.numberOfElements();
  cur = RPOFeatureSet::Cursor(rpoMachine.deleteFeatures);
  for (x = 1, rpoMachine.deleteFeatures.setToFirst(cur);
        (x <= 10) && (x <= numFeatures);
          x++, rpoMachine.deleteFeatures.setToNext(cur))
    {
    RPOFeature feat = rpoMachine.deleteFeatures.elementAt(cur);
    out << feat.featureCode.subString(1,6) << ' ';
    out << IString(feat.qty).subString(1,3) << ' ';
    }
  for (x--; x >= 1; x--)
    rpoMachine.deleteFeatures.removeFirst();
  out << " '" << endl;

  // line 7
  out << "File" << stemNum << ".7 = ' '\n";

  // line 8
  out << "File" << stemNum << ".8 = ' '\n";

  // line 9
  out << "File" << stemNum << ".9 = ' '\n";

  out << endl;

  if (rpoMachine.deleteFeatures.numberOfElements() ||
      rpoMachine.addFeatures.numberOfElements())
    return RPO_BUILD_DATAFORM_LEFTOVER_FEATURES;
  return RPO_BUILD_DATAFORM_OK;
}

IString uploadRPOs(
  RPO2List & rpoList,
  RPOForwardingData & forwardData
)
{
  RPOMachineSet machSet;
  RPO2ListToRPOMachineSet(machSet,rpoList);
  RPOMachine machine;
  EUOPProfile prf;
  APIRET      rc = 0;
  IString     xmitTemplate;
  IString     xmitFileName;

  prf.ReadProfile();

//Get the path where RM/2 is located to find the RM2XMIT.REX file
  xmitTemplate = EUOPProfile::strExecPath;

  //jt xmitFileName = prf.GetStringKey(EUOPProfile::WorkDir) + IString("\\NBHWMES.EXT");
  xmitFileName = xmitTemplate + IString("\\NBHWMES.EXT");//jt
  xmitTemplate += IString("\\RM2XMIT.REX");

  //jt   rc = DosCopy(xmitTemplate,xmitFileName,DCPY_EXISTING);
  rc = CopyFile(xmitTemplate,xmitFileName, 0); //edw
  if (rc)
     SetFileAttributes(xmitFileName, FILE_ATTRIBUTE_NORMAL);

  if ( !rc)                         //jt  rc != NO_ERROR)
    return RPO_UPLOAD_FILE_ERROR;

  ofstream out(xmitFileName,ios::app);

  if (out.fail())
    {
    remove(xmitFileName);
    return RPO_UPLOAD_FILE_ERROR;
    }

  int stemNum  = 1;

  for (; machSet.numberOfElements() > 0; stemNum++)
    {
    machine = machSet.firstElement();
    int result = BuildRPODataform(out, stemNum, machine, forwardData);
    if (result == RPO_BUILD_DATAFORM_FILE_ERROR)
      {
      out.close();
      remove(xmitFileName);
      return RPO_UPLOAD_FILE_ERROR;
      }
    machSet.removeFirst();
    if (result == RPO_BUILD_DATAFORM_LEFTOVER_FEATURES)
      machSet.add(machine);
    }

  // subtract one from stemNum to account for the last increment
  stemNum--;
  out << "\nNum_Of_Stems = " << stemNum << "\nReturn\n";

  out.close();
  if (out.fail())
    {
    //jt DosDelete(xmitFileName);
    remove(xmitFileName);
    return RPO_UPLOAD_FILE_ERROR;
    }
//  openHost(xmitFileName);
//  if (!getenv("RM2TESTRPO"))
//    DosDelete(xmitFileName);
  return (xmitFileName);
}
