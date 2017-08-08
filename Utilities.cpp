/******************************************************************************

    File:   Utilities.cpp
    Desc:   Utilities is a component in Level 1 of the design that provides a
            useful suite of utilities for use within any other module.

******************************************************************************/
#include "Utilities.h"

/******************************************************************************
    Name:   CUtilities
    Desc:   Default constructor
******************************************************************************/
CUtilities::CUtilities(void)
{
}

/******************************************************************************
    Name:   ~CUtilities
    Desc:   Default destructor
******************************************************************************/
CUtilities::~CUtilities(void)
{
}

/******************************************************************************
    Name:   TimeStr
    Desc:   Returns the current time in a string format
******************************************************************************/
void CUtilities::GetTime(char* msg)
{
    DBGTrace("---> CUtilities::GetTime");
    
    time_t rawtime;
    struct tm timeinfo;
    
    time (&rawtime);
    localtime_s (&timeinfo, &rawtime);
    
    strftime (msg, APP_MAX_CHAR,"%c",&timeinfo);
}

/******************************************************************************
    Name:   ByteToString
    Desc:   Convert from a byte to a string
******************************************************************************/
String CUtilities::ByteToString(byte toConvert)
{
    DBGTrace("---> CUtilities::ByteToString");
    
    String bytestring;
    ToString(&toConvert, byte_type, "", bytestring);
    return bytestring;
}

/******************************************************************************
    Name:   ByteToString
    Desc:   Convert from a 1D byte array to a string
******************************************************************************/
String CUtilities::ByteToString(byte* toConvert, int length)
{
    DBGTrace("---> CUtilities::ByteToString (with length)");
    
    String temp, bytestring;
    for (int i = 0; i < length; i++)
    {
        temp = B2S(toConvert[i]);
        strcat(bytestring, temp);
    }
    
    return bytestring;
}

/******************************************************************************
    Name:   ByteToStringArray
    Desc:   Convert from a 2D byte array by listDut to a string array
******************************************************************************/
void CUtilities::ByteToStringArray(String* output, byte* toConvert, int length, word* listDut)
{
    int dut, index;
    String temp, bytestring;
    for(int d = 0; listDut[d] != 0; d++)
    {
        dut = listDut[d] - 1;
        for (int i = 0; i < length; i++)
        {
            index = (i * TOOL_MAX_DUT) + dut;
            temp = B2S(toConvert[index]);
            strcat(bytestring, temp);
        }
        
        output[dut] = bytestring;
        memset(bytestring, NULL, sizeof(bytestring));
    }
}

/******************************************************************************
    Name:   StringToByte
    Desc:   Convert from a string to a 1D byte array
******************************************************************************/
void CUtilities::StringToByte(char* toConvert, byte* result, int max_size)
{
    DBGTrace("---> CUtilities::StringToByte");
    
    int value, position, temp;
    int hex_shift = 16;
    
    for (int i = 0; i < max_size; i++)
    {
        value = 0;
        
        for (int nybble = 0; nybble < 2; nybble++)
        {
            temp = 0;
            position = (i * 2) + nybble;
            
            switch(toConvert[position])
            {
            case '1': temp = 1; break;
            case '2': temp = 2; break;
            case '3': temp = 3; break;
            case '4': temp = 4; break;
            case '5': temp = 5; break;
            case '6': temp = 6; break;
            case '7': temp = 7; break;
            case '8': temp = 8; break;
            case '9': temp = 9; break;
            case 'A': temp = 10; break;
            case 'B': temp = 11; break;
            case 'C': temp = 12; break;
            case 'D': temp = 13; break;
            case 'E': temp = 14; break;
            case 'F': temp = 15; break;
            }
            
            // "shift" if it's the upper nybble
            value += ((nybble == 0) ? temp*hex_shift : temp);
        }
        
        result[i] = (byte)value;
    }
}

