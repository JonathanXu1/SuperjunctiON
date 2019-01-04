/*Be sure to install these libraries:
 * https://github.com/robojay/Socket.io-v1.x-Library
 * https://github.com/Infineon/TLV493D-A1B6-3DMagnetic-Sensor
 * https://github.com/bblanchon/ArduinoJson
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
const char* ssid = "MJMH";
const char* password = "4166892113";
String host = "192.168.0.16";
int port = 5000;
StaticJsonBuffer<200> jsonBuffer;
SocketIOClient socket;

// Tlv493d Opject
Tlv493d Tlv493dMagnetic3DSensor = Tlv493d();

// Switch statuses
boolean skip = false;
JsonObject& data = jsonBuffer.createObject();
data["output1"] = true;
data["output2"] = true;
data["mode"] = "auto";

void setData(String data) {
  Serial.println("Data " + data);
  /*
  data = data.substring(11);
  s1 = data.substring(0, 4).equals("true");
  if (s1) s1S = "true";
  else s1S = "false";
  data = data.substring(11);
  s2 = data.substring(0, 4).equals("true");
  if (s2) s2S = "true";
  else s2S = "false";
  data = data.substring(9);
  powerMode = data.substring(0, data.indexOf("'"));
  Serial.println("Update: " + s1S + " " + s2S + " " + powerMode);
  */
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

  //Set up wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  
  if (client.connected()) client.send("connection", "message", "Connected !!!!");
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
      data["output2"] = !data["output2"];
      digitalWrite(output2, !data["output2"]);
    } else if(azimuth <40){ //Socket1
      Serial.println(" socket1");
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
    socket.emit("updateData", data);
    Serial.println(data);
    skip = true;
    delay(300);
  } else { //Not pressed
    skip = false;
    Serial.println("Not pressed");
  }

  if (data["mode"].equals("auto")){
    int voltage = analogRead(outletSensor);
    if (voltage > 1000) data["mode"] = "outlet";
    else data["mode"] = "battery";
  }
  if (data["mode"].equals("battery")){
    digitalWrite(sourceRelay, LOW);
    delay(100);
    digitalWrite(inverterRelay, LOW);
    digitalWrite(outletLed, LOW);
  } else if (data["mode"].equals("outlet")){
    digitalWrite(inverterRelay, HIGH);
    delay(100);
    digitalWrite(sourceRelay, HIGH);
    digitalWrite(outletLed, HIGH);
  }  
}
