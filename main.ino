#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// --- CONFIGURATION ---
const int RELAY_PIN = 10;
const int FLOW_SENSOR_PIN = 2; 

// --- CALIBRATION ---
// Adjust this if physical liquid is too high/low.
// Lower number = pump stays on longer (more liquid).
float calibrationFactor = 4.5; 

// --- VARIABLES ---
volatile byte pulseCount = 0;  
float flowRate = 0.0;
unsigned int flowMilliLitres = 0;
unsigned long totalMilliLitres = 0;
unsigned long oldTime = 0;

// --- LCD SETUP ---
LiquidCrystal_I2C lcd(0x27, 16, 2);  

// --- KEYPAD SETUP ---
const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 12}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

String inputString = ""; 

// --- INTERRUPT FUNCTION ---
void pulseCounter() {
  pulseCount++;
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Relay OFF (Assuming Active LOW)
  
  pinMode(FLOW_SENSOR_PIN, INPUT);
  digitalWrite(FLOW_SENSOR_PIN, HIGH); 
  
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);
  
  lcd.init();
  lcd.backlight();
  
  resetDisplay();
}

void loop() {
  char customKey = customKeypad.getKey();
  
  if (customKey) {
    if (customKey == '#') {
      validateAndDispense();
    }
    else if (customKey == '*') {
      inputString = "";
      resetDisplay();
    }
    // Only allow numbers 0-9 to be typed. Ignores A, B, C, D.
    else if (customKey >= '0' && customKey <= '9') {
      if(inputString.length() < 3) { // Prevents typing more than 99
        inputString += customKey;
        lcd.setCursor(4, 1);
        lcd.print(inputString);
      }
    }
  }
}

void resetDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Amount:");
  lcd.setCursor(0, 1);
  lcd.print("Rs: ");
  inputString = "";
}

void validateAndDispense() {
  int amount = inputString.toInt();
  
  // NEW LOGIC: Range is 1 Rs to 20 Rs
  if (amount >= 1 && amount <= 100) {
    
    // CALCULATE VOLUME: 1 Rs = 50mL (20 Rs = 1000mL)
    int targetVolume = amount * 10;
    
    lcd.clear();
    lcd.print("Rs " + String(amount) + " OK!");
    lcd.setCursor(0, 1);
    lcd.print("Vol: " + String(targetVolume) + "mL");
    delay(2000);
    
    dispenseJuice(targetVolume); 
    
    lcd.clear();
    lcd.print("Done! Enjoy.");
    delay(3000);
    resetDisplay();
  } 
  else {
    lcd.clear();
    lcd.print("Invalid Amount");
    lcd.setCursor(0, 1);
    lcd.print("Range: 1-100 Rs"); // Matches new logic
    delay(2000);
    resetDisplay();
  }
}

void dispenseJuice(int targetMl) {
  pulseCount = 0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  oldTime = millis();
  unsigned long lastPulseTime = millis(); // Timeout tracker

  lcd.clear();
  lcd.print("Filling...");

  digitalWrite(RELAY_PIN, LOW); // Pump ON

  while (totalMilliLitres < targetMl) {
    
    // Safety: Shut off if 5 seconds pass with zero liquid flowing
    if ((millis() - lastPulseTime) > 5000 && totalMilliLitres < targetMl) {
        lcd.clear();
        lcd.print("Error: No flow!");
        delay(3000);
        break; // Exit loop
    }

    // Fixed: Stable 100ms sampling rate
    if((millis() - oldTime) > 100) { 
      
      // Safe Interrupt Handling
      noInterrupts(); 
      byte currentPulses = pulseCount;
      pulseCount = 0;
      interrupts(); 
      
      if (currentPulses > 0) {
          lastPulseTime = millis(); // Reset timeout tracker
      }
        
      // Fixed: 1000.0 restored for correct math conversion
      flowRate = ((70.0 / (millis() - oldTime)) * currentPulses) / calibrationFactor;
      double volumeThisSlice = (flowRate / 60.0) * (millis() - oldTime);
      totalMilliLitres += volumeThisSlice;
      
      oldTime = millis();

      lcd.setCursor(0, 1);
      lcd.print(String((int)totalMilliLitres) + " / " + String(targetMl) + "mL  ");
    }
  }
  
  digitalWrite(RELAY_PIN, HIGH); // Pump OFF
}