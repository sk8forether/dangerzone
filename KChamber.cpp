/*******************************************************************************
  
    File:   KChamber.cpp
    Desc:   Chamber class function definitions for the SPEA x70 Ping-pong
            tester.  Each chamber contains 35 sockets, and this class controls 
            any necessary functionality pertaining to having 2 chambers (such 
            as switching between chambers, gettin SNs, checking SNs, etc).
  
*******************************************************************************/
#include "KChamber.h"

Chamber *Chamber::Instance = NULL;

/******************************************************************************
    Name:   Chamber
    Desc:   Default constructor
******************************************************************************/
Chamber::Chamber(void)
{
    // initialize to CHAMBER_1 to start
    this->CurrChamber = CHAMBER_1;
    this->SetChamber(this->CurrChamber);
    
    memset(this->DutList, 0, sizeof(this->DutList));
    memset(this->SNList, 0, sizeof(this->SNList));
}

/******************************************************************************
    Name:   ~Chamber
    Desc:   Default destructor
******************************************************************************/
Chamber::~Chamber(void)
{
    DBGTrace("==> Chamber::~Chamber\n");
}

/******************************************************************************
    Name:   GetInstance
    Desc:   Returns/creates singleton instance 
******************************************************************************/
Chamber *Chamber::GetInstance(void)
{
    if (Chamber::Instance == NULL)
        Chamber::Instance = new Chamber();
    
    return Chamber::Instance;
}

/******************************************************************************
    Name:   Begin
    Desc:   Does the same stuff as the constructor to initialize the class at
            the beginning of the testflow in Testplan.cpp
******************************************************************************/
void Chamber::Begin(void)
{
    DBGTrace("==> Chamber::Begin\n");
    
    // initialize to CHAMBER_1 to start
    this->CurrChamber = CHAMBER_1;
    this->SetChamber(this->CurrChamber);
    
    memset(this->DutList, 0, sizeof(this->DutList));
    memset(this->SNList, 0, sizeof(this->SNList));
}

/******************************************************************************
    Name:   End
    Desc:   Disconnect both chambers at the end of the testflow in Testplan.cpp
******************************************************************************/
void Chamber::End(void)
{
    DBGTrace("==> Chamber::End\n");
    
    // SPEA Hardware calls
    DxMtx110ManageV2 ( CHAMBER_1, OPEN );
    DxMtx110ManageV2 ( CHAMBER_2, OPEN );
}

/******************************************************************************
    Name:   GetChamber
    Desc:   Returns the active chamber
******************************************************************************/
INT Chamber::GetChamber(void)
{
    DBGTrace("==> Chamber::GetChamber\n");
    
    return this->CurrChamber;
}

/******************************************************************************
    Name:   SetChamber
    Desc:   Actually does the hardware calls to change the active chamber
            DxMtx110ManageV2 is located in Testplan.cpp
            Chamber(1 or 2), Action(OPEN,CLOSE)
******************************************************************************/
void Chamber::SetChamber(INT chamber)
{
    DBGTrace("==> Chamber::SetChamber\n");
    
    INT otherchamber = 1 - chamber;
    
    // SPEA Hardware calls
    DxMtx110ManageV2 ( otherchamber, OPEN );
    DxMtx110ManageV2 ( chamber, CLOSE );
}

/******************************************************************************
    Name:   Toggle
    Desc:   Switches which chamber is currently "active" in the Software
******************************************************************************/
void Chamber::Toggle(void)
{
    DBGTrace("==> Chamber::Toggle\n");
    
    this->CurrChamber = 1 - this->CurrChamber;
}

/******************************************************************************
    Name:   Switch
    Desc:   Switches which chamber is currently active in the Hardware
******************************************************************************/
void Chamber::Switch(void)
{
    DBGTrace("==> Chamber::Switch\n");
    
    this->Toggle();
    
    this->SetChamber(this->CurrChamber);
    
    // debug
    //this->PrintChamber();
}

/******************************************************************************
    Name:   SetSNList
    Desc:   Saves the SNs for the CurrChamber in SNList
******************************************************************************/
void Chamber::SetSNList(QWORD *currSN)
{
    DBGTrace("==> Chamber::SetSNList\n");
    
    INT dut;
    
    // get active chamber
    INT chamber = this->CurrChamber;
    
    // save SN
    for (INT d = 0; this->DutList[chamber][d] != 0; d++)
    {
        dut = this->DutList[chamber][d] - 1;
        this->SNList[dut] = currSN[dut];
    }
}

