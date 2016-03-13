/*
 This sends sensor data to the MQTT server at regular intervals, disconnecting in between
 BG 6/3/16 
*/

#include <PubSubClient.h>                        //MQTT client
#include <ESP8266WiFi.h>                         //ESP specifics

// constants
const long avetime = 60000;                      // averaging period in ms - 60s
int pbIn = 4;                                    // Interrupt  is on GPIO4  PV input
int gbIn = 5;                                    // Interrupt  is on GPIO5  grid input

const char* ssid     = "yournetwork";            //wifi network, change value to suit
const char* password = "yourpassword";           //wifi password, change value to suit 

const char* host = "192.168.0.150";              //pointer to address of computer runnning MQTT broker
IPAddress server(192,168,0,150);                 //address of computer runnning MQTT broker again - try to remove later
const char* devicename = "ESP178";               // MQTT identifier - has to be unique on network
const char* topicpv =   "ESP178/PVWh";           // MQTT topic for pv data
const char* topicgrid = "ESP178/gridWh";         // MQTT topic for grid data
//                       012345678901234
IPAddress ip(192,168, 0, 178);                   // static IP address for this ESP unit
IPAddress gateway(192,168,0,1);                  // router IP to set Static IP address
IPAddress subnet(255,255,255,0);                 // needed to set static IP address 


// Variables
unsigned long oldtime;                           // store last reading time in s
unsigned long stime;                             // sample time       
int minc =0;                                     // counts seconds in minute  
volatile long pcount = 0;                        // variable to store the count coming from the PV , needs to be volatile as used by interrrupt service routine
volatile long gcount = 0;                        // variable to store the count coming from the load , needs to be volatile as used by interrrupt service routine
long pdata = 0;                                  // holds 1 minute counts for PV
long gdata = 0;                                  // holds 1 minute counts for grid
WiFiClient myclient;                             // Use WiFiClient class to create TCP connections 
const int httpPort = 80;                         // Port for TCP
PubSubClient client(myclient);                   // MQTT client using wificlient to access

byte message_buff[100];                          // holds message to be published to MQTT broke needs to be byte array        
String mytimestr;                                // to assemble date stamp for message to be published
String message;                                  // message to be published

void startWIFI(void)  {                          // use to establish WIFI connection as network may be reset or lost   
  Serial.println();                              // debug info if monitoring serial output
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);                    // connect to wifi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.config(ip, gateway, subnet);              //set to static ip have to do after inital setup 
  Serial.println("");
  Serial.println("WiFi connected");
  if (!myclient.connect(host, httpPort)) {       // set up wifi client link
    Serial.println("Wifi client connection failed");
    return;
   }
}

void callback(char* topic, byte* payload, unsigned int length) {
                                                 // not used currently, but would be used if unit subscribed to a topic
}


void linktoMQTT() {                               // Loop until  reconnected to MQTT broker
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");// printe debug info to serial port
    // Attempt to connect
    if (client.connect(devicename)) {
      Serial.println("connected");
      } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      for (int i=0; i < 500; i++){               // do in sections to allow wifi comms to happen in the background
        delay(10);                               
      }
    }
  }
}

void pint()        // PV pulse interupt service routine 
{
  pcount++;        // received a pulse so increment counter
}
void gint()        // Grid pulse interupt service routine 
{
  gcount++;        // received a pulse so increment counter
}

void setup()
{
  Serial.begin(115200);                          // used for testing and debug
  delay(10);
  // set up i/o
  pinMode(pbIn, INPUT);                          // interrput pv
  pinMode(gbIn, INPUT);                          // interrupt grid 
  //Attach the interrupts to the input pin and monitor for a falling edge Change
  attachInterrupt(pbIn, pint, FALLING);
  attachInterrupt(gbIn, gint, FALLING);  
  oldtime = millis();                            // start minute times

  startWIFI();                                    // start up wifi link
  client.setServer(server, 1883);                 // set up link to broker on correct port    
  client.setCallback(callback);                   // link callback function, for future
}

void loop() {
  stime = millis() ;                               // read clock first for accurate timing
  if ((stime - oldtime) >= avetime) {              // time to store the count , do nothing otherwise 
    oldtime = stime;                               // try to keep as close to 60 s         
    pdata = pcount;                                // update period counts
    pcount=0;                                      // zero counters for next period  
    gdata = gcount;
    gcount=0;
    Serial.println("ESPpcount3 running");          // debug information to serial port
    Serial.println(pdata);
    Serial.println(gdata);
    minc=minc+1;        
    if (minc==60) minc=0;
    
    if (WiFi.status() != WL_CONNECTED ) {           // reconnect to wifi if connection lost
      delay(1);
      startWIFI();
    }
    if (!client.connected()) {                       // Reconnect to MQTT broker
      linktoMQTT();
    }
    mytimestr = " ";                                 // not implemented yet, use time on server
    //         01345678901234567890
                                                     //do pv reading first, set up to suit MQTT
    message = mytimestr+String(pdata);
    Serial.println(message);
    message.getBytes(message_buff,message.length()+1);
    client.publish(topicpv,message_buff,message.length()+1,1);  // actaully send message to MQTT broker
                                                     // now do for grid reading
    message = mytimestr+String(gdata);
    Serial.println(message);
    message.getBytes(message_buff,message.length()+1);
    client.publish(topicgrid,message_buff,message.length()+1,1);  // send to MQTT broker
    client.disconnect();                              // finished publishing so disconnect
    Serial.println("disconnected from MQTT broker");
  }
}
