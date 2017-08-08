using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;

namespace AvgTurnCalcProject
{
    /// <summary>
    /// This class defines all the necessary functions for calculating and using business days
    /// Weekends and holidays have to be eliminated from possible business days
    /// This particular case has 11 holidays each year, and sometimes they change
    /// This code is only current for 2015, and may have to be updated each year be exact
    /// </summary>
    
    class BusinessDays
    {
        //New function that takes in 2 dates and calculates the number of business days between them
        public int GetBusinessDays(DateTime start, DateTime stop)
        {
            int total = 0;
            stop = stop.AddDays(1);
            if (start > stop)
            {
                MessageBox.Show("There are incorrect dates in the database for RMAs in this date range.  This may adversely affect turntimes.", "Error!");
                return (total = 0);
            }
            DateTime calc = start;
            do
            {
                if (IsDateValid(calc))                                         //  *TODO: This section takes longer than I would like.  Fix it.*
                {
                    total++;
                }
                calc = calc.AddDays(1);
            }
            while (calc != stop);
            return total;
        }

        //+++++++++++++++++++++ Begin code taken (and edited) from ECOManager - Util.cs ++++++++++++++++++++++++++
        //Changes I made: Added day after New Year's, added President's Day, took out Good Friday, and added Columbus Day.
        //There will likely be differences in each year's holidays.  Can be tweaked when holidays are announced.

        public DateTime AddBusinessDays(DateTime date, int numDays)
        {
            DateTime retVal = date;
            if (numDays > 0)
            {
                for (int i = 0; i < numDays; i++)
                {
                    do
                    {
                        retVal = retVal.AddDays(1);
                    }
                    while (!IsDateValid(retVal));
                }
            }
            else
            {
                for (int i = 0; i > numDays; i--)
                {
                    do
                    {
                        retVal = retVal.AddDays(-1);
                    }
                    while (!IsDateValid(retVal));
                }
            }
            return retVal;
        }

        /// <summary>
        /// This was pulled from an online code snippet.
        /// We're just going to trust that it works.
        /// </summary>
        /// <param name="y">The year.</param>
        /// <returns>The date of Easter for the specified year.</returns>
        private DateTime GetEaster(int y)
        {
            int a = y % 19;
            int b = y / 100;
            int c = y % 100;
            int d = b / 4;
            int e = b % 4;
            int f = (b + 8) / 25;
            int g = (b - f + 1) / 3;
            int h = (19 * a + b - d - g + 15) % 30;
            int i = c / 4;
            int k = c % 4;
            int l = (32 + 2 * e + 2 * i - h - k) % 7;
            int m = (a + 11 * h + 22 * l) / 451;
            int easterMonth = (h + l - 7 * m + 114) / 31;
            int p = (h + l - 7 * m + 114) % 31;
            int easterDay = p + 1;

            return new DateTime(y, easterMonth, easterDay);
        }

