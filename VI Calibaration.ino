/* ----------------------------------------------------
   Single-Phase Voltage and Current Sensor Calibration
   ----------------------------------------------------

   Instructions:

   1. Hardware Setup:
      - Connect your voltage sensor (e.g., ZMPT101B) to pin V1 (GPIO 34).
      - Connect one current sensor (e.g., SCT-013) at a time to pin I1 (GPIO 35).

   2. Calibration:
      - Start with CV1 = 1000 and CI1 = 1000.
      - Load a purely resistive device (e.g., heater, kettle).
      - Use a multimeter to measure actual voltage and current.
      - Compare those to the ESP32 readings in Serial Monitor.
      - Use this formula to calibrate:
         New CV1 = (Vr * CVold) / Vesp
         New CI1 = (Ir * CIold) / Iesp
         (Vr and Ir are reference values; Vesp and Iesp are from Serial)

   3. To calibrate multiple current sensors:
      - Swap them into I1 pin one at a time.
      - Note individual CI1 values for each sensor.
   ---------------------------------------------------- */

#include "EmonLib.h"  // EmonLib for energy monitoring
#define ESP32

// ------------------ Pin Configuration ------------------
#define V1 34   // Voltage sensor pin
#define I1 35   // Current sensor pin

// ------------------ Calibration ------------------
#define CV1 1000   // Voltage calibration factor
#define CI1 1000   // Current calibration factor

// ------------------ Monitor Object ------------------
EnergyMonitor emon1;

// ------------------ Energy Tracking ------------------
double accumulatedEnergy_kWh = 0.0;  // Total energy consumed (kWh)
unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);

  // ESP32 ADC range configuration
  analogSetPinAttenuation(V1, ADC_11db);
  analogSetPinAttenuation(I1, ADC_11db);

  // Setup voltage and current measurement
  emon1.voltage(V1, CV1, 1.732);  // pin, calibration, phase shift
  emon1.current(I1, CI1);         // pin, calibration

  lastMillis = millis();
}

void loop() {
  Serial.println("----");

  emon1.calcVI(120, 2000);  // Calculate over 120 half-cycles

  Serial.print("Vrms: ");
  Serial.print(emon1.Vrms);
  Serial.print(" V | Irms: ");
  Serial.print(emon1.Irms);
  Serial.print(" A | Real Power: ");
  Serial.print(emon1.realPower);
  Serial.print(" W");

  // ----- Energy Calculation -----
  unsigned long currentMillis = millis();
  double elapsedHours = (currentMillis - lastMillis) / 3600000.0; // ms → hours
  lastMillis = currentMillis;

  // Energy = Power × Time
  accumulatedEnergy_kWh += (emon1.realPower * elapsedHours) / 1000.0;

  Serial.print(" | Energy: ");
  Serial.print(accumulatedEnergy_kWh, 6); // 6 decimal places for precision
  Serial.println(" kWh");

  delay(1000);
}
