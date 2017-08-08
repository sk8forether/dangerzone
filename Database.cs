using System;
using System.Collections.Generic;
using System.Windows;
using System.Data.SqlClient;

namespace AvgTurnCalcProject
{
    /// <summary>
    /// ERP data queried first for Closed RMAs, then RMATracking queried to fill in all other available dates
    /// Duplicates in ERP db that are not present in RMATracking db are filled in
    /// A table is created by a list of Rows (see Row.cs)
    /// Once table is complete, the table is filtered down to the RMAs that were closed during the selected dates on calendars
    /// Then calculations can be done for overall turntime, and for turntime in certain departments
    /// The results are displayed to the user in MainWindow.xaml.cs
    /// </summary>

    class Database
    {
        private List<Row> Table;                                               //see Row.cs for Row class
        private int nEntered = 0;
        private int nReceived = 0;
        private int nEngineering = 0;
        private int nInspected = 0;
        private int nFloor = 0;
        private int nAverage = 0;
        private int nTotal = 0;
        private int nTotRec = 0;
        private int nTotEng = 0;
        private int nTotFloor = 0;
        BusinessDays bd = new BusinessDays();                                  //see BusinessDays.cs

        //Constructor
        public Database()
        {
            Table = Get_Table();
        }

        //Calculates average turntime for RMAs that were closed within selected date range
        public int[] Calculate(DateTime start, DateTime stop, int[] x)
        {
            Table = Filter(start, stop);
            nTotal = Table.Count;                                              //Total RMAs in this time
            if (nTotal == 0)                                                   //Shouldn't happen, but just in case
            {
                return x;
            }
            DateTime empty = new DateTime();

            //Loop through all closed RMAs
            for (int i = 0; i < Table.Count; i++)
            {
                //Entered to Received
                nEntered += bd.GetBusinessDays(Table[i].DateCreated, Table[i].DateReceived);

                //Received to Inspected
                if (Table[i].DateInspected != empty)
                {
                    nReceived += bd.GetBusinessDays(Table[i].DateReceived, Table[i].DateInspected);
                    nTotRec++;
                }

                //Time in Engineering
                if (Table[i].DateEngSent != empty && Table[i].DateEngReceived != empty)
                {
                    nEngineering += bd.GetBusinessDays(Table[i].DateEngSent, Table[i].DateEngReceived);
                    nTotEng++;
                }

                //Inspected to Floor
                if (Table[i].DateFloor != empty && Table[i].DateInspected != empty)
                {
                    nInspected += bd.GetBusinessDays(Table[i].DateInspected, Table[i].DateFloor);
                    nTotFloor++;
                }

                //Floor to Close
                if (Table[i].DateFloor != empty)
                {
                    nFloor += bd.GetBusinessDays(Table[i].DateFloor, Table[i].DateShip);
                }

                //Received to Close
                nAverage += bd.GetBusinessDays(Table[i].DateReceived, Table[i].DateShip);
            }

            //Collect results
            x[0] = nEntered / nTotal;                                          //Average time from Entered to Received
            x[1] = nReceived / nTotRec;                                        //Average time from Received to Inspected
            if (nTotEng != 0)
            {
                x[2] = nEngineering / nTotEng;                                 //Average time in Engineering
            }
            if (nTotFloor != 0)
            {
                x[3] = nInspected / nTotFloor;                                 //Average time from Inspected to Floor
                x[4] = nFloor / nTotFloor;                                     //Average time from Floor to Close
            }
            x[5] = nAverage / nTotal;                                          //Total Average time RMAs were in building
            x[6] = nTotal;                                                     //Total number of RMAs in timeframe
            return x;
        }

        //Connects to several tables in iERP85_JADAK db to get RMA info
        private List<Row> Update_ERPTable()
        {
            List<Row> ERPTable = new List<Row>();

            string strConnect = null;                                          //Code for connecting to database here
            SqlConnection cnn;
            SqlCommand command;
            string sql = null;
            SqlDataReader dataReader;
            
            strConnect = "Data Source=servername;Initial Catalog=server_stuff;Integrated Security=True";
            sql = @"SELECT Table.Field, Table.Fiel
                    FROM   Database.Table
                    WHERE  Table.Field IS NOT NULL
                    ORDER BY Table.Field2";
            cnn = new SqlConnection(strConnect);
            try
            {
                cnn.Open();
                command = new SqlCommand(sql, cnn);
                dataReader = command.ExecuteReader();
                while (dataReader.Read())
                {                                                              //Fill in a row
                    Row temp = new Row();
                    temp.RMANumber = dataReader["RMA_RMANumber"].ToString();
                    if (!dataReader.IsDBNull(1)) { temp.Line = dataReader.GetInt32(1); }  //Line #
                    if (!dataReader.IsDBNull(2)) { temp.Status = dataReader["RMA_Status"].ToString(); }
                    if (!dataReader.IsDBNull(3)) { if (DateTime.TryParse(dataReader["RMA_DateCreated"].ToString(), out temp.DateCreated)) { } }
                    if (!dataReader.IsDBNull(4)) { if (DateTime.TryParse(dataReader["RMAR_ReceiptDate"].ToString(), out temp.DateReceived)) { } }
                    if (!dataReader.IsDBNull(5)) { if (DateTime.TryParse(dataReader["RMAI_InspectionDate"].ToString(), out temp.DateInspected)) { } }

                    ERPTable.Add(temp);                                        //Add row to the table to store
                }
                dataReader.Close();
                command.Dispose();
                cnn.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error connecting to Database", "Error");
            }
            return ERPTable;
        }