        private bool IsDateValid(DateTime date)
        {
            bool valid = true;
            DateTime holiday;

            // Eliminate Saturday and Sunday
            valid &= date.DayOfWeek != DayOfWeek.Saturday;
            valid &= date.DayOfWeek != DayOfWeek.Sunday;
            if (!valid)
                return false;

            switch (date.Month)
            {
                case 1:
                    // Eliminate New Years and day after when it is a Friday
                    //This will probably have to be tweaked each year depending on what they decide we get off.
                    holiday = new DateTime(date.Year, 1, 1);
                    while (holiday.DayOfWeek == DayOfWeek.Saturday || holiday.DayOfWeek == DayOfWeek.Sunday)
                    {
                        holiday = holiday.AddDays(1);
                    }
                    if (date.Day == holiday.Day)
                    {
                        return false;
                    }
                    else if (date.Day == holiday.Day + 1 && holiday.DayOfWeek == DayOfWeek.Thursday)
                    {
                        return false;
                    }
                    break;
                case 2:
                    // Eliminate President's Day
                    //Third Monday of February
                    holiday = new DateTime(date.Year, 2, 1);
                    int n = 0;
                    while (n < 3)
                    {
                        while (holiday.DayOfWeek != DayOfWeek.Monday)
                        {
                            holiday = holiday.AddDays(1);
                        }
                        n++;
                        holiday = holiday.AddDays(1);
                    }
                    holiday = holiday.AddDays(-1);
                    if (date.Day == holiday.Day)
                    {
                        return false;
                    }
                    break;
                //case 3:
                //case 4:
                //    // Eliminate Good Friday
                //    holiday = GetEaster(date.Year);
                //    while (holiday.DayOfWeek == DayOfWeek.Saturday || holiday.DayOfWeek == DayOfWeek.Sunday)
                //    {
                //        holiday = holiday.AddDays(-1);
                //    }
                //    if (date.Month == holiday.Month && date.Day == holiday.Day)
                //    {
                //        return false;
                //    }
                //    break;
                case 5:
                    // Eliminate Memorial Day
                    //Last Monday in May
                    holiday = new DateTime(date.Year, 5, 31);
                    while (holiday.DayOfWeek != DayOfWeek.Monday)
                    {
                        holiday = holiday.AddDays(-1);
                    }
                    if (date.Day == holiday.Day)
                    {
                        return false;
                    }
                    break;
                case 7:
                    // Eliminate Independence Day (or a day near it)
                    //This may change if the 4th is on a weekend, they could give Friday or Monday off.
                    holiday = new DateTime(date.Year, 7, 4);
                    if (holiday.DayOfWeek == DayOfWeek.Saturday)
                    {
                        holiday = holiday.AddDays(2);
                    }
                    else if (holiday.DayOfWeek == DayOfWeek.Sunday)
                    {
                        holiday = holiday.AddDays(1);
                    }
                    if (date.Day == holiday.Day)
                    {
                        return false;
                    }
                    break;
                case 9:
                    // Eliminate Labor Day
                    //First Monday in September
                    holiday = new DateTime(date.Year, 9, 1);
                    while (holiday.DayOfWeek != DayOfWeek.Monday)
                    {
                        holiday = holiday.AddDays(1);
                    }
                    if (date.Day == holiday.Day)
                    {
                        return false;
                    }
                    break;
                case 10:
                    // Eliminate Columbus Day
                    //Happens on Monday near October 12th
                    holiday = new DateTime(date.Year, 10, 12);
                    if (holiday.DayOfWeek == DayOfWeek.Saturday)
                    {
                        holiday = holiday.AddDays(2);
                    }
                    else if (holiday.DayOfWeek == DayOfWeek.Sunday)
                    {
                        holiday = holiday.AddDays(1);
                    }
                    else
                    {
                        while (holiday.DayOfWeek != DayOfWeek.Monday)
                        {
                            holiday = holiday.AddDays(-1);
                        }
                    }
                    if (date.Day == holiday.Day)
                    {
                        return false;
                    }
                    break;
                case 11:
                    // Eliminate Thanksgiving AND Black Friday (day after Thanksgiving)
                    holiday = new DateTime(date.Year, 11, 1);
                    while (holiday.DayOfWeek != DayOfWeek.Thursday)
                    {
                        holiday = holiday.AddDays(1);
                    }
                    holiday = holiday.AddDays(21); // 4th Thursday
                    if (date.Day == holiday.Day)
                    {
                        return false;
                    }
                    else if (date.Day == holiday.Day + 1)
                    {
                        return false;
                    }
                    break;
                case 12:
                    // Eliminate Christmas AND Christmas Eve
                    holiday = new DateTime(date.Year, 12, 24);
                    if (holiday.DayOfWeek == DayOfWeek.Friday)
                    {
                        if (date.Day == holiday.Day || date.Day == holiday.Day + 3)
                        {
                            return false;
                        }
                    }
                    else if (holiday.DayOfWeek == DayOfWeek.Saturday)
                    {
                        if (date.Day == holiday.Day + 2 || date.Day == holiday.Day + 3)
                        {
                            return false;
                        }
                    }
                    else if (holiday.DayOfWeek == DayOfWeek.Sunday)
                    {
                        if (date.Day == holiday.Day + 1 || date.Day == holiday.Day + 2)
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (date.Day == holiday.Day || date.Day == holiday.Day + 1)
                        {
                            return false;
                        }
                    }
                    break;

                default:
                    break;
            }
            return true;
        }

        //+++++++++++++++++++End code taken from ECOManager - Util.cs++++++++++++++++++++++++++++++
    }
}
