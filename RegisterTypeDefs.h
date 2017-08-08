/******************************************************************************

    File:   RegisterTypeDefs.h
    Desc:   Defines the structs used for each register in the ASIC, and the RAM
            struct used in GetRAM.

******************************************************************************/
#ifndef _REGISTER_TYPE_DEFS_H_
#define _REGISTER_TYPE_DEFS_H_

#include <vector>
#include <algorithm>
#include <functional>

#include "Defines.h"

//-----------------------------------------------------------------------------
// conversion types
typedef struct convert_type
{
    String names[MAX_CONTYPE];
    contype type;
    String name;

    convert_type(void) { SetNames(); }
    
    convert_type(contype type_in) : type(type_in)
    {
        SetNames();
        name = names[type_in];
    }
    
    void SetNames(void)
    { 
        names[convert_bit] = STR(bit);
        names[convert_byte] = STR(byte);
        names[convert_uint] = STR(unsigned int);
        names[convert_SN] = STR(serial number);
        names[twos_comp] = "two's compliment";
        names[twos_comp_invert_low] = "two's compliment with negatives inverted";
        names[twos_comp_invert_high] = "two's compliment with positives inverted";
        names[sense_output] = STR(sense element output);
    }
} convert_type;

//-----------------------------------------------------------------------------
//  memory types
typedef struct memory_type
{
    String names[ROM + 1];
    mtype type;
    String name;

    memory_type(void) { SetNames(); }
    
    memory_type(mtype type_in) : type(type_in)
    {
        SetNames();
        name = names[type_in];
    }
    
    void SetNames(void)
    { 
        names[Volatile] = STR(Volatile);
        names[RAM] = STR(RAM);
        names[ROM] = STR(ROM);
    }
    
    bool operator!=(const memory_type& m) const
    {
        return type != m.type;
    }
    
    bool operator==(const memory_type& m) const
    {
        return type == m.type;
    }
} memory_type;

