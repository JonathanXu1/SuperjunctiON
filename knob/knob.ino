#include <Tlv493d.h>

// Tlv493d Opject
Tlv493d Tlv493dMagnetic3DSensor = Tlv493d();

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Tlv493dMagnetic3DSensor.begin();
  Tlv493dMagnetic3DSensor.setAccessMode(Tlv493dMagnetic3DSensor.MASTERCONTROLLEDMODE);
  Tlv493dMagnetic3DSensor.disableTemp();
}

void loop() {
  delay(Tlv493dMagnetic3DSensor.getMeasurementDelay());
  Tlv493dMagnetic3DSensor.updateData();

  double mag = Tlv493dMagnetic3DSensor.getAmount();
  double azimuth = Tlv493dMagnetic3DSensor.getAzimuth();
  double polar = Tlv493dMagnetic3DSensor.getPolar();

  if(mag > 100){
    Serial.print("Pressed ");
  } else {
    Serial.print("Not Pressed ");
  }

  Serial.print("Angle: ");
  Serial.println(azimuth);

  delay(10);
}
