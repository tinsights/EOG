//*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
//  ██████╗  █████╗ ████████╗ █████╗                         ██╗      ██████╗  ██████╗  ██████╗ ███████╗██████╗ 
//  ██╔══██╗██╔══██╗╚══██╔══╝██╔══██╗                        ██║     ██╔═══██╗██╔════╝ ██╔════╝ ██╔════╝██╔══██╗
//  ██║  ██║███████║   ██║   ███████║                        ██║     ██║   ██║██║  ███╗██║  ███╗█████╗  ██████╔╝
//  ██║  ██║██╔══██║   ██║   ██╔══██║                        ██║     ██║   ██║██║   ██║██║   ██║██╔══╝  ██╔══██╗
//  ██████╔╝██║  ██║   ██║   ██║  ██║                        ███████╗╚██████╔╝╚██████╔╝╚██████╔╝███████╗██║  ██║
//  ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝                        ╚══════╝ ╚═════╝  ╚═════╝  ╚═════╝ ╚══════╝╚═╝  ╚═╝
//*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
                                                                                                            
// Copyright Tinaes A J (ta371@cam.ac.uk)
// and Joe Ajebon (jaa58@cam.ac.uk)

#include <ResponsiveAnalogRead.h>

// Pin inputs
const int horizPin = A0;
const int vertPin = A1;

// Variables to store EOG inputs 
int horizValue = 0;
int vertValue = 0;

// Calibration and mapping declarations
int horizSensorMin = 1023, vertSensorMin = 1023;        // minimum sensor value
int horizSensorMax = 0, vertSensorMax = 0;              // maximum sensor value
int horizPix = 1440;                                    // horizontal pixel resolution to map to
int vertPix = 900;                                      // vertical pixel resolution

// possibility of changing analog reference
// to external pin (AREF) or 1.1V (INTERNAL)
// to increase resolution of ADC
//analogReference(type)

// Create ResponsiveAnalogRead objects for ADC inputs
// exponential moving average of ADC input, with "snap" (smooth/speed tradeoff constant)
// and "sleep" (minimum threshold before changing output)
// helping to elimniate noise.

float sleepThreshold = 4.0;        // threshold to wake and move output value. default 4.0
float snap = 0.1;                  // 0 to 1; lower snap = less noise, less snappy. default 0.01
boolean sleep = true;              // output sleeps when change is < threshold

ResponsiveAnalogRead analogHoriz(horizPin, sleep, snap);
ResponsiveAnalogRead analogVert(vertPin, sleep, snap);


// time constants
int calibrationTime = 10000;
int sampTime = 0;

void setup() {
  Serial.begin(9600);
  // set sleep threshold
  analogHoriz.setActivityThreshold(sleepThreshold);
  analogVert.setActivityThreshold(sleepThreshold);

  // calibration
  // turn on LED to signal the start of the calibration period:
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  while (millis() < calibrationTime) {
    analogHoriz.update();
    analogVert.update();
    horizValue = analogHoriz.getValue();
    vertValue = analogVert.getValue();

    // record the maximum and minimum sensor values
    if (horizValue > horizSensorMax) {
      horizSensorMax = horizValue;
    }
    if (horizValue < horizSensorMin) {
      horizSensorMin = horizValue;
    }

    if (vertValue > vertSensorMax) {
      vertSensorMax = vertValue;
    }
    if (horizValue < horizSensorMin) {
      vertSensorMin = vertValue;
    }
  }


  // signal the end of the calibration period
  digitalWrite(13, LOW);

}

void loop() {
  // read sensors
  analogHoriz.update();
  analogVert.update();
  horizValue = analogHoriz.getValue();
  vertValue = analogVert.getValue();

  // apply the calibration to the sensor reading
  // and map to pixel position say 1440 x 900
  horizValue = map(horizValue, horizSensorMin, horizSensorMax, 0, horizPix);
  vertValue = map(vertValue, vertSensorMin, vertSensorMax, 0, vertPix);
 
  // in case the sensor value is outside the range seen during calibration
  horizValue = constrain(horizValue, 0, horizPix);
  vertValue = constrain(vertValue, 0, vertPix);
  
  Serial.print(horizValue);
  Serial.print(";");
  Serial.print(vertValue);

  
  delay(sampTime);
}
