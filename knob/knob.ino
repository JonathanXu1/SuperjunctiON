#include <SPI.h>
#include <Tlv493d.h>

#include <Wire.h>

int output1 = D5;
int output2 = D6;
int source = D7;
int charger = D8;

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
  pinMode(source, OUTPUT);
  pinMode(charger, OUTPUT);
  
  digitalWrite(output1, HIGH);
  digitalWrite(output2, HIGH);
  digitalWrite(source, HIGH);
  digitalWrite(charger, HIGH);
}


void loop() {
  delay(Tlv493dMagnetic3DSensor.getMeasurementDelay());
  Tlv493dMagnetic3DSensor.updateData();

  double mag = Tlv493dMagnetic3DSensor.getAmount();
  double azimuth = Tlv493dMagnetic3DSensor.getAzimuth();
  double polar = Tlv493dMagnetic3DSensor.getPolar();

  if(mag > 55){
    Serial.print("Pressed ");
  } else {
    Serial.print("Not pressed (");
  }
  Serial.print(mag);
  Serial.print(") Angle: ");
  Serial.println(azimuth);
}
