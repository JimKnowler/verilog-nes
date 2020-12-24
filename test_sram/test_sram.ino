/*
 * Memory Test Program for Lyontek LY62256 SRAM chips
 * - 32K * 8bit low power CMOS SRAM
 * 
 */

const int pinChipEnableN = 48;        // duplicate for two chips?
const int pinWriteEnableN = 52;
const int pinOutputEnableN = 50;

const int kNumPinsA = 15;
const int kNumPinsD = 8;

const int kDelayMillisecondsSRAM = 1;

const int kRamSize = 32768;
const int kBlockSize = 256;
const int kNumBlocks = kRamSize / kBlockSize;

enum DataPinsState {
  NONE,
  IN,
  OUT,
};

DataPinsState dataPinsState = NONE;

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
  setAddress(address);

  if (dataPinsState != OUT) {
    for (int i=0; i<kNumPinsD; i++) {
     int pin = pinD(i);
     pinMode(pin, OUTPUT);
    }
    
    dataPinsState = OUT;
  }
  
  for (int i=0; i<kNumPinsD; i++) {
     int pin = pinD(i);
     int pinValue = bitRead(value, i);
     digitalWrite(pin, pinValue);
  }

  digitalWrite(pinWriteEnableN, LOW);
  digitalWrite(pinChipEnableN, LOW);

  delay(kDelayMillisecondsSRAM);

  digitalWrite(pinChipEnableN, HIGH);
  digitalWrite(pinWriteEnableN, HIGH);
}

int sramRead(int address) {
  if (dataPinsState != IN) {
    for (int i=0; i<kNumPinsD; i++) {
     int pin = pinD(i);
     pinMode(pin, INPUT);
    }
    
    dataPinsState = IN;
  }
  
  int readValue = 0;
  
  setAddress(address);
  
  for (int i=0; i<kNumPinsD; i++) {
     int pin = pinD(i);
     pinMode(pin, INPUT);
  }

  digitalWrite(pinChipEnableN, LOW);
  digitalWrite(pinOutputEnableN, LOW);

  delay(kDelayMillisecondsSRAM);

  for (int i=0; i<kNumPinsD; i++) {
     int pin = pinD(i);
     int value = digitalRead(pin);
     bitWrite(readValue, i, value);
  }

  digitalWrite(pinOutputEnableN, HIGH);
  digitalWrite(pinChipEnableN, HIGH);

  return readValue;
}

void sramClear(int address, int num) {
  for (int i=0; i<num; i++) {
      sramWrite( address + i, 0 );

      if ((i % kBlockSize) == 0) {
        if ((i % (kBlockSize << 5)) == 0) {
          Serial.println(""); 
        }
        
        Serial.print(".");
      }
  }
}

void sramWriteTestPattern(int address, int num) {
  for (int i=0; i<num; i++) {
      sramWrite( address + i, i % kBlockSize );
  }
}

bool sramValidateTestPattern(int address, int num) {
  for (int i=0; i<num; i++) {
      int value = sramRead( address + i );
      int expected = i % 256;
      if (value != expected) {
        return false;
      }
  }

  return true;
}

bool sramValidateClear(int address, int num) {
  for (int i=0; i<num; i++) {
      int value = sramRead( address + i );
      if (value != 0) {
        return false;
      }
  }

  return true;
}

void sramPrint(int address, int num) {
  Serial.print("address ");
  Serial.print(address);
  Serial.print(" num ");
  Serial.println(num);

  for (int i=0; i<num; i++) {
      if (0 == (i % 32)) {
        Serial.println("");
      }
      int value = sramRead( address + i );
      Serial.print(value, HEX);
      Serial.print(", ");
  }
}

void setup() {  
  initSerial();
  initPins();
  checkPins();

  // clear the entire RAM
  sramClear(0, kNumBlocks * kBlockSize);

  // step through each block of memory, writing and reading
  for (int testPatternBlockIndex=0; testPatternBlockIndex<kNumBlocks; testPatternBlockIndex++) {
    Serial.print("Writing Test Pattern to Block index");
    Serial.print(testPatternBlockIndex);
    Serial.print(" of ");
    Serial.println(kNumBlocks);
    
    const int testPatternBlockOffset = testPatternBlockIndex * kBlockSize;

    sramWriteTestPattern(testPatternBlockOffset, kBlockSize);
    Serial.println(" -> Validating block ");

    for (int i=0; i<kNumBlocks; i++) {
      const int testBlockOffset = i * kBlockSize;

      Serial.print(i);
      Serial.print(", ");
      if (0 == (i % 32)) {
        Serial.println("");
      }

      bool testPassed = false;
      
      if (testBlockOffset == testPatternBlockOffset) {
        testPassed = sramValidateTestPattern(testBlockOffset, kBlockSize);
      } else {
        testPassed = sramValidateClear(testBlockOffset, kBlockSize);
      }

      if (!testPassed) {
        Serial.print("failure in block index ");
        Serial.println(i);

        sramPrint(testBlockOffset, kBlockSize);

        assert(!"failed test");
      }
    }

    sramClear(testPatternBlockOffset, kBlockSize);
  }

  Serial.println("PASSED");
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
