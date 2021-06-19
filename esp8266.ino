#define SECRET_SSID "LAPTOP-BUJQDF9Q 7015"    // replace MySSID with your WiFi network name
#define SECRET_PASS "dhia1998" // replace MyPassword with your WiFi password

#define SECRET_CH_ID 1412129     // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "TPTZSQY6KPS8DLWA" // replace XYZ with your channel write API Key




#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
char auth[] = " cba0tTPNqKj5hOn7tdZqF9xNWzAaus5u" ; // auth for blynk 
char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

/* holds the current serial line */
String currentLine;

int lastpresencedelapiece=1,lastpositionbas=1,lastpositionhaut=1,laststock=1;
unsigned long lastSendTime = 0;
//send every 15 seconds (rate limit by ThingSpeak, 15 seconds per field)
const long sendInterval = 15 * 1000;

void read_serial_packet();
void send_to_thingsspeak();
void connect_to_wifi();
//void blynk_stup(char , char , char) ;
void start_blynk();
void setup() {
  //debug serial (TX) and data receive serial (RX)
  //when programming the ESP8266, remove the connection
  //from the STM32 to the ESP8266's RX pin!
  //can also be handled with .swap() so that RX and TX pins
  //get swapped to 2 different pins, but this way we can't
  //get the debug output from a NodeMCU
  Serial.begin(115200);
  //wait at max 1 second for a string on the serial
  Serial.setTimeout(1000);
  Serial.println("Firwmare start!");

  //set WiFi mode to STATION
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client); // Initialize ThingSpeak
  //setup blynk
  Blynk.begin(auth, ssid, pass);
  connect_to_wifi();
  
}

void loop() {
  read_serial_packet();
    send_to_thingsspeak();
    // run blynk 
    start_blynk() ;
  }


void read_serial_packet() {
  if (Serial.available()) {
    currentLine = Serial.readStringUntil('\n');

    Serial.println("GOT LINE");
    Serial.println(currentLine);

    int commaSplitIndex = currentLine.indexOf(',');
    if (commaSplitIndex > 0) {
      String presencedelapiece = currentLine.substring(0, commaSplitIndex);
      String positionbas = currentLine.substring(commaSplitIndex + 1);
      String positionhaut = currentLine.substring(commaSplitIndex+2);
      String stock = currentLine.substring(commaSplitIndex + 3);

      Serial.println("[Update] poresence piece: " + presencedelapiece + " position bas: " + positionbas + " position haut: " + positionhaut + " etat stock: " + stock );

      lastpresencedelapiece = presencedelapiece.toInt();
      lastpositionbas = positionbas.toInt();
      lastpositionhaut= positionhaut.toInt();
      laststock = stock.toInt();
    }
  }
}

void connect_to_wifi() {
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
}

void send_to_thingsspeak() {
  ThingSpeak.setField(1, lastpresencedelapiece);
  ThingSpeak.setField(2, lastpositionbas);
  ThingSpeak.setField(3, lastpositionhaut);
  ThingSpeak.setField(4, laststock);
  //send update via HTTPS REST call
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println(
        "Problem updating channel. HTTP error code " + String(x));
  }
}
/*void blynk_stup(auth , ssid , pass) 
{
Blynk.begin(auth, ssid, pass);

}*/
void start_blynk()
{
Blynk.run();
}
