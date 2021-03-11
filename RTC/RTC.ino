/*
    Real Time Clock Module DS1302

    Displays the current date/time in a 12-hour AM/PM format and checks alarm's date stored in RAM.
    If alarm's time was passed, the alarm will be executed. 
    Simple output to the serial monitor for simplicity. 
    Automatically adjust for Daylight Savings Time and Standard Time for Eastern Time Zone (Toronto, New York, Detroit).

    Connections:
    * CLK/SCLK --> 5
    * DAT/IO --> 4
    * RST/CE --> 2
    * VCC --> 2.0v - 5.5v
    * GND --> GND
*/
#include <ThreeWire.h>  
#include <RtcDS1302.h>

// determine the size of an array dynamically
#define countof(a) (sizeof(a) / sizeof(a[0]))

// RtcDateTime uses Y2K as OriginYear, while time_t uses Epoch 1970 as OriginYear.
// This is the difference between years 2000 and 1970 in seconds.
#define 	UNIX_OFFSET   946684800

const int RST_PIN   = 2;  // Chip Enable
const int IO_PIN   = 4;   // Input/Output
const int SCLK_PIN = 5;   // Serial Clock

ThreeWire myWire(IO_PIN,SCLK_PIN,RST_PIN);
RtcDS1302<ThreeWire> Rtc(myWire);

// 30 chars is max, RAM is 31 Bytes, last char is null.
// Will be used to store the next alarm date+time.
const char data[] = "";

//Is Eastern Daylight Savings Time?
bool isEDT = false;

// Set the current date and time for RTC based on compile time.
// Calculate the next alarm date.
void setup () 
{
    Serial.begin(9600);

    //__DATE__ is a preprocessor macro that expands to current date (at compile time) 
    // in the form mmm dd yyyy (e.g. "Jan 14 2012"), as a string.
    const char* nowDate = __DATE__;
    const char* nowTime = __TIME__;
    
    Rtc.Begin();
    runRtcChecks();

    // Set the date and time.  This only needs to be done once if there is a battery backup
    RtcDateTime compiled = RtcDateTime(nowDate, nowTime);
    Rtc.SetDateTime(compiled);    

    // Eastern Time Zone (Toronto, New York, Detroit)
    // Daylight Saving Time is from Second Sunday in March after 2AM
    // until First Sunday in November until 2AM
    isEDT = isDaylightSavingTime(compiled);
    
    storeAlarmInMemory(compiled);
}

// Print current time every 5 seconds. 
// If current time passed the alarm time, execute Alarm and set the next alarm's date+time.
void loop () 
{
    RtcDateTime now = Rtc.GetDateTime();
    RtcDateTime nextAlarm = readAlarmFromMemory();

    if (!now.IsValid())
    {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }
    else 
    {
        // Adjust for DaylightSavingTime/Standard Time, Eastern Time Zone
        if (now.Month() == 3 && !isEDT && isDaylightSavingTime(now))
        {
            // from EST to EDT (+ 1 hour)
            now+=3600;
            Rtc.SetDateTime(now);

            nextAlarm+=3600;
            storeAlarmInMemory(nextAlarm);

            isEDT = true;
        }
        else if (now.Month() == 11 && isEDT && !isDaylightSavingTime(now)) 
        {
            // from EDT to EST (- 1 hour)
            now-=3600;
            Rtc.SetDateTime(now);

            nextAlarm-=3600;
            storeAlarmInMemory(nextAlarm);
        
            isEDT = false;
        }
    
        // debug
        Serial.print("Next Alarm: ");
        printDateTime(nextAlarm);

        // execute alarm
        if (now >= nextAlarm){
            Serial.println("!!!!  ALARM  !!!!");
            storeAlarmInMemory(nextAlarm);
        }

        // debug
        Serial.print("Time now:   ");
        printDateTime(now);
    }

    delay(5000);
}

// Print date+time in a 12-hour AM/PM format.
void printDateTime(const RtcDateTime& dt)
{
    char datestring[30];

    bool isPM = (dt.Hour() > 12);
    // read the format string from the Flash memory
    snprintf_P(datestring, 
            countof(datestring),
            // PSTR macro = pointer to a string 
            // creating a PROGMEM array and returning its address
            PSTR("%02u %s %04u %02u:%02u:%02u %s %s %s"),
            dt.Day(),
            monthShortString(dt.Month()),
            dt.Year(),
            //Sutract 12, Exception: 12 AM = 00 in 24 hrs
            isPM ? dt.Hour()-12 : (dt.Hour() == 0 ? 12 : dt.Hour()),
            dt.Minute(),
            dt.Second(),
            isPM ? "PM" : "AM", 
            "+", 
            isEDT ? "EDT":"EST"); // Eastern Daylight Time/Eastern Standard Time
    Serial.println(datestring);
}