/******************************************************************************
    Name:   ToString
    Desc:   Convert from a specified type to a string
******************************************************************************/
void CUtilities::ToString(void* value, variable_type type, char* formatIn, char* output, int convertFromDouble)
{
    DBGTrace("---> CUtilities::ToString");
    
    String format = "";
    
    sprintf(output, "");
    strcpy(format, formatIn);
    
    switch(type)
    {
        case bool_type:
            {
                if (strcmp(formatIn,"") == 0)
                    strcpy(format, "%6s%s");
                bool val = *((bool*) value);
                if (val == true)
                    sprintf(output, format, "", "T");
                else
                    sprintf(output, format, "", "F");
                break;
            }
        case byte_type:
            {
                if (strcmp(formatIn,"") == 0)
                    strcpy(format, "%02X");
                sprintf(output, (char *)format, *((byte*) value));

                if (convertFromDouble)
                {
                    double valueD = *((double*) value);
                    byte valueB = (byte) valueD;
                    sprintf(output, (char *)format, valueB);
                }
                break;
            }
        case int_type:
            {
                if (strcmp(formatIn,"") == 0)
                    strcpy(format, "%3i");
                sprintf(output, (char *)format, *((int*) value));
            }
        case uint_type:
            {
                if (strcmp(formatIn,"") == 0)
                    strcpy(format, "%3u");
                sprintf(output, (char *)format, *((uint*) value));
                break;
            }
        case double_type:
            {
                if (strcmp(formatIn,"") == 0)
                    strcpy(format, "%7.3f");
                sprintf(output, (char *)format, *((double*) value));
                break;
            }
        case word_type:
            {
                if (strcmp(formatIn,"") == 0)
                    strcpy(format, "%i");
                sprintf(output, (char *)format, *((word*) value));
                break;
            }
        case dword_type:
            {
                if (strcmp(formatIn,"") == 0)
                    strcpy(format, "%i");
                sprintf(output, (char *)format, *((dword*) value));
                break;
            }
        case qword_type:
            {
                if (strcmp(formatIn,"") == 0)
                    strcpy(format, "%i");
                sprintf(output, (char *)format, *((qword*) value));
                break;
            }
        case scope_type:
            {
                switch (  *((byte*) value)  )
                {
                    case 0:
                        {
                            sprintf(output, "|-  ");
                            break;
                        }
                    case 1:
                        {
                            sprintf(output, "  -|");
                            break;
                        }
                    default:
                        {
                            ERRChk(ERROR_UNDEFINED, "unknown symbol");
                            break;
                        }
                }
                break;
            }
        default:
            {
                sprintf(output, "Error: Unknown type: %i", type);
                break;
            }
    }

}

/******************************************************************************
    Name:   IndexIntoArray
    Desc:   Indexes into a void* array, containing data of a specifed type
******************************************************************************/
void CUtilities::IndexIntoArray(void* input, variable_type type, int index, int* output)
{
    DBGTrace("---> CUtilities::IndexIntoArray");
    
    switch(type)
    {
        case bool_type:
            {
                *output = (int) &((bool*) input)[index];
                
                /*int OutputVal;
                int* inputI = (int*) input;
                int* outputI = &inputI[index];
                outputVal = (int) outputI;
                *output = outputVal;*/
                break;
            }
        case int_type:
            {
                *output = (int) &((int*) input)[index];
                
                /*int OutputVal;
                int* inputI = (int*) input;
                int* outputI = &inputI[index];
                outputVal = (int) outputI;
                *output = outputVal;*/
                break;
            }
        case double_type:
            {
                *output = (int) &((double*) input)[index];
                
                /*int outputVal;
                double* inputD = (double*) input;
                double* outputD = &inputD[index];
                outputVal = (int) outputD;
                *output = outputVal;*/
                break;
            }
        case word_type:
            {
                *output = (int) &((word*) input)[index];
                
                /*int outputVal;
                word* inputD = (word*) input;
                word* outputD = &inputD[index];
                outputVal = (int) outputD;
                *output = outputVal;*/
                break;
            }
        case byte_type:
        case scope_type:
            {
                *output = (int) &((byte*) input)[index];
                
                /*double* inputD = (double*) input;
                double* outputD = &inputD[index];
                outputVal = (int) outputD;
                *output = outputVal;*/
                break;
            }
        default:
            {
                String msg;
                sprintf(msg, "Unimplemented type: %i", type);
                CUtilities::Error.Add(msg);
                break;
            }
    }
}

