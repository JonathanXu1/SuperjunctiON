/*Be sure to install these libraries:
 * https://github.com/robojay/Socket.io-v1.x-Library
 * https://github.com/Infineon/TLV493D-A1B6-3DMagnetic-Sensor
 * https://github.com/bblanchon/ArduinoJson  <-- Select version 5
*/

#include <SocketIOClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Tlv493d.h>
#include <Wire.h>

//Pin declarations
int output1 = D5;
int output2 = D6;
int sourceRelay = D7; //High for outlet, low for battery
int inverterRelay = D8; //Low for connected to battery
int outletSensor = A0;
int outletLed = D4;

// Networking variables
const char* ssid = "newhome";
const char* password = "maolan123";
String host = "superjunction.herokuapp.com";
int port = 5000;
DynamicJsonBuffer jsonBuffer(JSON_OBJECT_SIZE(3));
String JSON;
SocketIOClient socket;

// Tlv493d Opject
Tlv493d Tlv493dMagnetic3DSensor = Tlv493d();

// Switch statuses
boolean skip = false;
JsonObject& data = jsonBuffer.createObject();

void setData(String incoming) {
  Serial.println("Incoming: " + incoming);
  JsonObject& root = jsonBuffer.parseObject(incoming);
//  data["output1"] = root ["output1"];
//  data["output2"] = root ["output2"];
//  data["mode"] = root ["mode"];
//  digitalWrite(output1, !data["output1"]);
//  digitalWrite(output2, !data["output2"]);
//  Serial.println("Incoming data processed");
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
  pinMode(inverterRelay, OUTPUT);
  pinMode(outletSensor, INPUT);
  pinMode(outletLed, OUTPUT);
  
  digitalWrite(output1, LOW);
  digitalWrite(output2, LOW);
  digitalWrite(sourceRelay, HIGH);
  digitalWrite(inverterRelay, HIGH);
  digitalWrite(outletLed, HIGH);

  data["output1"] = true;
  data["output2"] = true;
  data["mode"] = "auto";

  //Set up wifi
  WiFi.begin(ssid, password);

  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20){
    delay(500);
    Serial.print(".");
  }
  if(i == 21){
    Serial.println("");
    Serial.println("WiFi failed to connect");
  } else {
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  socket.on("updateData", setData);
  
  if (!socket.connect(host, port)) {
    Serial.println("Not connected to host");
  }
  if (socket.connected()) {
    Serial.println("Connected to host");
  }
  else {
    Serial.println("Connection Error");
  }
}

void loop() {
  socket.monitor();
  
  delay(Tlv493dMagnetic3DSensor.getMeasurementDelay());
  Tlv493dMagnetic3DSensor.updateData();

  double mag = Tlv493dMagnetic3DSensor.getAmount();
  double azimuth = (Tlv493dMagnetic3DSensor.getAzimuth() + 3.14)*15.9;
  double polar = Tlv493dMagnetic3DSensor.getPolar();

  if(mag > 45 && !skip){ //Pressed
    Serial.print("Pressed: ");
     if(azimuth < 20){ //Socket2
      Serial.println(" output2");
      data["output2"] = !data["output2"];
      digitalWrite(output2, !data["output2"]);
    } else if(azimuth <40){ //Socket1
      Serial.println(" output1");
      data["output1"] = !data["output1"];
      digitalWrite(output1, !data["output1"]);
    } else if(azimuth < 60){ //battery
      Serial.println(" battery");
      data["mode"] = "battery";
    } else if(azimuth < 80){ //Auto
      Serial.println(" auto");
      data["mode"] = "auto";
    } else { //Outlet
      Serial.println(" outlet");
      data["mode"] = "outlet";
    }
    if(socket.connected()){
      JSON = "";
      data.printTo(JSON);
      socket.emit("updateData", JSON);
    }
    skip = true;
  } else { //Not pressed
    skip = false;
    Serial.println("Not pressed");
  }
  
  if (data["mode"] == "auto"){
    int voltage = analogRead(outletSensor);
    if (voltage > 1000) data["mode"] = "outlet";
    else data["mode"] = "battery";
  }
  if (data["mode"] == "battery"){
    digitalWrite(sourceRelay, LOW);
    delay(100);
    digitalWrite(inverterRelay, LOW);
    digitalWrite(outletLed, LOW);
  } else if (data["mode"] == "outlet"){
    digitalWrite(inverterRelay, HIGH);
    delay(100);
    digitalWrite(sourceRelay, HIGH);
    digitalWrite(outletLed, HIGH);
  }  
}