// Initialize chip by turning off write protection and clearing the clock halt flag.
void runRtcChecks()
{
    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }
}

// Calculate the next alarm's date and store in memory.
void storeAlarmInMemory(RtcDateTime lastAlarm) 
{
    // Calculate next alarm date 
    // by rounding off to the last minute
    RtcDateTime nextAlarm = RtcDateTime(lastAlarm.Year(), lastAlarm.Month(), lastAlarm.Day(), lastAlarm.Hour(), lastAlarm.Minute(), 0);
    // then adding +1 min
    nextAlarm = RtcDateTime(nextAlarm.Epoch64Time()-UNIX_OFFSET + 60);
    char datestring[28];

    // read the format string from the Flash memory
    snprintf_P(datestring, 
            countof(datestring),
            // PSTR macro = pointer to a string 
            // creating a PROGMEM array and returning its address
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            nextAlarm.Day(),
            nextAlarm.Month(),
            nextAlarm.Year(),
            nextAlarm.Hour(),
            nextAlarm.Minute(),
            nextAlarm.Second()); 

    // Store next alarm in memory on the RTC
    uint8_t _count = sizeof(datestring);
    uint8_t written = Rtc.SetMemory((const uint8_t*)datestring, _count); // this includes a null terminator for the string
    if (written != _count) 
    {
        Serial.print("something didn't match, count = ");
        Serial.print(_count, DEC);
        Serial.print(", written = ");
        Serial.print(written, DEC);
        Serial.println();
    }
}

RtcDateTime readAlarmFromMemory() {
    // read data
    uint8_t buff[31];
    char buffy[31];

    const uint8_t count = sizeof(buff);
    // get our data
    uint8_t gotten = Rtc.GetMemory(buff, count);

    if (gotten != count) 
    {
        Serial.print("something didn't match, count = ");
        Serial.print(count, DEC);
        Serial.print(", gotten = ");
        Serial.print(gotten, DEC);
        Serial.println();
    }

    // print the string, but terminate if we get a null
    for (uint8_t ch = 0; ch < gotten && buff[ch]; ch++)
    {
        buffy[ch] = buff[ch];
    }

    uint16_t day, month, year, hour, minute, second;
    sscanf(buffy, "%02u/%02u/%04u %02u:%02u:%02u",
    &day,
    &month,
    &year, 
    &hour, 
    &minute,
    &second);

    RtcDateTime nextAlarm = RtcDateTime(year, month, day, hour, minute, second);
    return nextAlarm;
}

const char* monthShortString(uint8_t mth) {
    
    // Initialize array of pointer
    const char *months[13] = { "Nil", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    return months[mth];
}

// Adapted from https://github.com/SensorsIot/NTPtimeESP/blob/master/NTPtimeESP.cpp
boolean isDaylightSavingTime(RtcDateTime _tempDateTime) {
	// DST begins on second Sunday of March and ends on first Sunday of November. 
	// Time change occurs at 2AM locally
	if (_tempDateTime.Month() < 3 || _tempDateTime.Month() > 11) return false;  //January, february, and december are out.
	if (_tempDateTime.Month() > 3 && _tempDateTime.Month() < 11) return true;   //April to October are in
	int previousSunday = _tempDateTime.Day() - (_tempDateTime.DayOfWeek());  
	
    // -------------------- March ---------------------------------------
	//In march, we are DST if our previous Sunday was = to or after the 8th.
	if (_tempDateTime.Month() == 3 ) {  // in march, if previous Sunday is after the 8th, is DST
		// unless Sunday and hour < 2am
		if ( previousSunday >= 8 ) { 
			// return true if day > 14 or (dow == 0 and hour >= 2)
			return ((_tempDateTime.Day() > 14) || 
			((_tempDateTime.DayOfWeek() == 0 && _tempDateTime.Hour() >= 2) || _tempDateTime.DayOfWeek() > 0));
		}
		else
		{
			// previousSunday has to be < 8 to get here
			return false;
		} 
	} 

	// ------------------------------- November -------------------------------
	// gets here only if month = November
	//In november we must be before the first Sunday to be dst.
	//That means the previous Sunday must be before the 2nd.
	if (previousSunday < 1)
	{
		// is not true for Sunday after 2am or any day after 1st Sunday any time
		return ((_tempDateTime.DayOfWeek() == 0 && _tempDateTime.Hour() < 2) || (_tempDateTime.DayOfWeek() > 0));
	} 
	else
	{
		// return false unless after first wk and dow = Sunday and hour < 2
		return (_tempDateTime.Day() <8 && _tempDateTime.DayOfWeek() == 0 && _tempDateTime.Hour() < 2);
	}  
} 