//-----------------------------------------------------------------------------
//  register struct
typedef struct ASICregister
{
    String          name;               // for use in debug
    byte            page;               // must be on the same page within a register
    byte            addr[APP_MAX_ADDR]; // can have multiple addresses
    byte            mask[APP_MAX_ADDR]; // each addr has a mask
    convert_type    conversion;         // read as byte, signed int, unsigned, etc.
    memory_type     memory;             // Volatile, RAM, or ROM
    int             num_registers;      // same for each addr in array
    int             lsb[2];             // { addr array index, bit (7-0) }
    int             msb[2];             // same style as lsb
    
    // default contructor
    ASICregister(void) {}
    
    // construct with single addr and mask
    ASICregister(String name_in, byte page_in, byte addr_in, byte mask_in,
        convert_type conv_in, memory_type mem_in, int num_in)
        : name(name_in), page(page_in), conversion(conv_in), memory(mem_in),
        num_registers(num_in)
    {
        memset(addr, ADDR_INVALID, sizeof(addr));
        memset(mask, NULL, sizeof(mask));
        
        addr[0] = addr_in;
        mask[0] = mask_in;
    }
    
    // construct with multiple addr and mask
    ASICregister(String name_in, byte page_in, byte* addr_in, byte* mask_in,
        convert_type conv_in, memory_type mem_in, int num_in)
        : name(name_in), page(page_in), conversion(conv_in), memory(mem_in),
        num_registers(num_in)
    {
        memset(addr, ADDR_INVALID, sizeof(addr));
        memset(mask, NULL, sizeof(mask));
        
        for (int a = 0; a < num_registers; a++)
        {
            addr[a] = addr_in[a];
            mask[a] = mask_in[a];
        }
    }
    
    // construct with multiple addr and mask, including a lsb and msb
    ASICregister(String name_in, byte page_in, byte* addr_in, byte* mask_in,
        convert_type conv_in, memory_type mem_in, int num_in, int* lsb_in, int* msb_in)
        : name(name_in), page(page_in), conversion(conv_in), memory(mem_in),
        num_registers(num_in)
    {
        memset(addr, ADDR_INVALID, sizeof(addr));
        memset(mask, NULL, sizeof(mask));
        
        for (int a = 0; a < num_registers; a++)
        {
            addr[a] = addr_in[a];
            mask[a] = mask_in[a];
        }
        
        memcpy(lsb, lsb_in, sizeof(lsb));
        memcpy(msb, msb_in, sizeof(msb));
    }
    
    // convert byte values from register into ints
    void ConvertToInt(byte* values, int* output, word* listDut)
    {
        int dut, index;
        int temp = 0;
        
        for (int d = 0; listDut[d] != 0; d++)
        {
            dut = listDut[d] - 1;
            index = ((0 * TOOL_MAX_DUT) + dut);
            
            switch(conversion.type)
            {
            case convert_bit:
                temp = ((values[index] & mask[0]) > 0) ? 1 : 0;
                
                if (num_registers > 1)
                {
                    String msg;
                    sprintf(msg, "ASICregister %s is incorrectly setup with %i bytes and %s", (char*)name, num_registers, (char*)conversion.name);
                    ERRLog(ERROR_ASIC_SPECIFIC, msg);
                }
                
                output[index] = temp;
                break;
                
            case convert_byte:
                temp = (int)(values[index] & mask[0]);
                
                if (num_registers > 1)
                {
                    String msg;
                    sprintf(msg, "ASICregister %s is incorrectly setup with %i bytes and %s", (char*)name, num_registers, (char*)conversion.name);
                    ERRLog(ERROR_ASIC_SPECIFIC, msg);
                }
                
                output[index] = temp;
                break;
                
            case convert_uint:
                if (num_registers == 1)
                    temp = (int)(values[index] & mask[0]);
                else
                {
                    //
                    for (int a = 0; a < num_registers; a++)
                    {
                        index = ((a * TOOL_MAX_DUT) + dut);
                        temp += (int)(values[index] & mask[a]);
                    }
                }
                
                output[index] = temp;
                break;
                
            case convert_SN:
                //
                break;
                
            case twos_comp:
                //
                break;
                
            case twos_comp_invert_low:
                //
                break;
                
            case twos_comp_invert_high:
                //
                break;
                
            case sense_output:
                //
                break;
                
            default:
                ERRLog(ERROR_UNDEFINED, "No such conversion-type exists.");
                break;
            }
            
            //output[index] = temp;
        }
    }
    
    // convert int values to set to a register into bytes
    //void ConvertFromInt(int* values, byte* fromDUT, byte* output, word* listDut)
    void ConvertFromInt(int* values, byte* output, word* listDut)
    {
        int dut, index;
        //int temp;
        
        for (int d = 0; listDut[d] != 0; d++)
        {
            dut = listDut[d] - 1;
            index = ((0 * TOOL_MAX_DUT) + dut);
            
            switch(conversion.type)
            {
            case convert_bit:
                //
                
                break;
                
            case convert_byte:
                //
                
                break;
                
            case convert_uint:
                //
                break;
                
            case convert_SN:
                //
                break;
                
            case twos_comp:
                //
                break;
                
            case twos_comp_invert_low:
                //
                break;
                
            case twos_comp_invert_high:
                //
                break;
                
            case sense_output:
                //
                break;
                
            default:
                ERRLog(ERROR_UNDEFINED, "No such conversion-type exists.");
                break;
            }
            
            //output[index] = temp;
        }
    }
    
    // compare registers based on address
    bool operator<(const ASICregister& a) const
    {
        if (page == a.page)
            return addr[0] < a.addr[0];
        else
            return page < a.page;
    }
    bool operator>(const ASICregister& a) const
    {
        if (page == a.page)
            return addr[0] > a.addr[0];
        else
            return page > a.page;
    }
    bool operator==(const ASICregister& a) const
    {
        if (page != a.page)
            return false;
        if (memory != a.memory)
            return false;
        
        for (int i = 0; i < APP_MAX_ADDR; i++)
        {
            if ( (addr[i] != a.addr[i]) || (mask[i] != a.mask[i]) )
                return false;
        }
        
        // if everything up to this point matches, they are the same
        return true;
    }
    
    // print all information on a register
    void Print(void)
    {
        char msg[APP_MAX_CHAR_LONGER];
        String temp;
        
        sprintf(msg, "Register: %s\n", (char *)name);
        
        #ifdef _HAS_PAGES_
            sprintf(temp, "Page%02X\n", page);
        #else
            sprintf(temp, "");
        #endif
        strcat(msg, temp);
        
        for (int i = 0; addr[i] != ADDR_INVALID; i++)
        {
            sprintf(temp, "Addr%i: %02X  Mask: %02X\n", i, addr[i], mask[i]);
            strcat_s(msg, APP_MAX_CHAR_LONGER, temp);
        }
        
        sprintf(temp, "Read as: %s\n", (char *)conversion.name );
        strcat(msg, temp);
        
        sprintf(temp, "Memory Type: %s\n", (char *)memory.name);
        strcat(msg, temp);
        
        sprintf(temp, "Number of bytes: %i\n", num_registers);
        strcat(msg, temp);
        
        // always display
        bool was_on = DBGVerboseEnabled;
        DBGVerboseEnabled = YES;
        DBGVerbose(msg);
        DBGVerboseEnabled = was_on;
    }
} ASICregister;

