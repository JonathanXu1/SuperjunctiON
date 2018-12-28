#include <Tlv493d.h>

// Tlv493d Opject
Tlv493d Tlv493dMagnetic3DSensor = Tlv493d();

void setup() {
  Serial.begin(115200);
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
  String message = "";

  if(mag > 55){
    message += "Pressed";
  } else {
    message += "Not pressed ";
  } 
  message += "Angle: ";
  Serial.print(message);
  
  Serial.println(azimuth);
}
