// Code for interfacing a Wii Nunchuck controller with Q-Sys via UDP communication.
// The Nunchuck joystick controlls the pan and tilt of the camera and the front buttons control the zoom.
// This code runs on a NodeMCU ESP8266 microcontroller wich connects to a WPA Personal WiFi network.
// The Wii Nunchuck connects to 3.3v power, GND, and the I2C buss pins D1-SCL D2-SDA
// Data fron the Wii Nunchuck is sent wirelessly over the network to a script running on the Q-Sys core.
// This code uses the NintendoExtensionCtrl library to interface with the Wii Nunchuck which can be downloaded here https://github.com/dmadison/NintendoExtensionCtrl

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NintendoExtensionCtrl.h>

const char* ssid = "YOUR NETWORK";    //Enter Your Network Name Here
const char* password = "YOUR PASSWORD";    //Enter Your Network Password Here


IPAddress remoteIP(192,168,0,29);    // IP address of the Q-SYS core ***Arduino.Host_IP_Address on block controller should match***
unsigned int remotePort = 4224;   // Port that the core is listening on ***Arduino.Host_Port on block controller should match***

uint8_t target_esp_mac[6] = {0x24, 0x0a, 0xc4, 0x9a, 0x58, 0x28};    // Mac address of the NodeMCU

Nunchuk nchuk;    // Initialize the Wii library


// UDP stuff and things
WiFiUDP Udp;    // Initialize UDP communication
unsigned int localUdpPort = 4210;  // local port to listen on

char  ReplyBuffer[] = "NodeMCU Online";  // a reply string to send back for debugging

String Delim = ",";   // Delimiter for the outgoing string of values


void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  // Begin connection to UDP port
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  // Begin connection to Wii Nunchuck
  nchuk.begin();

  while (!nchuk.connect()) {
    Serial.println("Nunchuk not detected!");
    delay(1000);
  }

  Serial.println("Start Loop");
}


void loop()
{
 //=========== Pull in UDP strings ==========================  
      
      // if there's data available, read a packet
      int packetSize = Udp.parsePacket();
      if (packetSize) {
        
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");

        IPAddress remote = Udp.remoteIP();
        for (int i=0; i < 4; i++) {
      Serial.print(remote[i], DEC);
          if (i < 3) {
      Serial.print(".");
          }
        }
        
      Serial.print(", port ");
      Serial.println(Udp.remotePort());

        char packetBuffer[packetSize];  // buffer to hold incoming packet,
        
        
            // read the packet into packetBufffer
        Udp.read(packetBuffer, packetSize);

      Serial.println("Contents:");
      Serial.print("");
      Serial.println(packetBuffer);
    
        // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
 
      }
      

//=========== Send out UDP strings ==========================  

      boolean success = nchuk.update();  // Get new data from the Wii nunchuck controller
    
      if (!success) {  // Ruh roh
        Serial.println("Controller disconnected!");
        delay(1000);
      }
      else {
//        // Read a button (on/off, C and Z)
//        boolean zButton = nchuk.buttonZ();
//      
//        // Read a button (on/off, C and Z)
//        boolean cButton = nchuk.buttonC();
//    
//        // Read a joystick axis (0-255, X and Y)
//        int joyX = nchuk.joyX();
//        
//        // Read a joystick axis (0-255, X and Y)
//        int joyY = nchuk.joyY();

        
        String NunData = nchuk.joyX() + Delim + nchuk.joyY() + Delim + nchuk.buttonZ() + Delim + nchuk.buttonC() + Delim;   // Construct the Wii nunchuck values into a string
        Serial.println(NunData.c_str());
      
      Udp.beginPacket(remoteIP, remotePort);    // Open UDP communication with IP and Port
      Udp.write(NunData.c_str());   // Send out UDP string
      Udp.endPacket();

      delay(10);
      } 
     
}
