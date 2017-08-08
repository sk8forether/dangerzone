using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Controls;

namespace AvgTurnCalcProject
{
    /// <summary>
    /// A Row is a collection of the information for a single RMA, as well as "flags" (boolean descriptors)
    /// The combined and duplicate flags are used when several Rows are collected in a List
    /// </summary>
    
    class Row
    {
        public string RMANumber;                                               //ERP and RMA
        public int Line;                                                       //ERP and RMA
        public string Status;                                                  //ERP
        public DateTime DateCreated;                                           //ERP
        public DateTime DateReceived;                                          //ERP
        public DateTime DateInspected;                                         //ERP
        public DateTime DateFloor;                                             //RMA
        public DateTime DateEngSent;                                           //RMA
        public DateTime DateEngReceived;                                       //RMA
        public DateTime DateShip;                                              //RMA
        public bool combined;                                                  //Says whether ERP and RMA have been combined for this row
        public bool duplicate;                                                 //There is a duplicate RMA # and Line in db

        //Prints the row to a textbox: this is mainly for testing and troubleshooting
        public void print_Row(TextBox textBox1)
        {
            string empty = "1/1/0001 12:00:00 AM";                             //This is what a "null" DateTime will output

            textBox1.Text += this.RMANumber + " Line " + this.Line + " Status " + this.Status + " Created ";
            if (this.DateCreated.ToString() != empty) 
            {
                textBox1.Text += this.DateCreated; 
            }
            textBox1.Text += " Recieved ";
            if (this.DateReceived.ToString() != empty) 
            { 
                textBox1.Text += this.DateReceived; 
            }
            textBox1.Text += " Inspected ";
            if (this.DateInspected.ToString() != empty) 
            { 
                textBox1.Text += this.DateInspected; 
            }
            textBox1.Text += " Floor ";
            if (this.DateFloor.ToString() != empty) 
            { 
                textBox1.Text += this.DateFloor; 
            }
            textBox1.Text += " Eng Sent";
            if (this.DateEngSent.ToString() != empty) 
            { 
                textBox1.Text += this.DateEngSent; 
            }
            textBox1.Text += " Eng Rec ";
            if (this.DateEngReceived.ToString() != empty) 
            { 
                textBox1.Text += this.DateEngReceived; 
            }
            textBox1.Text += " Ship ";
            if (this.DateShip.ToString() != empty) 
            { 
                textBox1.Text += this.DateShip; 
            }
            if (duplicate) 
            { 
                textBox1.Text += "\n This is a duplicate."; 
            }
        }

        //If the RMANumber and Line match, then collect all the data for this Row.  
        //Use this on an ERP Row to add an RMA Row's info.
        //Assumes all ERP data already obtained.
        public Row combine_Row(Row Row2)
        {
            if (this.RMANumber == Row2.RMANumber && this.Line == Row2.Line)    //These should be combined, otherwise returns original row
            {
                this.DateFloor = Row2.DateFloor;
                this.DateEngSent = Row2.DateEngSent;
                this.DateEngReceived = Row2.DateEngReceived;
                this.DateShip = Row2.DateShip;
                Row2.combined = true;
            }
            return this;
        }
    }
}