//-----------------------------------------------------------------------------
//  register comparison
struct find_address : unary_function<ASICregister, bool>
{
    byte addr;
    find_address(byte addr) : addr(addr) {}
    bool operator() (const ASICregister& a) const
    {
        for (int i = 0; i < APP_MAX_ADDR; i++)
        {
            if (addr == a.addr[i])
                return true;
        }
        return false;
    }
};

//-----------------------------------------------------------------------------
//  RAM register struct
typedef struct RAMstruct
{
    vector<ASICregister> RAMvector;  // contains all RAM registers in ASIC
    int count;                       // number of bytes in RAM registers
    
    RAMstruct(void) { count = 0; }
    
    int Size(void) { return count; }
    
    void Add(ASICregister reg)
    {
        vector<ASICregister>::iterator it;
        
        // add register to vector if it's not already in it
        if (find(RAMvector.begin(), RAMvector.end(), reg) != RAMvector.end())
            ERRWarn("Tried to Add an ASICregister to RAMstruct more than once.");
        else
        {
            // keep track of the number of bytes in RAM
            count += reg.num_registers;
            for (int i = 0; i < APP_MAX_ADDR; i++)
            {
                if (reg.addr[i] != ADDR_INVALID)
                {
                    it = find_if(RAMvector.begin(), RAMvector.end(), find_address(reg.addr[i]));
                    if (it != RAMvector.end())
                        count--;
                }
            }
            
            RAMvector.push_back(reg);
        }
        
        // keep vector sorted by address (low to high)
        sort(RAMvector.begin(), RAMvector.end());
    }
    
    // ensure it contains all RAM registers and values
    void Check(void)
    {
        if (Size() != NUM_RAM_REG)
        {
            String msg;
            Print();
            sprintf(msg, "RAMstruct is not the proper size! (%i != %i)", Size(), NUM_RAM_REG);
            ERRCrit(ERROR_INIT, msg);
        }
        
        if (RAMvector.size() != NUM_RAM_VALUES)
        {
            String msg;
            Print();
            sprintf(msg, "RAMvector is not the proper size! (%i != %i)", RAMvector.size(), NUM_RAM_VALUES);
            ERRCrit(ERROR_INIT, msg);
        }
    }
    
    // prints all RAM register names, page, and addresses in vector for debug
    void Print(void)
    {
        char msg[APP_MAX_CHAR_LONGER];
        String temp;
        vector<ASICregister>::iterator it;
        
        sprintf(msg, "\nRAM Registers:");
        
        #ifdef _HAS_PAGES_
            sprintf(temp, " Page%02X\n", RAMvector.begin()->page);
        #else
            sprintf(temp, "\n");
        #endif
        strcat(msg, temp);
        
        for (it = RAMvector.begin(); it != RAMvector.end(); ++it)
        {
            sprintf(temp, "Register %02X: %s\n", it->addr[0], (char *)it->name);
            strcat_s(msg, APP_MAX_CHAR_LONGER, temp);
        }
        
        // always display
        bool was_on = DBGVerboseEnabled;
        DBGVerboseEnabled = YES;
        DBGVerbose(msg);
        DBGVerboseEnabled = was_on;
    }
    
    String ToString(int range_low, int range_high)
    {
        if (range_high - range_low > APP_MAX_REG_PRINT)
        {
            ERRLog("Cannot print that many registers with this ToString function.");
            return "";
        }
        if ( range_low >= (int)RAMvector.size() )
        {
            ERRLog("low input is passed end of range. Remember, not every ASICregister is only 1 byte.");
            return "";
        }
        if ( range_high > (int)RAMvector.size() )
        {
            ERRLog("high input is passed end of range. Remember not every ASICregister is only 1 byte.");
            return "";
        }
        
        String temp, msg;
        vector<ASICregister>::iterator it = RAMvector.begin() + range_low;
        vector<ASICregister>::iterator it2 = RAMvector.begin() + range_high;
        
        sprintf(msg, "\nRAM (%i,%i):", range_low, range_high);
        
        #ifdef _HAS_PAGES_
            sprintf(temp, " Page%02X\n", RAMvector.begin()->page);
        #else
            sprintf(temp, "\n");
        #endif
        strcat(msg, temp);
        
        for (; it != it2; it++)
        {
            if (it > RAMvector.end())
            {
                sprintf(temp, "ERROR: surpassed end of range.");
                strcat(msg, temp);
                return msg;
            }
            sprintf(temp, "%s\n", (char *)it->name);
            msg += temp;
        }
        
        return msg;
    }
    
    // prints all information for each RAM register in vector for debug
    void PrintAll(void)
    {
        vector<ASICregister>::iterator it;
        for (it = RAMvector.begin(); it != RAMvector.end(); ++it)
        {
            it->Print();
        }
    }
} RAMstruct;

