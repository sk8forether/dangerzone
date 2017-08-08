/******************************************************************************

    File:   Utilities.h
    Desc:   Utilities is a component in Level 1 of the design that provides a
            useful suite of utilities for use within any other module.

******************************************************************************/
#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include "Defines.h"

#define CONVERT_FROM_DOUBLE_DISABLED    0
#define CONVERT_FROM_DOUBLE_ENABLED     1

#include "Message.h"
#include "Error.h"
#include "Timer.h"
#include "FileIO.h"
//#include "Parser.h"

//-----------------------------------------------------------------------------
//  Utilities class definition
class CUtilities
{
public:
    CUtilities(void);
    ~CUtilities(void);
    
    static CMessage Message;
    static CError Error;
    static CTimer Timer;
    //static CParser Parser;
    
    static bool IsOdd(int number) { return ( number%2 == 1 ); }
    static bool IsOdd(byte number) { return ( number%2 == 1 ); }
    static bool IsOdd(word number) { return ( number%2 == 1 ); }
    static bool IsOdd(dword number) { return ( number%2 == 1 ); }
    static bool IsOdd(qword number) { return ( number%2 == 1 ); }
    static bool IsOdd(double number) { return ( ((int)floor(number))%2 == 1 ); }
    
    static String ByteToString(byte toConvert);
    static String ByteToString(byte* toConvert, int length);
    static void ByteToStringArray(String* output, byte* toConvert, int length, word* listDut);
    static void StringToByte(char* toConvert, byte* result, int max_size);
    
    static void GetTime(char* msg);
    static void ToString(void* value, variable_type type, char* format, char* output, int convertFromDouble = CONVERT_FROM_DOUBLE_DISABLED);
    static void IndexIntoArray(void* input, variable_type type, int index, int* output);
    static void InitializeArray(word value, int* size, word* output);
    static void CopyArrayToDouble(void* input, variable_type type, int* size, double* output, word* listDut = NULL);
    static void BreakOutPowersOf2(dword value, char* output, int dir = 1);
    static void ConcatenateArray(int* array_in, int* array_to_add);
    static int FindInArray(int value, int* array_in);
    static int FormatString(String* labels_in, String labelFormat, int count, String* output, String* indicies = NULL);
};

#endif