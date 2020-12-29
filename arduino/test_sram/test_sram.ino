/*
 * Memory Test Program for Lyontek LY62256 SRAM chips
 * - 32K * 8bit low power CMOS SRAM chips
 * - 2 x SRAM chips = 64KB
 */

#include <assert.h>

// Unique CE pin for each SRAM chip
// Common bus across both SRAM chips for Address, Data, WE & OE pins

const int pinChipEnableN[2] = {46, 48};
const int pinWriteEnableN = 52;         
const int pinOutputEnableN = 50;

const int kNumPinsA = 15;
const int kNumPinsD = 8;

const int kBankSize = 32 * 1024;
const int kRamSize = 2 * kBankSize;
const int kBlockSize = 256;
const int kNumBlocks = kRamSize / kBlockSize;

enum DataPinsState {
  NONE,
  IN,
  OUT,
};

DataPinsState dataPinsState = NONE;

int bankSelected = -1;        // which SRAM chip (bank) is currently selected
                              // -1 = neither, 0 or 1

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
  pinMode(pinChipEnableN[0], OUTPUT);
  pinMode(pinChipEnableN[1], OUTPUT);
  pinMode(pinWriteEnableN, OUTPUT);
  pinMode(pinOutputEnableN, OUTPUT);

  digitalWrite(pinChipEnableN[0], HIGH);
  digitalWrite(pinChipEnableN[1], HIGH);
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

void selectBank(int bank) {  
  if (bankSelected != bank) { 
    if (bankSelected >= 0) {
      digitalWrite(pinChipEnableN[bankSelected], HIGH);
    }

    bankSelected = bank;

    digitalWrite(pinChipEnableN[bankSelected], LOW);
  }
}

void setAddress(int address) {
  selectBank((address >> kNumPinsA) & 0x0001);
  
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

  //delayMicroseconds(1);
  
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

  digitalWrite(pinOutputEnableN, LOW);

  for (int i=0; i<kNumPinsD; i++) {
     int pin = pinD(i);
     int value = digitalRead(pin);
     bitWrite(readValue, i, value);
  }
  
  digitalWrite(pinOutputEnableN, HIGH);

  return readValue;
}

void sramSet(int address, int num, int value) {
  for (int i=0; i<num; i++) {
      sramWrite( address + i, value );

      if ((i % kBlockSize) == 0) {
        if ((i % (kBlockSize << 5)) == 0) {
          Serial.println(""); 
        }
        
        Serial.print(".");
      }
  }
}

void sramClear(int address, int num) {
  sramSet(address, num, 0);
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

bool sramValidateSet(int address, int num, int expectedValue) {
  for (int i=0; i<num; i++) {
      int actualValue = sramRead( address + i );
      if (actualValue != expectedValue) {
        return false;
      }
  }

  return true;
}

bool sramValidateClear(int address, int num) {
  return sramValidateSet(address, num, 0);
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

void memoryTest1() {
  Serial.println("");
  Serial.println("Memory Test 1");

  // Memory Test 1
  // - Divide memory into blocks - where each block is 256 bytes
  // - iterate through each block in turn
  //   - Fill block with the block's index
  //     (i.e. block 5 with be filled with 256 x 0x5)
  // - iterate through each block in turn
  //   - test that block is filled with its' index

  for (int blockIndex=0; blockIndex<kNumBlocks; blockIndex++) {
    int blockAddress = blockIndex * kBlockSize;
    sramSet(blockAddress, kBlockSize, blockIndex);
  }

  for (int blockIndex=0; blockIndex<kNumBlocks; blockIndex++) {
    int blockAddress = blockIndex * kBlockSize;

    bool testPassed = sramValidateSet(blockAddress, kBlockSize, blockIndex);

    if (!testPassed) {
        Serial.print("failure in block index ");
        Serial.println(blockIndex);

        sramPrint(blockAddress, kBlockSize);

        assert(!"failed test");
      }
  }
  
  Serial.println("Memory Test 1: PASSED");
}

void memoryTest2() {
  Serial.println("");
  Serial.println("Memory Test 2");

  // Memory Test 2
  // - Write 0s to all memory
  // - Divide memory into blocks - where each block is 256 bytes
  // - iterate through each block in turn
  //   - Write 0..255 to the chosen block
  //   - test that chosen block has 0..255
  //   - test that all other blocks are 0
  //   - write 0s to chosen block
  
  // clear the entire RAM
  sramClear(0, kNumBlocks * kBlockSize);

  // step through each block of memory, writing and reading
  for (int testPatternBlockIndex=0; testPatternBlockIndex<kNumBlocks; testPatternBlockIndex++) {
    Serial.print("Writing Test Pattern to block index ");
    Serial.print(testPatternBlockIndex);
    Serial.print(" of ");
    Serial.println(kNumBlocks);
    
    const int testPatternBlockOffset = testPatternBlockIndex * kBlockSize;

    sramWriteTestPattern(testPatternBlockOffset, kBlockSize);
    Serial.println("Validating block ");

    for (int i=0; i<kNumBlocks; i++) {
      const int testBlockOffset = i * kBlockSize;

      if (0 == (i % 32)) {
        Serial.println("");
      }
      
      Serial.print(i, HEX);
      Serial.print(", ");
      
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

  Serial.println("");
  Serial.println("Memory Test 2: PASSED");
}

void setup() {  
  initSerial();
  initPins();
  checkPins();

  memoryTest1();
  memoryTest2();
}


void loop() {
  // put your main code here, to run repeatedly:
  
}