//-----------------------------------------------------------------------------
//  Default struct
//  Used for storing, comparing, and displaying the Default RAM image and mask
typedef struct Default
{
    byte            SpecImage[NUM_RAM_REG];
    byte            Mask[NUM_RAM_REG];
    
    // default contructor
    Default(void)
    {
        memset(SpecImage, 0x00, sizeof(SpecImage));
        memset(Mask, 0x00, sizeof(Mask));
    }
    
    // construct with strings from Spec
    Default(char* default_in, char* mask_in)
    {
        S2B(default_in, SpecImage, sizeof(SpecImage));
        S2B(mask_in, Mask, sizeof(Mask));
    }
    
    void Compare(byte* Raw, byte* difference, bool* result, word* listDut)
    {
        int dut, index;
        byte diff[NUM_RAM_REG][TOOL_MAX_DUT];
        
        memset(diff, 0x00, sizeof(diff));
        
        // Note: memset will not set a boolean array to true correctly
        // If the spec image doesn't match for at least one byte, result will be false for that dut
        for (int i = 0; i < TOOL_MAX_DUT; i++)
            result[i] = true;
        
        for (int d = 0; listDut[d] != 0; d++)
        {
            dut = listDut[d] - 1;
            for (int i = 0; i < NUM_RAM_REG; i++)
            {
                index = (i * TOOL_MAX_DUT) + dut;
                if ( Raw[index] != SpecImage[i] )
                {
                    result[dut] = false;
                    diff[i][dut] = (Raw[index] ^ SpecImage[i]);
                }
            }
        }
        memcpy(difference, diff, sizeof(diff));
    }
    
    void CompareMasked(byte* Raw, byte* difference, bool* result, word* listDut)
    {
        int dut, index;
        byte diff[NUM_RAM_REG][TOOL_MAX_DUT];
        
        memset(diff, 0x00, sizeof(diff));
        
        for (int i = 0; i < TOOL_MAX_DUT; i++)
            result[i] = true;
        
        for (int d = 0; listDut[d] != 0; d++)
        {
            dut = listDut[d] - 1;
            for (int i = 0; i < NUM_RAM_REG; i++)
            {
                index = (i * TOOL_MAX_DUT) + dut;
                if ( (Raw[index] & Mask[i]) != (SpecImage[i]& Mask[i]) )
                {
                    result[dut] = false;
                    diff[i][dut] = (Raw[index] ^ SpecImage[i]);
                }
            }
        }
        memcpy(difference, diff, sizeof(diff));
    }
    
    void Print(void)
    {
        // always display
        bool was_on = DBGVerboseEnabled;
        DBGVerboseEnabled = YES;
        
        // print SpecImage and Mask
        String msg;
        sprintf(msg, "\nDefaultImage: %s", (char*)B2S(SpecImage, NUM_RAM_REG));
        DBGVerbose(msg);
        sprintf(msg, "DefaultMask:  %s\n", (char*)B2S(Mask, NUM_RAM_REG));
        DBGVerbose(msg);
        
        // restore setting
        DBGVerboseEnabled = was_on;
    }
} Default;

