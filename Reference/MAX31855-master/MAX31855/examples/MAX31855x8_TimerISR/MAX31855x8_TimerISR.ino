/*
 MAX31855 library example sketch
 
 This sample code is designed to be used on the MAX31855x8 breakout board.
 Look for the MAX31855 breakout boards on www.whizoo.com.
 
 This sample code corresponds to the MAX31855x8 board.  The board has a single MAX31855 IC on it, and uses a multiplexer
 to select the correct thermoucouple.  The MAX31855 takes around 100ms to take an accurate temperature reading, so 
 the best sample rate one can expect is to sample all 8 channels once-per-second.  If you are only sampling 2 channels
 then you can do it 4 times-per-second, and so on.
 
 There are 2 versions of the MAX31855x8 board: 3.3V and 5V.  There is a solder jumper on the board
 that can be changed to go from one voltage to the other.
 
 This sample code uses an Interrupt Service Routine (ISR) and a timer to continuously take readings in background.  This
 enables the main loop to obtain readings without waiting.

 In the MAX31855 library, there are 2 functions:
 1. float readJunction([CELCUIS|FAHRENHEIT])
 Returns the internal temperature of the MAX31855 IC
 
 2. float readThermocouple([CELCUIS|FAHRENHEIT])
 Returns the temperature of the probe connected to the MAX31855

 readJunction() and readThermocouple() will return the temperature, or one of these errors:
 #define FAULT_OPEN        10000  // No thermocouple
 #define FAULT_SHORT_GND   10001  // Thermocouple short to ground
 #define FAULT_SHORT_VCC   10002  // Thermocouple short to VCC
 #define NO_MAX31855       10003  // MAX31855 not communicating

 Note:  If you connect the thermocouple probe the wrong way around, the temperature will go up
 instead of down (and vice versa).  No problem, just reverse the terminals.
 
 Released under WTFPL license.
 
 27 October 2014 by Peter Easton
 
*/
#include <MAX31855.h>

// Pin connections to the MAX31855x8 board 
// The power requirement for the board is less than 2mA.  Most microcontrollers can source or sink a lot more
// than that one each I/O pin.  For example, the ATmega328 supports up to 20mA.  For convenience, the board
// is placed directly on top of a row of I/O pins on the microcontroller.  Power is supplied to the board by
// holding the GND pin low and the VIN pin high
#define GND  3
#define T0   4
#define T1   5
#define T2   6
#define VIN  7
#define MISO 8
#define CS   9
#define SCK  10

// Get the junction (IC) temperature.  Can be anything as long as it isn't 0-7
#define JUNCTION 10


void setup() {
  // Display temperatures using the serial port
  Serial.begin(9600);
  
  // Initialize pins
  pinMode(GND, OUTPUT);
  pinMode(T0, OUTPUT);
  pinMode(T1, OUTPUT);
  pinMode(T2, OUTPUT);
  pinMode(VIN, OUTPUT);
  
  // Power up the board
  digitalWrite(GND, LOW);
  digitalWrite(VIN, HIGH);
  delay(200);
  
  // Start taking the temperature of all thermocouples
  initializeThermocoupleTimer();
  
  // Wait a second to allow every thermocouple to be read
  delay(1000);
}


void loop () {
  // Display the junction temperature
  float temperature = getTemperature(JUNCTION);
  Serial.print("J=");
  printTemperature(temperature);
    
  
  // Display the temperatures of the 8 thermocouples
  for (int thermocouple=0; thermocouple<8; thermocouple++) {
    temperature = getTemperature(thermocouple);
    
    if (temperature == FAULT_OPEN)
        continue;
    Serial.print(" T");
    Serial.print(thermocouple);
    Serial.print("=");
    printTemperature(temperature);
  }
  Serial.println();
  
  // Wait a second before taking the next set of readings
  delay(1000);
}


// Print the temperature, or the type of fault
void printTemperature(double temperature) {
  switch ((int) temperature) {
    case FAULT_OPEN:
      Serial.print("FAULT_OPEN");
      break;
    case FAULT_SHORT_GND:
      Serial.print("FAULT_SHORT_GND");
      break;
    case FAULT_SHORT_VCC:
      Serial.print("FAULT_SHORT_VCC");
      break;
    case NO_MAX31855:
      Serial.print("NO_MAX31855");
      break;
    default:
      Serial.print(temperature);
      break;
  }
  Serial.print(" ");
}



