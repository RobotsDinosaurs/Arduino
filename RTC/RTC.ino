/*
    Real Time Clock Module DS1302

    Displays date/time and RAM value on a serial monitor. 

    Connections:
    * CLK/SCLK --> 5
    * DAT/IO --> 4
    * RST/CE --> 2
    * VCC --> 2.0v - 5.5v
    * GND --> GND
*/
#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include <Timezone.h>

const int RST_PIN   = 2;  // Chip Enable
const int IO_PIN   = 4;   // Input/Output
const int SCLK_PIN = 5;   // Serial Clock

ThreeWire myWire(IO_PIN,SCLK_PIN,RST_PIN);
RtcDS1302<ThreeWire> Rtc(myWire);

// determine the size of an array dynamically
#define countof(a) (sizeof(a) / sizeof(a[0]))

// 30 chars is max, RAM is 31 Bytes, last char is null
const char data[] = "twinkle twinkle little star...";

// RtcDateTime uses Y2K as OriginYear, while time_t uses Epoch 1970 as OriginYear
// this is the difference between 2000 and 1970
#define 	UNIX_OFFSET   946684800

// US Eastern Time Zone (Toronto, New York, Detroit)
TimeChangeRule myEDT = {"EDT", Second, Sun, Mar, 2, -240};    // Daylight time = UTC - 4 hours (Summer time)
TimeChangeRule myEST = {"EST", First, Sun, Nov, 2, -300};     // Standard time = UTC - 5 hours
Timezone myTZ(myEDT, myEST);

// If TimeChangeRules are already stored in EEPROM, comment out the three
// lines above and uncomment the line below.
//Timezone myTZ(100);       // assumes rules stored at EEPROM address 100

TimeChangeRule *tcr;        // pointer to the time change rule, use to get TZ abbrev

void setup () 
{
    Serial.begin(9600);

    //Serial.print("compiled: ");
    //__DATE__ is a preprocessor macro that expands to current date (at compile time) 
    // in the form mmm dd yyyy (e.g. "Jan 14 2012"), as a string.
    const char* nowDate = __DATE__;
    const char* nowTime = __TIME__;
    
    //Serial.print(nowDate);
    //Serial.println(nowTime);
    
  // Set the date and time.  This only needs to be done once if there is a battery backup
  // Format (Year, Month, Day, Hour (24 Format), Minute, Second, Day of Week)
  // Update the following line to current date time the first time it is run then comment out
  //Time t(2018, 7, 9, 16, 02, 00, Time::kMonday);
  //rtc.time(t);
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(nowDate, nowTime);
    printDateTime(compiled, "AAA");
    //Serial.println();

    //setTime(myTZ.toUTC(compiled.Epoch64Time()-UNIX_OFFSET));

    runRtcChecks(compiled);
    
/* comment out on a second run to see that the info is stored long term */
    storeDataInMemory(data, sizeof(data));
/* end of comment out section */
}

void loop () 
{
    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now, "AAA");

    if (!now.IsValid())
    {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    time_t utc = myTZ.toUTC(now.Epoch64Time() - UNIX_OFFSET);
    time_t local = myTZ.toLocal(utc, &tcr);
    Serial.println();
    printDateTime(utc, "UTC");
    printDateTime(local, tcr -> abbrev);
  
    delay(5000);

    readDataFromMemory();
    
    delay(5000);
}

// format and print a time_t value, with a time zone appended.
/*void printDateTimeTZ(time_t t, const char *tz)
{
    char buf[32];
    //Serial.println(buf);
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    //Serial.println(m);
    strcpy(m, monthShortStr(month(t)));
    Serial.println(t);
    Serial.println(month(t));
    Serial.println(day(t));
    Serial.println(m);
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
        hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
    Serial.println(buf);
}*/

void printDateTime(const RtcDateTime& dt, const char format[4])
{
    char datestring[28];

    bool isPM = (dt.Hour() > 12);
    // read the format string from the Flash memory
    snprintf_P(datestring, 
            countof(datestring),
            // PSTR macro = pointer to a string 
            // creating a PROGMEM array and returning its address
            PSTR("%02u:%02u:%02u %s %02u %s %04u %s"),
            //Sutract 12, Exception: 12 AM = 00 in 24 hrs
            isPM ? dt.Hour()-12 : (dt.Hour() == 0 ? 12 : dt.Hour()),
            dt.Minute(),
            dt.Second(),
            isPM ? "PM" : "AM", 
            dt.Day(),
            monthShortStr(dt.Month()),
            dt.Year(), 
            format); 
    Serial.println(datestring);
}

void runRtcChecks(RtcDateTime compiled)
{
     // Initialize chip by turning off write protection and clearing the clock halt flag.
    //Rtc.writeProtect(false);
    //Rtc.halt(false);
    if (!Rtc.IsDateTimeValid()) 
    {
        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

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

    RtcDateTime now = Rtc.GetDateTime(); // always gets 24-hour format
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
}

void storeDataInMemory(const char _data[], uint8_t _count) 
{
    // Store something in memory on the RTC
    //uint8_t count = sizeof(_data);
    uint8_t written = Rtc.SetMemory((const uint8_t*)_data, _count); // this includes a null terminator for the string
    if (written != _count) 
    {
        Serial.print("something didn't match, count = ");
        Serial.print(_count, DEC);
        Serial.print(", written = ");
        Serial.print(written, DEC);
        Serial.println();
    }
}

void readDataFromMemory() {
    // read data
    uint8_t buff[31];
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

    Serial.print("data read (");
    Serial.print(gotten);
    Serial.print(") = \"");
    // print the string, but terminate if we get a null
    for (uint8_t ch = 0; ch < gotten && buff[ch]; ch++)
    {
        Serial.print((char)buff[ch]);
    }
    Serial.println("\"");    
}

const char* monthShortString(uint8_t mth) {
    
    // Initialize array of pointer
    const char *months[13] = { "", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    Serial.println(mth);
    Serial.println(months[mth]);
    return months[mth];
}