        //Connects to RMATracking table in production db to get RMA info
        private List<Row> Update_RMATable()
        {
            List<Row> RMATable = new List<Row>();
            string strConnect = null;                                          //Code for connecting to database here
            SqlConnection cnn;
            SqlCommand command;
            string sql = null;
            SqlDataReader dataReader;
            strConnect = "Data Source=servername;Initial Catalog=server_stuff;Integrated Security=True";
            sql = @"SELECT Table.Field, Table.Fiel
                    FROM   Database.Table
                    WHERE  Table.Field IS NOT NULL
                    ORDER BY Table.Field2";
            cnn = new SqlConnection(strConnect);
            try
            {
                cnn.Open();
                command = new SqlCommand(sql, cnn);
                dataReader = command.ExecuteReader();
                while (dataReader.Read())
                {                                                              //Fill in a row
                    Row temp = new Row();
                    temp.RMANumber = dataReader["RMANumber"].ToString();
                    if (!dataReader.IsDBNull(1)) { temp.Line = dataReader.GetInt32(1); }  //Line #
                    if (!dataReader.IsDBNull(2)) { if (DateTime.TryParse(dataReader["SentToFloor"].ToString(), out temp.DateFloor)) { } }
                    if (!dataReader.IsDBNull(3)) { if (DateTime.TryParse(dataReader["SentToEngineering"].ToString(), out temp.DateEngSent)) { } }
                    if (!dataReader.IsDBNull(4)) { if (DateTime.TryParse(dataReader["ReceivedFromEngineering"].ToString(), out temp.DateEngReceived)) { } }
                    if (!dataReader.IsDBNull(5)) { if (DateTime.TryParse(dataReader["ShipDate"].ToString(), out temp.DateShip)) { } }
                    RMATable.Add(temp);                                        //Add row to the table to store
                }
                dataReader.Close();
                command.Dispose();
                cnn.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error connecting to Database", "Error");
            }
            return RMATable;
        }

        //Sets up a Table from the ERP db of RMAs, collects the duplicates, 
        //matches the duplicates, then combines the duplicates and non-duplicates 
        //with their respective matches in the Table
        private List<Row> Get_Table()
        {
            List<Row> RMATable = new List<Row>();                              //Initialize Tables and get data
            RMATable = Update_RMATable();                                      //Get data from RMATracking database
            Table = Update_ERPTable();                                         //Get data from Intuitive ERP database into Table first
            DateTime Socks = new DateTime(2015, 2, 18);                        //Date that Sox Compliance began affecting db
            List<int> index = new List<int>();

            List<Row> Duplicates = new List<Row>();                            //Check for duplicates in ERP db
            for (int i = 0; i < Table.Count; i++)
            {
                for (int j = 0; j < Table.Count; j++)
                {
                    if (i != j && Table[i].RMANumber == Table[j].RMANumber && Table[i].Line == Table[j].Line && (!Duplicates.Contains(Table[i])) && (!Duplicates.Contains(Table[j])))
                    {
                        Table[i].duplicate = true; Table[j].duplicate = true;
                        Duplicates.Add(Table[i]); Duplicates.Add(Table[j]);

                        index.Add(i); index.Add(j);                            //Store indicies for original table
                    }
                }
            }
            List<Row> Matches = new List<Row>();                               //Find matches in RMA db for duplicates in ERP
            for (int k = 0; k < Duplicates.Count; k++)
            {
                int count = 0;
                for (int m = 0; m < RMATable.Count; m++)
                {
                    if (Duplicates[k].RMANumber == RMATable[m].RMANumber && Duplicates[k].Line == RMATable[m].Line)
                    {
                        if (!Matches.Contains(RMATable[m]))
                        {
                            Matches.Add(RMATable[m]);
                            count++;
                        }
                        else if (DateTime.Compare(Duplicates[k].DateCreated, Socks) >= 0 && count < 1)
                        {                                                      //Some duplicates in ERP are not present in RMA db after Sox compliance date
                            Row dup = Duplicates[k];                           //so this conditional accounts for mimicking them in RMA db
                            RMATable.Add(dup);                                 //  *TODO: turn this into a SQL update to add them to the RMA db permanently*
                            Matches.Add(dup);
                            count++;
                        }
                    }
                }
            }
            int p = 0;                                                         //For iterating through Matches table while inside Table loop
            for (int n = 0; n < Table.Count; n++)                              //This loop combines duplicates and non-duplicates
            {
                if (index.Contains(n))                                         //Combine duplicates here
                {
                    Table[n].combine_Row(Matches[p]);
                    n++; p++;
                    Table[n].combine_Row(Matches[p]);
                    p++;
                }
                else                                                           //Match and combine non-duplicates here
                {
                    Row use = new Row();
                    use = RMATable.Find(q => (q.RMANumber.Contains(Table[n].RMANumber) && (q.Line == Table[n].Line)));
                    if (use != null)
                    {
                        Table[n].combine_Row(use);
                    }
                }
            }
            return Table;
        }

        //Filters table down to only RMAs in selected date range
        private List<Row> Filter(DateTime start, DateTime stop)
        {
            List<Row> NewTable = new List<Row>();
            for (int i = 0; i < Table.Count; i++)
            {
                if (start <= Table[i].DateShip && Table[i].DateShip <= stop)
                {
                    NewTable.Add(Table[i]);
                }
            }
            return NewTable;
        }
    }
}
