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

ThreeWire myWire(4,5,2); // IO/DAT, SCLK/CLK, CE/RST
RtcDS1302<ThreeWire> Rtc(myWire);

// determine the size of an array dynamically
#define countof(a) (sizeof(a) / sizeof(a[0]))

// 30 chars is max, RAM is 31 Bytes, last char is null
const char data[] = "twinkle twinkle little star...";

void setup () 
{
    Serial.begin(9600);

    Serial.print("compiled: ");
    //__DATE__ is a preprocessor macro that expands to current date (at compile time) 
    // in the form mmm dd yyyy (e.g. "Jan 14 2012"), as a string.
    Serial.print(__DATE__);
    Serial.println(__TIME__);
    
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    
    printDateTime(compiled);
    Serial.println();
    
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


/* comment out on a second run to see that the info is stored long term */
    // Store something in memory on the RTC
    uint8_t count = sizeof(data);
    uint8_t written = Rtc.SetMemory((const uint8_t*)data, count); // this includes a null terminator for the string
    if (written != count) 
    {
        Serial.print("something didn't match, count = ");
        Serial.print(count, DEC);
        Serial.print(", written = ");
        Serial.print(written, DEC);
        Serial.println();
    }
/* end of comment out section */
}

void loop () 
{
    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);
    Serial.println(" +");

    if (!now.IsValid())
    {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    delay(5000);

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
    
    delay(5000);
}



void printDateTime(const RtcDateTime& dt)
{
    char datestring[23];

    bool isPM = (dt.Hour() > 12);
    // read the format string from the Flash memory
    snprintf_P(datestring, 
            countof(datestring),
            // PSTR macro = pointer to a string 
            // creating a PROGMEM array and returning its address
            PSTR("%02u/%02u/%04u %02u:%02u:%02u %s"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            //Sutract 12, Exception: 12 AM = 00 in 24 hrs
            isPM ? dt.Hour()-12 : (dt.Hour() == 0 ? 12 : dt.Hour()),
            dt.Minute(),
            dt.Second(),
            isPM ? "PM" : "AM" ); 
    Serial.print(datestring);
}