//-----------------------------------------------------------------------------
//  Image struct
//  Used for storing, comparing, and displaying RAM and ROM images
typedef struct Image
{
    byte            Raw[NUM_RAM_REG][TOOL_MAX_DUT];
    int             Converted[NUM_RAM_VALUES][TOOL_MAX_DUT];
    memory_type     memory;
    byte            page;
    
    Image(void) {}
    
    Image(mtype mem_in, byte page_in) : memory(mem_in), page(page_in)
    {
        memset(Raw, 0x00, sizeof(Raw));
        memset(Converted, 0, sizeof(Converted));
    }
    
    Image(mtype mem_in, byte page_in, byte* raw_in) : memory(mem_in), page(page_in)
    {
        memcpy(Raw, raw_in, sizeof(Raw));
        memset(Converted, 0, sizeof(Converted));
    }
    
    void SetRaw(byte* raw_in)
    {
        memcpy(Raw, raw_in, sizeof(Raw));
    }
    
    // convert raw register values from byte to int using RAMstruct
    void Convert(int* output, RAMstruct RAMregisters, word* listDut)
    {
        // TODO
    }
    
    // compare array to array of bytes
    void Compare(byte* otherRaw, byte* difference, bool* result, word* listDut)
    {
        int dut, index;
        String temp[TOOL_MAX_DUT];
        char msg[APP_MAX_CHAR_LONGER];
        byte diff[NUM_RAM_REG][TOOL_MAX_DUT];
        
        memset(diff, 0x00, sizeof(diff));
        
        for (int i = 0; i < TOOL_MAX_DUT; i++)
            result[i] = true;
        
        for (int d = 0; listDut[d] != 0; d++)
        {
            dut = listDut[d] - 1;
            for (int i = 0; i < NUM_RAM_REG; i++)
            {
                index = (i * TOOL_MAX_DUT) + dut;
                if (Raw[i][dut] != otherRaw[index])
                {
                    result[dut] = false;
                    diff[i][dut] = (Raw[i][dut] ^ otherRaw[index]);
                }
            }
            
            B2SArray(&temp[0], &diff[0][0], NUM_RAM_REG, listDut);
            
            // print difference
            if (result[dut] == false)
            {
                sprintf_s(msg, APP_MAX_CHAR_LONGER, "\n%sImage diff array [%i]: ", (char*)memory.name, dut);
                strcat_s(msg, APP_MAX_CHAR_LONGER, temp[dut]);
                DBGVerbose(msg);
            }
        }
        
        if (difference != NULL)
            memcpy(difference, diff, sizeof(diff));
    }
    
    // compare images
    void Compare(Image img, byte* difference, bool* result, word* listDut)
    {
        int dut;
        String temp[TOOL_MAX_DUT];
        char msg[APP_MAX_CHAR_LONGER];
        byte diff[NUM_RAM_REG][TOOL_MAX_DUT];
        
        memset(diff, 0x00, sizeof(diff));
        
        for (int i = 0; i < TOOL_MAX_DUT; i++)
            result[i] = true;
        
        for (int d = 0; listDut[d] != 0; d++)
        {
            dut = listDut[d] - 1;
            for (int i = 0; i < NUM_RAM_REG; i++)
            {
                if (Raw[i][dut] != img.Raw[i][dut])
                {
                    result[dut] = false;
                    diff[i][dut] = (Raw[i][dut] ^ img.Raw[i][dut]);
                }
            }
            
            B2SArray(&temp[0], &diff[0][0], NUM_RAM_REG, listDut);
            
            // print difference
            if (result[dut] == false)
            {
                sprintf_s(msg, APP_MAX_CHAR_LONGER, "\n%sImage diff %sImage [%i]: ", (char*)memory.name, (char*)img.memory.name, dut);
                strcat_s(msg, APP_MAX_CHAR_LONGER, temp[dut]);
                DBGVerbose(msg);
            }
        }
        
        if (difference != NULL)
            memcpy(difference, diff, sizeof(diff));
    }
    
    void CompareDefault(Default DefaultImg, byte* difference, bool* result, word* listDut)
    {
        byte diff[NUM_RAM_REG][TOOL_MAX_DUT];
        memset(diff, 0x00, sizeof(diff));
        
        DefaultImg.Compare(&Raw[0][0], &diff[0][0], result, listDut);
        
        // debug output of img difference from default img
        if (DBGVerboseEnabled)
        {
            int dut;
            String temp[TOOL_MAX_DUT];
            char msg[APP_MAX_CHAR_LONGER] = "";
            
            B2SArray(&temp[0], &diff[0][0], NUM_RAM_REG, listDut);
            
            for (int d = 0; listDut[d] != 0; d++)
            {
                dut = listDut[d] - 1;
                
                if (result[dut] == false)
                {
                    sprintf_s(msg, APP_MAX_CHAR_LONGER, "\n%sImage diff DefaultImage [%i]: ", (char*)memory.name, dut);
                    strcat_s(msg, APP_MAX_CHAR_LONGER, temp[dut]);
                    DBGVerbose(msg);
                }
            }
        }
        
        if (difference != NULL)
            memcpy(difference, diff, sizeof(diff));
    }
    
    void CompareMaskedDefault(Default DefaultImg, byte* difference, bool* result, word* listDut)
    {
        int dut;
        String temp[TOOL_MAX_DUT];
        char msg[APP_MAX_CHAR_LONGER];
        
        byte diff[NUM_RAM_REG][TOOL_MAX_DUT];
        memset(diff, 0x00, sizeof(diff));
        
        DefaultImg.CompareMasked(&Raw[0][0], &diff[0][0], result, listDut);
        
        B2SArray(&temp[0], &diff[0][0], NUM_RAM_REG, listDut);
        
        for (int d = 0; listDut[d] != 0; d++)
        {
            dut = listDut[d] - 1;
            
            if (result[dut] == false)
            {
                sprintf_s(msg, APP_MAX_CHAR_LONGER, "\nMasked %sImage diff Masked DefaultImage [%i]: ", (char*)memory.name, dut);
                strcat_s(msg, APP_MAX_CHAR_LONGER, temp[dut]);
                DBGVerbose(msg);
            }
        }
        
        if (difference != NULL)
            memcpy(difference, diff, sizeof(diff));
    }
    
    // print raw image for 1 DUT
    void Print(int dut)
    {
        String msg;
        String output = B2S(&Raw[0][dut], NUM_RAM_REG);
        sprintf(msg, "\n%sImage[%i]: %s", (char*)memory.name, dut, (char*)output);
        
        // always display
        bool was_on = DBGVerboseEnabled;
        DBGVerboseEnabled = YES;
        DBGVerbose(msg);
        DBGVerboseEnabled = was_on;
    }
    
    // print raw image for all DUTs
    void Print(word* listDut)
    {
        int dut;
        
        for (int d = 0; listDut[d] != 0; d++)
        {
            dut = listDut[d] - 1;
            Print(dut);
        }
    }
    
    // print converted values of RAM registers for 1 DUT
    void PrintConverted(int dut)
    {
        String msg, temp;
        
        // always display
        bool was_on = DBGVerboseEnabled;
        DBGVerboseEnabled = YES;
        
        sprintf(msg, "\n%sImage[%i]: ", (char*)memory.name, dut);
        
        for (int i; i < NUM_RAM_REG; i++)
        {
            sprintf(temp, "%i", Converted[i][dut]);
            strcat(msg, temp);
        }
        
        DBGVerbose(msg);
        
        DBGVerboseEnabled = was_on;
    }
    
    // print out converted values of RAM registers for all DUTs
    void PrintConverted(word* listDut)
    {
        int dut;
        String msg, temp;
        
        // always display
        bool was_on = DBGVerboseEnabled;
        DBGVerboseEnabled = YES;
        
        for (int d = 0; listDut[d] != 0; d++)
        {
            dut = listDut[d] - 1;
            
            sprintf(msg, "\n%sImage[%i]: ", (char*)memory.name, dut);
            
            for (int i = 0; i < NUM_RAM_REG; i++)
            {
                sprintf(temp, "%i", Converted[i][dut]);
                strcat(msg, temp);
            }
            
            DBGVerbose(msg);
        }
        
        DBGVerboseEnabled = was_on;
    }
    
    void View(int dut)
    {
        CTestbench::LVImageView(B2S(&Raw[0][dut], NUM_RAM_REG),page);
    }
    
    void View(Image Img, int dut)
    {
        CTestbench::LVImageView(B2S(&Raw[0][dut], NUM_RAM_REG), B2S(&Img.Raw[0][dut], NUM_RAM_REG), page);
    }
} Image;

