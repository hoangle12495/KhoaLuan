#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <SocketIoClient.h>
#define DHTPIN 2
#define DHTTYPE DHT11
  
DHT dht (DHTPIN,DHTTYPE);
WiFiClient client;
SocketIoClient ioClient;

extern String RID;
extern String Rfull;

unsigned long old_time;
const char* ssid = "Zte";
const char* password = "viettel123";
const int port = 80;
const char* server = "hoangle1420043.mybluemix.net";
const int LED = 16;
const int BTN = 0;
const int sendingInternval = 2*1000;
void write_uint8_t(uint8_t value);
SoftwareSerial m_serial(12,14);
uint8_t read_uint8_t();
bool read_sensor(uint8_t *temp, uint8_t *humi);
void write_float(float value);
float read_float();
void write_sensor(uint8_t temp, uint8_t humi);

void event(const char * payload, size_t length) {
  if(strcmp((char*)payload,"LED1_ON") == 0){
        digitalWrite(LED,0);
        write_uint8_t(1);
      }else if(strcmp((char*)payload, "LED1_OFF") == 0){
        digitalWrite(LED,1);
        write_uint8_t(0);
      }else if(strcmp((char*)payload, "LED2_OFF") == 0){
        //digitalWrite(LED,1);
        write_uint8_t(4);
      }else if(strcmp((char*)payload, "LED2_ON") == 0){
        //digitalWrite(LED,1);
        write_uint8_t(3);
      }
  
}

void setup() {
  // put your setup code here, to run once:
  old_time = millis();
  m_serial.begin(9600);
  //dht.begin();
  pinMode(LED,OUTPUT);
  pinMode(BTN,INPUT);
  Serial.begin(9600);
  Serial.println("ESP8266 Websocket Client");
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  ioClient.on("BTN_CHANGE_SERVER", event);
  ioClient.begin(server,port);
}

void loop() {
  ioClient.loop();
  
   uint8_t temp;
   uint8_t humi;
     if(read_sensor(&temp,&humi)/*m_serial.available()>= sizeof(humi)*/){
        //humi = read_uint8_t();
       if((humi>=50) && (humi<=90)){
        if((temp>=20 && (temp <=50))){
        if(client.connect(server,port)){
        String req_uri = "/update?temp=" + String(temp) + "&humd=" + String(humi);
        client.print("GET " + req_uri + " HTTP/1.1\n" + "Host: "+ server +"\n" + "Connection: close\n" + "Content-Length:0\n" +"\n\n");
        Serial.printf("Nhiet do %s - Do am %s\r\n", String(temp).c_str(), String(humi).c_str());
      }   
        }
       }
      client.stop();
    }
   
}
void write_uint8_t(uint8_t value){
  
  // số cần gửi có giá trị trong khoảng 0 ->255
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++){
m_serial.write(*p++);
   }
}

uint8_t read_uint8_t(){
  //số trả về có giá trị trong khoảng 0->255
  uint8_t C[1];
uint8_t value = 0;
if(m_serial.available()>=sizeof(value)){
m_serial.readBytes(C, sizeof(value));
   
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++){
      *p++=C[i];
   }
   return value;
}else{
  return 0;
}
}
void write_float(float value)
{
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++){
m_serial.write(*p++);
   }
}

float read_float()
{uint8_t C[4];
float value = 0.0;
if(m_serial.available()>=sizeof(value)){
m_serial.readBytes(C, sizeof(value));
   
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++){
      *p++=C[i];
   }
   return value;
}else{
  return 0;
}
}
bool read_sensor(uint8_t *temp, uint8_t *humi){
  uint32_t kich_co_goi_tin=sizeof(*temp)+sizeof(*humi);
    if(m_serial.available()>=kich_co_goi_tin){
      //đã nhận đủ gói tin
      /////////////
      (*temp)=read_uint8_t();
      (*humi)=read_uint8_t();
      return true;// cuối cùng trả về true báo đã nhận đủ và hoàn tất
    }else{
      return false;// báo chưa đủ 
    }
    
}
