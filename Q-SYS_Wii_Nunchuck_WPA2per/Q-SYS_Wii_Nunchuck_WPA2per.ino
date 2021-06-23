#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NintendoExtensionCtrl.h>

const char* ssid = "YOUR NETWORK";    //Enter Your Network Name Here
const char* password = "YOUR PASSWORD";    //Enter Your Network Password Here

int zButton = 1;   //var for storing Z button state
int cButton = 1;   //var for storing C button state
int Xval = 128;   //var for storing X cooridnate
int Yval = 128;   //var for storing Y cooridnate

IPAddress remoteIP(192,168,0,29);    // IP address of the Q-SYS core ***Arduino.Host_IP_Address on block controller should match***
unsigned int remotePort = 4224;   // Port that the core is listening on ***Arduino.Host_Port on block controller should match***


uint8_t target_esp_mac[6] = {0x24, 0x0a, 0xc4, 0x9a, 0x58, 0x28};

Nunchuk nchuk;

// UDP
WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on

char  ReplyBuffer[] = "NodeMCU Received";  // a reply string to send back for debugging
char  NunData[] = "X: ";  // a reply string to send back


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
 //=========== Pull in UDP commands ==========================  
      
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


      boolean success = nchuk.update();  // Get new data from the controller
    
      if (!success) {  // Ruh roh
        Serial.println("Controller disconnected!");
        delay(1000);
      }
      else {
//        // Read a button (on/off, C and Z)
//        boolean zButton = nchuk.buttonZ();
        String Z = ",";
//      
//        // Read a button (on/off, C and Z)
//        boolean cButton = nchuk.buttonC();
        String C = ",";
//    
//        // Read a joystick axis (0-255, X and Y)
//        int joyX = nchuk.joyX();
        String X = ",";
//        int Xval = joyX;
//        
//        // Read a joystick axis (0-255, X and Y)
//        int joyY = nchuk.joyY();
        String Y = ",";
//        int Yval = joyY;
        
        String NunData = nchuk.joyX() + Y + nchuk.joyY() + Z + nchuk.buttonZ() + Z + nchuk.buttonC() + C;

        Serial.println(NunData.c_str());
      
      Udp.beginPacket(remoteIP, remotePort);
      Udp.write(NunData.c_str());
      Udp.endPacket();

      delay(10);
      } 
     
}