//-----------------------------------------------------------------------------
//  Volatile Image struct
//  Used for storing, comparing, and displaying RAM, ROM, and Volatile images
typedef struct VolImage
{
    byte            Raw[MAX_PAGE_SIZE][TOOL_MAX_DUT];
    memory_type     memory;
    byte            page;              // needs to be defined for LV front panel
    int             count;             // number of registers in image
    
    VolImage(void)
    {
        page = PAGE_00;
        memory = Volatile;
        memset(Raw, 0x00, sizeof(Raw));
        count = 0;
    }
    
    VolImage(byte page_in) : page(page_in)
    {
        memory = Volatile;
        memset(Raw, 0x00, sizeof(Raw));
        count = 0;
    }
    
    VolImage(byte page_in, int size) : page(page_in), count(size)
    {
        memory = Volatile;
        memset(Raw, 0x00, sizeof(Raw));
    }
    
    VolImage(byte page_in, byte* array_in, int size) : page(page_in), count(size)
    {
        memory = Volatile;
        memcpy(Raw, array_in, sizeof(Raw));
    }
    
    void SetRaw(byte* raw_in, int size)
    {
        memcpy(Raw, raw_in, sizeof(Raw));
        count = size;
    }
    
    void Compare(VolImage img, byte* difference, bool* result, word* listDut)
    {
        int dut;
        String temp[TOOL_MAX_DUT];
        char msg[APP_MAX_CHAR_LONGER];
        byte diff[MAX_PAGE_SIZE][TOOL_MAX_DUT];
        
        memset(diff, 0x00, sizeof(diff));
        
        for (int i = 0; i < TOOL_MAX_DUT; i++)
            result[i] = true;
        
        for (int d = 0; listDut[d] != 0; d++)
        {
            dut = listDut[d] - 1;
            for (int i = 0; i < count; i++)
            {
                if (Raw[i][dut] != img.Raw[i][dut])
                {
                    result[dut] = false;
                    diff[i][dut] = (Raw[i][dut] ^ img.Raw[i][dut]);
                }
            }
            
            B2SArray(&temp[0], &diff[0][0], count, listDut);
            
            // print difference
            if (result[dut] == false)
            {
                sprintf_s(msg, APP_MAX_CHAR_LONGER, "\n%sImage diff %sImage [%i]: ", (char*)memory.name, (char*)img.memory.name, dut);
                strcat_s(msg, APP_MAX_CHAR_LONGER, temp[dut]);
                DBGVerbose(msg);
            }
        }
        
        if (difference != NULL)
            memcpy(difference, diff, sizeof(diff));
    }
    
    // print raw image for 1 DUT
    void Print(int dut)
    {
        String msg;
        String output = B2S(&Raw[0][dut], count);
        sprintf(msg, "\n%sVolatile Image[%i]: %s", (char*)memory.name, dut, (char*)output);
        
        // always display
        bool was_on = DBGVerboseEnabled;
        DBGVerboseEnabled = YES;
        DBGVerbose(msg);
        DBGVerboseEnabled = was_on;
    }
    
    // print raw image for all DUTs
    void Print(word* listDut)
    {
        int dut;
        
        for (int d = 0; listDut[d] != 0; d++)
        {
            dut = listDut[d] - 1;
            Print(dut);
        }
    }
    
    void View(int dut)
    {
        CTestbench::LVImageView(B2S(&Raw[0][dut], count),page);
    }
    
    void View(Image Img, int dut)
    {
        CTestbench::LVImageView(B2S(&Raw[0][dut], count), B2S(&Img.Raw[0][dut], count), page);
    }
} VolImage;

#endif