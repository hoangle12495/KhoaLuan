#include <SoftwareSerial.h>
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
#include <Arduino.h>

DHT dht (DHTPIN,DHTTYPE);
unsigned long old_time;
SoftwareSerial m_serial(8,9);
void write_uint8_t(uint8_t value);
uint8_t read_uint8_t();
void write_float(float value);
float read_float();
void write_sensor(uint8_t temp, uint8_t humi);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  m_serial.begin(9600);
  old_time = millis();
  pinMode(13,HIGH);
  dht.begin() ;
  delay(300);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t c;
  uint8_t temp ;
   uint8_t humi ;
  if(isnan(temp) || isnan(humi)){
    Serial.println("Failed to read from DHTT sensor!");
    return;
  }
  if((millis() - old_time)> 10000){
    
    temp = dht.readTemperature();
    humi = dht.readHumidity();
    write_sensor(temp,humi);
    //write_uint8_t(humi);
    Serial.print("Nhiet do: ");
    Serial.println(temp);               //Xuất nhiệt độ
    Serial.print("Do am: ");
    Serial.println(humi);  
    old_time = millis();
  }

  if(m_serial.available()>=sizeof(c)){
    c = read_uint8_t();
    Serial.print(c);
    if(c == 1)
    {
      digitalWrite(13,1);
    }
    else if(c == 0)
    {
      digitalWrite(13,0);
    }
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
   //Serial.println(value);
}else{
  return 0;
}
}
void write_sensor(uint8_t temp, uint8_t humi){
    write_uint8_t(temp);
    write_uint8_t(humi);
    Serial.println(humi);
    Serial.println(temp); 

}
