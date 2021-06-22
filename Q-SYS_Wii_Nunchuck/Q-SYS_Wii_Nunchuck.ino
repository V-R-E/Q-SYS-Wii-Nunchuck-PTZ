#include  <ESP8266WiFi.h>
#include <NintendoExtensionCtrl.h>

extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
#include "c_types.h"
}

// SSID to connect to
char ssid[] = "CFWLAN";
char username[] = "cp.ts-iot-tracker";
char identity[] = "cp.ts-iot-tracker";
char password[] = "N$o>PNfjH;cW";

int Zval = 1;
int Xval = 128;
int Yval = 128;



#include <WiFiUdp.h>




IPAddress remoteIP(10,1,30,100);
unsigned int remotePort = 4224;


uint8_t target_esp_mac[6] = {0x24, 0x0a, 0xc4, 0x9a, 0x58, 0x28};

Nunchuk nchuk;

// UDP
WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on

char  ReplyBuffer[] = "NodeMCU Received";  // a reply string to send back
char  NunData[] = "X: ";  // a reply string to send back





void setup() {

  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  delay(1000);
  Serial.setDebugOutput(true);
  Serial.printf("SDK version: %s\n", system_get_sdk_version());
  Serial.printf("Free Heap: %4d\n",ESP.getFreeHeap());
 
  // Setting ESP into STATION mode only (no AP mode or dual mode)
  wifi_set_opmode(STATION_MODE);

  struct station_config wifi_config;

  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char*)wifi_config.ssid, ssid);
  strcpy((char*)wifi_config.password, password);

  wifi_station_set_config(&wifi_config);
  wifi_set_macaddr(STATION_IF,target_esp_mac);
  

  wifi_station_set_wpa2_enterprise_auth(1);

  // Clean up to be sure no old data is still inside
  wifi_station_clear_cert_key();
  wifi_station_clear_enterprise_ca_cert();
  wifi_station_clear_enterprise_identity();
  wifi_station_clear_enterprise_username();
  wifi_station_clear_enterprise_password();
  wifi_station_clear_enterprise_new_password();
  
  wifi_station_set_enterprise_identity((uint8*)identity, strlen(identity));
  wifi_station_set_enterprise_username((uint8*)username, strlen(username));
  wifi_station_set_enterprise_password((uint8*)password, strlen((char*)password));

  
  wifi_station_connect();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("????");
    digitalWrite(LED_BUILTIN, HIGH);
  }

  Serial.println("CONNECT");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN, LOW);

  // Begin connection to UDP port
  Udp.begin(localUdpPort);

  nchuk.begin();

  while (!nchuk.connect()) {
    Serial.println("Nunchuk not detected!");
    delay(1000);
  }

  Serial.println("Start Loop");

}
void loop() {
    
      //=========== Pull in UDP commands ==========================  
      int c;
      
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
  

//    //========== Parse the things ===============================
//    
//          Channel = strtok(packetBuffer, &ChannelDelimiter);
//            ChannelInt = atoi(Channel);
//        Intensity = strtok(NULL, &IntensityDelimiter);
//          IntensityInt = atoi(Intensity);
//             
//        while(1){
//          Channel = strtok(NULL, &ChannelDelimiter);
//            ChannelInt = atoi(Channel);
//          Intensity = strtok(NULL, &IntensityDelimiter);
//            IntensityInt = atoi(Intensity);
//    
//          if(Intensity == NULL)
//            break;
//          int IntensityInt = atoi(Intensity);
//          
//        }

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
//        boolean zButton = nchuk.buttonZ();
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
