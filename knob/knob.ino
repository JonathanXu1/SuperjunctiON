// Include libraries
//#include <SocketIoClient.h>
#include <SPI.h>
#include <Tlv493d.h>
#include <Wire.h>

//Pin declarations
int output1 = D5;
int output2 = D6;
int sourceRelay = D7; //High for outlet, low for battery
int chargerRelay = D8; //High for charge, low for disconnect charger
int outletSensor = D3;

// Switch statuses
boolean s1 = true;
boolean s2 = true;
int powerMode = 1; //1 for auto, 2 for battery, 3 for outlet

// Network login
const char* ssid = "newhomoe";
const char* password = "maolan123";

// Tlv493d Opject
Tlv493d Tlv493dMagnetic3DSensor = Tlv493d();

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
  pinMode(chargerRelay, OUTPUT);
  pinMode(outletSensor, INPUT);
  
  digitalWrite(output1, HIGH);
  digitalWrite(output2, HIGH);
  digitalWrite(sourceRelay, HIGH); 
  digitalWrite(chargerRelay, LOW);
}

void loop() {
  delay(Tlv493dMagnetic3DSensor.getMeasurementDelay());
  Tlv493dMagnetic3DSensor.updateData();

  double mag = Tlv493dMagnetic3DSensor.getAmount();
  double azimuth = (Tlv493dMagnetic3DSensor.getAzimuth() + 3.14)*15.9;
  double polar = Tlv493dMagnetic3DSensor.getPolar();

  if(mag > 45){ //Pressed
    Serial.print("Pressed: ");
     if(azimuth < 20){ //Socket2
      Serial.println(" socket2");
      s2 = !s2;
      digitalWrite(output2, s2);
    } else if(azimuth <40){ //Socket1
      Serial.println(" socket1");
      s1 = !s1;
      digitalWrite(output1, s1);
    } else if(azimuth < 60){ //battery
      Serial.println(" battery");
      powerMode = 2;
    } else if(azimuth < 80){ //Auto
      Serial.println(" auto");
      powerMode = 1;
    } else { //Outlet
      Serial.println(" outlet");
      powerMode = 3;
    }
    delay(500);
  } else { //Not pressed
    Serial.println("Not pressed");
  }

  if (powerMode == 1){
    digitalWrite(sourceRelay, digitalRead(outletSensor));
  } else if (powerMode == 2){
    digitalWrite(sourceRelay, LOW);
  } else if (powerMode == 3){
    digitalWrite(sourceRelay, HIGH);
  }

  Serial.println(digitalRead(outletSensor));
  
}