/******************************************************************************
    Name:   SNCheck
    Desc:   Gets the SNs for the parts in the CurrChamber that is active, and
            verifies they are correct against the SNList for that chamber
******************************************************************************/
void Chamber::SNCheck(QWORD *currSN, DOUBLE *ValidList)
{
    DBGTrace("==> Chamber::SNCheck\n");
    
    INT dut;
    
    // get active chamber
    INT chamber = this->CurrChamber;
    
    // compare to correct SNs
    for (INT d = 0; this->DutList[chamber][d] != 0; d++)
    {
        dut = this->DutList[chamber][d] - 1;
        
        // if it has already failed, keep it that way
        if (ValidList[dut] == FALSE)
            continue;
        else if (currSN[dut] == this->SNList[dut])
            ValidList[dut] = TRUE;
        else
            ValidList[dut] = FALSE;
    }
}

/******************************************************************************
    Name:   SNCombineArray
    Desc:   Takes in 2 arrays (TestDataArray and Array) each of length
            2APP_MAX_DUT and combines the values from Array into TestDataArray
            (for use in KTests)
******************************************************************************/
void Chamber::SNCombineArray(DOUBLE *TestDataArray, DOUBLE *Array)
{
    DBGTrace("==> Chamber::SNCombineArray\n");
    
    INT odd, even;
    
    for (INT n = 0; n < APP_HALF_DUT; n++)
    {
        odd = 2*n + 1;
        even = 2*n;
        
        TestDataArray[odd] = Array[odd];
        TestDataArray[even] = Array[even];
    }
}

/******************************************************************************
    Name:   GetDutList
    Desc:   Returns a pointer to a DUT List for the active chamber
******************************************************************************/
WORD* Chamber::GetDutList(void)
{
    return this->DutList[this->CurrChamber];
}

/******************************************************************************
    Name:   UpdateDutList
    Desc:   Updates the DUT list for each chamber based on the global Die list
******************************************************************************/
void Chamber::UpdateDutList(WORD *listDut)
{
    //DBGTrace("==> Chamber::UpdateDutList\n");
    
    memset(this->DutList, 0, sizeof(this->DutList));
    
    INT a = 0, b = 0;
    
    for (INT d = 0; listDut[d] != 0; d++)
    {
        if( IsOdd(listDut[d]) )
        {
            this->DutList[CHAMBER_1][a] = listDut[d];
            a++;
        }
        else
        {
            this->DutList[CHAMBER_2][b] = listDut[d];
            b++;
        }
        
        if (d > 70)
            break;
    }
}

/******************************************************************************
    Name:   IsOdd
    Desc:   Checks if a DUT is odd
******************************************************************************/
BOOL Chamber::IsOdd(WORD dut)
{
    return (BOOL)dut%2;
}

/******************************************************************************
    Name:   PrintChamber
    Desc:   Prints the active chamber all fancy-like to debug output
******************************************************************************/
void Chamber::PrintChamber(void)
{
    CHAR msg[APP_MAX_CHAR];
    sprintf(msg, "Chamber %i is currently active\n", this->CurrChamber + 1);
    DBGPrint(msg);
}

/******************************************************************************
    Name:   PrintSNList
    Desc:   Prints the list of SNs for the active chamber to debug output
******************************************************************************/
void Chamber::PrintSNList(void)
{
    DBGTrace("==> Chamber::PrintSNList\n");
    
    INT dut;
    UNION64 sn;
    
    // get active chamber
    INT chamber = this->CurrChamber;
    
    CHAR tmp[APP_MAX_CHAR];
    CHAR tmp2[APP_MAX_CHAR];
    CHAR msg[4096];
    
    // init string
    memset(msg, '\0', sizeof(msg));
    
    // append title
    sprintf(tmp, "SNList for Chamber %i", this->CurrChamber + 1);
    strcat(tmp, APP_NEWLINE);
    strcat(msg, tmp);
    
    // append meas
    for (INT n = 0; n < APP_HALF_DUT; n++)
    {
        if (chamber == CHAMBER_1)
        {
            dut = 2*n;
        }
        else
        {
            dut = 2*n + 1;
        }
        
        sn.qword = 0;
        sn.qword = this->SNList[dut];
        sprintf(tmp, "Dut %02d: ", dut + 1);
        for(INT i = APP_PROM_SN_REG; i != 0 ; i--)
        {
            sprintf(tmp2, "%02X", sn.byte[i - 1]);
            strcat(tmp,tmp2);
        }
        
        strcat(tmp, APP_NEWLINE);
        strcat(msg, tmp);
    }
    
    // print out debug
    DBGPrint(msg);
}
