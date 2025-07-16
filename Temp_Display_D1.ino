#include <ESP8266WiFi.h>    
#include <ESP8266WebServer.h>  

const char* ssid = "APOLLOGUIDANCE";
const char* pwd =  "topsecret"; 
ESP8266WebServer server(80);

char RS232_Buffer[150];  
int RS232_Pointer = 0;  
int Data_Buffer[12];      
int incomingByte;
int RS232_Input;
int mul;
int i, No;


void setup() {
pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200); 
  WiFi.begin(ssid, pwd); 

  server.on("/", []() {
    
    String output = "<h1>ESP8266 Daten</h1>";
    for (int j = 0; j < 12; j++) {
      output += "Daten[" + String(j) + "] = " + String(Data_Buffer[j]) + "<br>";
    }
    
    server.send(200, "text/html", output);
  });

  server.begin();

  
}

void loop() {

  digitalWrite(LED_BUILTIN, HIGH);  
  delay(1000);                       
  digitalWrite(LED_BUILTIN, LOW);    
  delay(1000);  


  while (Serial.available() > 0)
  {
 
    incomingByte = Serial.read(); 
    RS232_Buffer [RS232_Pointer] =  incomingByte;
     
    if (RS232_Pointer <= 148) RS232_Pointer++;
    if (incomingByte == 0x0D)
    {
      No = 12;
      for (i=RS232_Pointer-2;  i>0; i--)
      {
         if (RS232_Buffer [i] == 0x3b || RS232_Buffer[i] == 0x2c) 
         {
            i--;
            mul=1;
            RS232_Input = 0;
            
            while (RS232_Buffer [i] >=0x30 && RS232_Buffer [i] <= 0x39 && i>=0)
            {
              RS232_Input = RS232_Input + (RS232_Buffer [i]-0x30)*mul;
              i--;
              mul*=10;
            }
            
          if (RS232_Buffer [i] == 0x2d) Data_Buffer [No] =  RS232_Input *-1; else  Data_Buffer [No] =  RS232_Input;
          No--;
          i++;
         }
      }

    RS232_Pointer =  0; 
    }  
 
  }
  server.handleClient();
}