/******************************************************************************
    Name:   CopyArrayToDouble
    Desc:   Copies a void * array of the specified type into a double array
******************************************************************************/
void CUtilities::InitializeArray(word value, int* size, word* output)
{
    DBGTrace("---> CUtilities::InitializeArray");
    
    String msg;
    int numDim = 0;
    
    for (int d = 0; size[d] != 0; d++)
    {
        if (d > APP_MAX_ARRAY_DIM)
        {
            sprintf(msg, "Found %i dimensions. Only %i allowed.", d, APP_MAX_ARRAY_DIM);
            CUtilities::Error.Add(msg);
            break;
        }
        numDim++;
        //size[d] = sizeIn[d];
    }
    numDim--;
    
    if (numDim > 1)
        ERRChk(ERROR_HW_CRITICAL, "More than one dimension not implemented yet.", "CUtilities::InitializeArray");
    
    for (int i = 0; i < size[0]; i++)
    {
        output[i] = value;
    }
}

/******************************************************************************
    Name:   CopyArrayToDouble
    Desc:   Copies a void * array of the specified type into a double array
******************************************************************************/
void CUtilities::CopyArrayToDouble(void* input, variable_type type, int* sizeIn, double* output, word* listDut_in)
{
    DBGTrace("---> CUtilities::CopyArrayToDouble");
    
    word* listDut = NULL;
    
    String format;
    String msg;
    int compiledindex[APP_MAX_ARRAY_DIM];
    int arrayIndicies[APP_MAX_ARRAY_DIM];
    int listDutIndex = 0, dut = 0;
    int finalIndex;
    int numDim = 1;
    int size[APP_MAX_ARRAY_DIM];
    int indexedArrayAddress = 0;
    
    memset(compiledindex, 0, sizeof(compiledindex));
    memset(arrayIndicies, 0, sizeof(arrayIndicies));
    memset(size, 0, sizeof(size));
    
    CTestbench::ConstructUsefulSize(sizeIn, size);
    
    for (int d = 0; sizeIn[d] != 0; d++)
    {
        if (d > APP_MAX_ARRAY_DIM)
        {
            sprintf(msg, "Found %i dimensions. Only %i allowed.", d, APP_MAX_ARRAY_DIM);
            CUtilities::Error.Add(msg);
            break;
        }
        numDim++;
        //size[d] = sizeIn[d];
    }
    numDim--;
    
    for (int i = 0; i < size[0]; i++)
    {
        arrayIndicies[0] = i;
        for (int j = 0; j < size[1]; j++)
        {
            arrayIndicies[1] = j;
            for (int k = 0; k < size[2]; k++)
            {
                arrayIndicies[2] = k;
                for (int m = 0; m < size[3]; m++)
                {
                    arrayIndicies[3] = m;
                
                    // is a listDut is specified
                    if (listDut != NULL)
                    {
                        //if (listDutIndex == TOOL_MAX_DUT)
                        //    ERRChk(ERROR_HW_CRITICAL, "Size argument not big enough to use listDut", "CUtilities::CopyArrayToDouble");
                        
                        // dut to be copied
                        dut = listDut[listDutIndex];
                        
                        // listDut is a zero terminated array, so copying is done
                        //if (dut == 0)
                        //    return;
                        
                        if (arrayIndicies[numDim -1] == 0)
                            listDutIndex = 0;
                        
                        if (dut == arrayIndicies[numDim -1] + 1)
                            listDutIndex++;
                        else
                            continue;
                    }
                    
                    compiledindex[0] =                     i  * size[1];
                    compiledindex[1] = (compiledindex[0] + j) * size[2];
                    compiledindex[2] = (compiledindex[1] + k) * size[3];
                    compiledindex[3] =  compiledindex[2] + m;
                    finalIndex = compiledindex[3];
                    
                    strcpy(format, "");
                    if (type == byte_type)
                        strcpy(format, "%i");
                    CUtilities::IndexIntoArray(input, type, finalIndex, &indexedArrayAddress);
                    
                    switch(type)
                    {
                        case bool_type:
                            {
                                output[finalIndex] = (double) *((bool *)indexedArrayAddress);
                                break;
                            }
                        case byte_type:
                            {
                                output[finalIndex] = (double) *((byte *)indexedArrayAddress);
                                break;
                            }
                        case int_type:
                            {
                                output[finalIndex] = (double) *((int *)indexedArrayAddress);
                                break;
                            }
                        case double_type:
                            {
                                output[finalIndex] = *((double *)indexedArrayAddress);
                                break;
                            }
                        case uint_type:
                            {
                                output[finalIndex] = (double) *((uint *)indexedArrayAddress);
                                break;
                            }
                        case word_type:
                            {
                                output[finalIndex] = (double) *((word *)indexedArrayAddress);
                                break;
                            }
                        case dword_type:
                            {
                                output[finalIndex] = (double) *((dword *)indexedArrayAddress);
                                break;
                            }
                        case qword_type:
                            {
                                output[finalIndex] = (double) *((qword *)indexedArrayAddress);
                                break;
                            }
                        default:
                            {
                                CUtilities::Error.Add("CopyArrayToDouble::Unknown type");
                                break;
                            }
                        
                    }
                    
                    //CUtilities::ToString((void*) indexedArrayAddress, type, format, valueStr);
                    //output[index[3]] = atof(valueStr);
                }
            }
        }
    }
}

