#include <SoftwareSerial.h>
#include <CapacitiveSensor.h>


//const char keyword_OK[] = "OK";
//const char keyword_carrot[] = ">";
unsigned long time_connectWeb;
unsigned long timeout_start_val;
String data_web;
String data_back;
String AllowPhoneBase;
const int MaxWebData = 1000;


String findPhoneBase[10];
int    findPhoneNum;
const int MaxData = 150;
String data_in;
boolean data_end;

//char scratch_data_from_ESP[20];//first byte is the length of bytes
SoftwareSerial ESP8266(6, 7); // RX, TX
SoftwareSerial doorBT(10, 11); // RX, TX
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,2);
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  cs_4_2.set_CS_AutocaL_Millis(0x0);
  pinMode(5,OUTPUT);
  digitalWrite(5,LOW);
  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  ESP8266.begin(9600);
  delay(300);
  ESP8266.println("AT+RST");
  read_ESP8266(2000);
  delay(1000);


  pinMode(9,OUTPUT);
  digitalWrite(9,HIGH);
  doorBT.begin(9600);
  data_end=false;
  timeout_start_val=millis();    
  time_connectWeb=millis();
}

void loop() { // run over and over
  for(int x=0;x<findPhoneNum;x++){
      if(data_back.indexOf(findPhoneBase[x]) !=-1)  {
        Serial.println(findPhoneBase[x]);
        digitalWrite(5,HIGH);
        delay(1000);
                digitalWrite(5,LOW); 
      }
  }
  delay(500);
  doorBT.listen();
  while(doorBT.available()){
          char temp;
          temp = doorBT.read();
          if(temp == '\r' || temp=='\n') data_in+='@';
          else data_in+=temp;
         if(data_in.indexOf("OK")!=-1){
              Serial.print(data_in);
              findPhone();
              data_in="";
         }
   }

  if ((millis()-time_connectWeb)>10000) { //時間經過 20s
      ESP8266.listen();
  while(ESP8266.available()) {
    Serial.write(ESP8266.read());
  }
         data_web="";
         connectWeb();
         parsePhone();
         time_connectWeb=millis();   //計時歸零
  }
  if ((millis()-timeout_start_val)>2000) { //時間經過 5s
        Serial.print("Search phone.");
        delay(300);
        doorBT.println("AT+INQ");
        delay(300);
        timeout_start_val=millis();   //計時歸零
  }
  delay(300);
  if (Serial.available()) {
          char ccc;
          ccc= Serial.read();
          if(ccc=='!'){
                Serial.println(data_back);
          }
  }
}
void findPhone(){
      int i=0;
      String data_back2;
      findPhoneNum=0;
      data_back2 = data_in;
      i=(data_back2).indexOf("+INQ:");
      while (i!= -1){
            //Serial.println(data_back);
            findPhoneBase[findPhoneNum] = (data_back2+i).substring(i+5,i+17);
            Serial.println( findPhoneBase[findPhoneNum]);
            findPhoneNum++;
            data_back2 = data_back2.substring(i+5,data_back.length());
            i=(data_back2).indexOf("+INQ:");  
      }
}
void parsePhone(){
      int x,y;

      //data_back = data_web;
      x=(data_web).indexOf("AllowPhone");
      y=(data_web).indexOf("ending");
      if(x!=-1 && y!=-1) {
            data_back = data_web.substring(x+10,y);
      }      

}

void connectWeb(){
      ESP8266.println("AT+CIPSTART=\"TCP\",\"info-107a.rhcloud.com\",80");
      read_ESP8266(2000);
      ESP8266.println("AT+CIPSEND=54\r\n");
      read_ESP8266(2000);
      ESP8266.println("GET /mac.php HTTP/1.0");
      delay(500);
      read_ESP8266(1500);
      ESP8266.println("Host: info-107a.rhcloud.com\r\n");
      read_ESP8266(5000);
     // Serial.println(data_web);
}

int read_ESP8266(int timeout_val){
      timeout_start_val=millis();
      int keyCount=0;
      char temp;
      while(1){
            if(ESP8266.available()){  //有資料就存到 key_in[]
                  temp = ESP8266.read();
                  if(temp == '\r' || temp=='\n') data_web+='@';
                  else data_web+=temp;
                  if(data_web.length() >= MaxWebData){   //資料量過太,重新檢測是否加大陣列
                      Serial.println("WebData too large!");
                      return keyCount;  
                  }    
            }
            if((millis()-timeout_start_val)>timeout_val ){   //時間到
                  return keyCount;  
            }
      }

}
