/*******************************************************************************
  
    File:   KChamber.h
    Desc:   Chamber class definition for the SPEA x70 Ping-pong tester.  
            Each chamber contains 35 sockets, and this class controls any 
            necessary functionality pertaining to having 2 chambers (such as
            switching between chambers, gettin SNs, checking SNs, etc).
  
*******************************************************************************/
#ifndef _K_CHAMBER_H_
#define _K_CHAMBER_H_

#include "KDefines.h"

//-----------------------------------------------------------------------------
//  chamber class
class Chamber
{
private:
    INT CurrChamber;
    WORD DutList[2][APP_HALF_DUT + 1];
    QWORD SNList[APP_MAX_DUT];
    
    Chamber(void);
    static Chamber *Instance;
    
    void SetChamber(INT chamber);
    void Toggle(void);
    BOOL IsOdd(WORD dut);

public:
    ~Chamber(void);
    
    static Chamber *GetInstance(void);
    
    void Begin(void);
    void End(void);
    INT GetChamber(void);
    void Switch(void);
    void SetSNList(QWORD *currSN);
    void SNCheck(QWORD *currSN, DOUBLE *ValidList);
    void SNCombineArray(DOUBLE *TestDataArray, DOUBLE *Array);
    WORD* GetDutList(void);
    void UpdateDutList(WORD *listDut);
    void PrintChamber(void);
    void PrintSNList(void);
};

#endif