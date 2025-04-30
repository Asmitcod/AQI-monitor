#include <LiquidCrystal.h>
#include <math.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
#define MQ135_PIN A0

const float RLOAD = 10.0;
const float RZERO = 76.63;
unsigned long lastDisplay = 0;
int displayState = 0; // To track which value to display (cycles through 0-3)

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("AQ Monitor Init");
  delay(2000);
  lcd.clear();
}

void loop() {
  int sensorValue = analogRead(MQ135_PIN);
  float resistance = ((1023.0 / sensorValue) - 1) * RLOAD;
  float ratio = resistance / RZERO;
  
  // General AQI
  float generalAQI = ratio * 100;
  String genLabel = getAQILabel(generalAQI);
  
  // Specific gas PPMs (from MQ135 curves)
  float co2_ppm = pow(10, (-0.42 * log10(ratio) + 1.92));
  float nh3_ppm = pow(10, (-0.45 * log10(ratio) + 1.70));
  float tol_ppm = pow(10, (-0.34 * log10(ratio) + 1.10));
  
  // Map PPM to AQI values
  int co2_aqi = mapAQI(co2_ppm, 400, 5000, 0, 200);
  int nh3_aqi = mapAQI(nh3_ppm, 0, 250, 0, 300);
  int tol_aqi = mapAQI(tol_ppm, 0, 500, 0, 300);
  
  // Labels
  String co2_label = getAQILabel(co2_aqi);
  String nh3_label = getAQILabel(nh3_aqi);
  String tol_label = getAQILabel(tol_aqi);
  
  // ---- SERIAL OUTPUT ----
  Serial.println("===== AIR QUALITY =====");
  Serial.print("General AQI: ");
  Serial.print(generalAQI, 1);
  Serial.print(" (");
  Serial.print(genLabel);
  Serial.println(")");
  
  Serial.print("CO2: ");
  Serial.print(co2_ppm, 1);
  Serial.print(" ppm | AQI: ");
  Serial.print(co2_label);
  Serial.println(")");
  
  Serial.print("NH3: ");
  Serial.print(nh3_ppm, 1);
  Serial.print(" (");
  Serial.print(nh3_label);
  Serial.println(")");
  
  Serial.print("Toluene: ");
  Serial.print(tol_ppm, 1);
  Serial.print(" (");
  Serial.print(tol_label);
  Serial.println(")");
  Serial.println("========================\n");
  
  // For graph - machine readable output
  Serial.print("AQI:");
  Serial.print(generalAQI, 1);
  Serial.print(" CO2:");
  Serial.print(co2_ppm, 1);
  Serial.print(" NH3:");
  Serial.print(nh3_ppm, 1);
  Serial.print(" Toluene:");
  Serial.println(tol_ppm, 1);
  
  // ---- LCD OUTPUT: Rotating Display ----
  if (millis() - lastDisplay > 2000) {
    lcd.clear();
    
    // Cycle through different displays
    switch (displayState) {
      case 0: // General AQI
        lcd.setCursor(0, 0);
        lcd.print("AQI: ");
        lcd.print(generalAQI, 1);
        lcd.setCursor(0, 1);
        lcd.print(genLabel);
        break;
        
      case 1: // CO2
        lcd.setCursor(0, 0);
        lcd.print("CO2: ");
        lcd.print(co2_ppm, 1);
        lcd.print(" ppm");
        lcd.setCursor(0, 1);
        lcd.print(co2_label);
        break;
        
      case 2: // NH3
        lcd.setCursor(0, 0);
        lcd.print("NH3: ");
        lcd.print(nh3_ppm, 1);
        lcd.print(" ppm");
        lcd.setCursor(0, 1);
        lcd.print(nh3_label);
        break;
        
      case 3: // Toluene
        lcd.setCursor(0, 0);
        lcd.print("TOL: ");
        lcd.print(tol_ppm, 1);
        lcd.print(" ppm");
        lcd.setCursor(0, 1);
        lcd.print(tol_label);
        break;
    }
    
    // Move to next display state
    displayState = (displayState + 1) % 4;
    lastDisplay = millis();
  }
  
  delay(500);  // Small delay to reduce flickering
}

// Map PPM to AQI range
int mapAQI(float ppm, float ppmLow, float ppmHigh, int aqiLow, int aqiHigh) {
  if (ppm < ppmLow) return aqiLow;
  if (ppm > ppmHigh) return aqiHigh;
  return map(ppm, ppmLow, ppmHigh, aqiLow, aqiHigh);
}

// Convert AQI value to label
String getAQILabel(float aqi) {
  if (aqi <= 50) return "Good";
  else if (aqi <= 100) return "Moderate";
  else if (aqi <= 150) return "Unhealthy";
  else if (aqi <= 200) return "Very Unhealthy";
  else return "Hazardous";
}
