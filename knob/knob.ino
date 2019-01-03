#include <SocketIOClient.h>
#include <SPI.h>
#include <Tlv493d.h>
#include <Wire.h>

//Pin declarations
int output1 = D5;
int output2 = D6;
int sourceRelay = D7; //High for outlet, low for battery
int outletSensor = A0;

// Switch statuses
boolean s1 = false; //False for normally open
boolean s2 = false;
int powerMode = 1; //1 for auto, 2 for battery, 3 for outlet
boolean skip = false;

// Network login
const char* ssid = "newhome";
const char* password = "maolan123";
String host = "192.168.0.17";
int port = 3000;

SocketIOClient socket;

// Tlv493d Opject
Tlv493d Tlv493dMagnetic3DSensor = Tlv493d();

void light(String state) {
  Serial.println("[light] " + state);
  if (state == "\"state\":true") {
    Serial.println("[light] ON");
  }
  else {
    Serial.println("[light] off");
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial);

  Wire.setClock(100000);
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
   
  Tlv493dMagnetic3DSensor.begin();
  Tlv493dMagnetic3DSensor.setAccessMode(Tlv493dMagnetic3DSensor.MASTERCONTROLLEDMODE);
  Tlv493dMagnetic3DSensor.disableTemp();

  pinMode(output1, OUTPUT);
  pinMode(output2, OUTPUT);
  pinMode(sourceRelay, OUTPUT);
  pinMode(outletSensor, INPUT);
  
  digitalWrite(output1, LOW);
  digitalWrite(output2, LOW);
  digitalWrite(sourceRelay, HIGH); 

  Serial.println("cust2");

  //Set up wifi
  WiFi.begin(ssid, password);
  uint8_t i = 0;
  Serial.print("Connecting to wifi");
  while (WiFi.status() != WL_CONNECTED && i++ < 20){
    Serial.print(".");
    delay(500);
  }
  if(i == 21){
    Serial.println("Connecton failed");
  }
  Serial.println("wifi connected");
  socket.on("light", light);
  socket.connect(host, port);
  Serial.println("server connected");
}

void loop() {
  socket.monitor();
  delay(Tlv493dMagnetic3DSensor.getMeasurementDelay() + 10);
  Tlv493dMagnetic3DSensor.updateData();

  double mag = Tlv493dMagnetic3DSensor.getAmount();
  double azimuth = (Tlv493dMagnetic3DSensor.getAzimuth() + 3.14)*15.9;
  double polar = Tlv493dMagnetic3DSensor.getPolar();

  if(mag > 45 && !skip){ //Pressed
    Serial.print("Pressed: ");
     if(azimuth < 20){ //Socket2
      Serial.println(" socket2");
      s2 = !s2;
      digitalWrite(output2, s2);
      if(!s2) socket.emit("output2", "{\"state\":true}");
      else socket.emit("output2", "{\"state\":false}");
    } else if(azimuth <40){ //Socket1
      Serial.println(" socket1");
      s1 = !s1;
      digitalWrite(output1, s1);
      if(!s1) socket.emit("output1", "{\"state\":true}");
      else socket.emit("output1", "{\"state\":false}");
    } else if(azimuth < 60){ //battery
      Serial.println(" battery");
      powerMode = 2;
      socket.emit("mode", "{\"state\":'battery'}");
    } else if(azimuth < 80){ //Auto
      Serial.println(" auto");
      powerMode = 1;
      socket.emit("mode", "{\"state\":'auto'}");
    } else { //Outlet
      Serial.println(" outlet");
      powerMode = 3;
      socket.emit("mode", "{\"state\":'outlet'}");
    }
    skip = true;
    delay(300);
  } else { //Not pressed
    skip = false;
    Serial.println("Not pressed");
  }

  if (powerMode == 1){
    int voltage = analogRead(outletSensor);
    digitalWrite(sourceRelay, voltage > 1000);
  } else if (powerMode == 2){
    digitalWrite(sourceRelay, LOW);
  } else if (powerMode == 3){
    digitalWrite(sourceRelay, HIGH);
  }  
}
