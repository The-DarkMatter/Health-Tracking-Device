#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <string.h>
#include <math.h>
 
#define SIM800_TX_PIN 8
#define SIM800_RX_PIN 7
 
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);
 
TinyGPS gps;
SoftwareSerial ss(2, 3);
void setup() {
 
  Serial.begin(9600);
 
  ss.begin(9600);
 
 
  Serial.println("Patient Tracking system");
  Serial.println();
 
}
 
// reverses a string 'str' of length 'len'
void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}
 
 // Converts a given integer x to string str[].  d is the number
 // of digits required in output. If d is more than the number
 // of digits in x, then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }
 
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
 
    reverse(str, i);
    str[i] = '\0';
    return i;
}
 
// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;
 
    // Extract floating part
    float fpart = n - (float)ipart;
 
    // convert integer part to string
    int i = intToStr(ipart, res, 0);
 
    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot
 
        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);
 
        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}
 
void loop() {
  //GPS data fetching loop
   bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
 
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }
 
  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat);
    Serial.print(" LON=");
    Serial.println(flon);
    /*Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.println(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());*/
 
    //Sending SMS to Amogh Upadhyay
 
    while(!Serial);
   
    //Being serial communication witj Arduino and SIM800
    serialSIM800.begin(9600);
    delay(1000);
   
    Serial.println("Setup Complete!");
    Serial.println("Sending SMS...");
     
    //Set SMS format to ASCII
    serialSIM800.write("AT+CMGF=1\r\n");
    delay(1000);
   
    //Send new SMS command and message number
    serialSIM800.write("AT+CMGS=\"8989536268\"\r\n");
    delay(1000);
 
    //Send SMS content
    //String lat = toString(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    //String lon = toString(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
//    
//    serialSIM800.write(flat);
//   
//    serialSIM800.write(flon);
 
    char flat_converted[20];
    char flon_converted[20];
    ftoa(flat, flat_converted, 6);
    ftoa(flon, flon_converted, 6);
    Serial.println("Latitude: ");
    Serial.println(flat_converted);
    Serial.println("\n");
    Serial.println("Longitude: ");
    Serial.println(flon_converted);
    serialSIM800.write("Alert From Patient Tracking System \n");
    serialSIM800.write("Patient Location has been recorded at:- \n");
    serialSIM800.write("Latitude: ");
    serialSIM800.write(flat_converted);
    serialSIM800.write("\n");
    serialSIM800.write("Longitude: ");
    serialSIM800.write(flon_converted);
   
   
    delay(1000);
    //Send Ctrl+Z / ESC to denote SMS message is complete
    serialSIM800.write((char)26);
    delay(1000);
         
    Serial.println("SMS Sent!");
   
    delay(60000);
  }
}
