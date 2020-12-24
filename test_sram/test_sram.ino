const int pinChipEnableN = 48;
const int pinWriteEnableN = 52;
const int pinOutputEnableN = 50;

const int kNumPinsA = 15;
const int kNumPinsD = 8;

const int kDelayMilliseconds = 2;

#include <assert.h>

int pinA(int index) {
  assert(index >= 0);
  assert(index < kNumPinsA);  
  const int pinA0 = 53;
  return pinA0 - (index * 2);
}

int pinD(int index) {
  assert(index >= 0);
  assert(index < kNumPinsD);
  const int pinD0 = 36;
  return pinD0 - (index * 2);
}

void initSerial() {
  Serial.begin(9600);
  Serial.println("SRAM Test");  
}

void initPins() {
  pinMode(pinChipEnableN, OUTPUT);
  pinMode(pinWriteEnableN, OUTPUT);
  pinMode(pinOutputEnableN, OUTPUT);

  digitalWrite(pinChipEnableN, HIGH);
  digitalWrite(pinWriteEnableN, HIGH);
  digitalWrite(pinOutputEnableN, HIGH);

  for (int i=0; i<kNumPinsA; i++) {
    int pin = pinA(i);
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
}

void checkPins() {
  Serial.println("Checking Pins...");
  
  assert(pinA(0) == 53);
  assert(pinA(1) == 51);
  assert(pinA(14) == 25);
  
  assert(pinD(0) == 36);
  assert(pinD(7) == 22);
}

void setAddress(int address) {
  for (int i=0; i<kNumPinsA; i++) {
    int pinValue = bitRead(address, i);
    digitalWrite(pinA(i), pinValue);
  }
}

void sramWrite(int address, int value) {
#if 0
  Serial.print("Writing ");
  Serial.print(value);
  Serial.print(" to address ");
  Serial.println(address);
#endif
  
  setAddress(address);
  
  for (int i=0; i<kNumPinsD; i++) {
     int pin = pinD(i);
     int pinValue = bitRead(value, i);
     pinMode(pin, OUTPUT);
     digitalWrite(pin, pinValue);

#if 0
     Serial.print("setting D ");
     Serial.print(i);
     Serial.print(" on pin ");
     Serial.print(pin);
     Serial.print(" to value ");
     Serial.println(pinValue );
#endif
  }

  digitalWrite(pinWriteEnableN, LOW);
  digitalWrite(pinChipEnableN, LOW);

  delay(kDelayMilliseconds);

  digitalWrite(pinChipEnableN, HIGH);
  digitalWrite(pinWriteEnableN, HIGH);
}

int sramRead(int address) {
  int readValue = 0;
  
  setAddress(address);
  
  for (int i=0; i<kNumPinsD; i++) {
     int pin = pinD(i);
     pinMode(pin, INPUT);
  }

  digitalWrite(pinOutputEnableN, LOW);
  digitalWrite(pinChipEnableN, LOW);

  delay(kDelayMilliseconds);

  for (int i=0; i<kNumPinsD; i++) {
     int pin = pinD(i);
     int value = digitalRead(pin);
     bitWrite(readValue, i, value);

#if 0
     Serial.print("reading D ");
     Serial.print(i);
     Serial.print(" on pin ");
     Serial.print(pin);
     Serial.print(" as value ");
     Serial.println(value );
#endif
  }

  digitalWrite(pinChipEnableN, HIGH);
  digitalWrite(pinOutputEnableN, HIGH);

  return readValue;
}

void setup() {  
  initSerial();
  initPins();
  checkPins();

  /// TODO: size of SRAM chip
  const int kRamSize = 512;

  Serial.println("Writing...");
  for (int i=0; i<kRamSize; i++) {
      sramWrite( i, i % 256 );
  }
  
  Serial.println("Reading...");
  for (int i=0; i<kRamSize; i++) {
      if (0 == (i % 16)) {
        Serial.println("");
      }
      int value = sramRead(i);
      Serial.print(value, HEX);
      Serial.print(" ");

  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