/******************************************************************************
    Name:   BreakOutPowersOf2
    Desc:   Take a double and break out all the powers of two and return a string
******************************************************************************/
void CUtilities::BreakOutPowersOf2(dword value, char* output, int dir)
{
    DBGTrace("---> CUtilities::BreakOutPowersOf2");
    
    String piece;
    sprintf(output, "");
    
    if (dir == 0)
    {
        for (int i = 0; i <= 32; i++)
        {
            if ((value & (1<<i)) != 0)
            {
                sprintf(piece, "%i ", 1<<i);
                strcat(output, (char *)piece);
            }
        
        }
    }
    else
    {
        for (int i = 31; i > 0; i--)
        {
            if ((value & (1<<i)) != 0)
            {
                sprintf(piece, "%i ", 1<<i);
                strcat(output, (char *)piece);
            }
        
        }
    }
}

/******************************************************************************
    Name:   ConcatenateArray
    Desc:   Concatenate 2 zeros terminated arrays and return result in variable array_in
******************************************************************************/
void CUtilities::ConcatenateArray(int* array_in, int* array_to_add)
{
    DBGTrace("---> CUtilities::ConcatenateArray");
    
    int index = 0;
    int index2 = 0;
    for (index = 0; array_in[index] != 0; index++) { ; }
    for (int index2 = 0; array_to_add[index2] != 0; index2++)
    {
        array_in[index++] = array_to_add[index2];
    }
}

/******************************************************************************
    Name:   FindInArray
    Desc:   Find value in zero terminated array
******************************************************************************/
int CUtilities::FindInArray(int value, int* array_in)
{
    DBGTrace("---> CUtilities::FindInArray");
    
    int index = 0;
    while (array_in[index] != 0)
    {
        if (array_in[index] == value)
            return index;
        index++;
    }
    
    return -1;
}

/******************************************************************************
    Name:   FormatString
    Desc:   
******************************************************************************/
int CUtilities::FormatString(String* labels_in, String labelFormat, int count, String* output, String* indicies)
{
    DBGTrace("---> CUtilities::FormatString");
    
    String tmpStr;
    
    sprintf(*output,"");
    for (int i = 0; i < count; i++)
    {
        for (int j = 0; strcmp(labels_in[j],"") == 1; j++)
        {
            if (indicies != NULL)
            {
                sprintf(tmpStr,(char *) labelFormat, (char *) labels_in[j], (char *) indicies[i]);
            }
            else
            {
                sprintf_s(tmpStr, APP_MAX_CHAR, (char *) labelFormat, (char *)labels_in[j]);
            }
            strcat(*output,tmpStr);            
        }
    }
    
    return SUCCESS;
}