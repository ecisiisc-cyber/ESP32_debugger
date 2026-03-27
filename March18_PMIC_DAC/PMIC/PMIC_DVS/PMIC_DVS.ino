#include <Wire.h>

// --- UART 1 Pins (ESP32-S3 Hardware Defaults) ---
const int UART1_RX_PIN = 18;
const int UART1_TX_PIN = 17;

#define DEV_ADDR 0x5B

//--------------------------------------
// Write register (DATA SHEET CORRECT)
//--------------------------------------
void writeReg(uint16_t reg, uint8_t data) {
  // uint8_t opcode_h = ((1 << 5) | ((reg >> 8) & 0x03)); previous

  uint8_t opcode_h = ((1 << 2) | ((reg >> 8) & 0x03));
  uint8_t opcode_l = reg & 0xFF;

  Wire.beginTransmission(DEV_ADDR);
  Wire.write(opcode_h);
  Wire.write(opcode_l);
  Wire.write(data);
  Wire.endTransmission();
}

//--------------------------------------
// Unlock VSET registers
//--------------------------------------
void unlockVSET() {
  Wire.beginTransmission(DEV_ADDR);
  Wire.write(0x07);  // special opcode
  Wire.write(0xD3);  // enable VSET writes
  Wire.write(0xDD);  // password
  Wire.endTransmission();

  delay(1);
}


//--------------------------------------
// Setup
//--------------------------------------
void setup() {

  // Initialize UART 1 for receiving commands (Baud rate: 9600)
  Serial1.begin(115200, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);
  Serial1.println("ESP32-S3 System Ready. ");
  Serial1.println("DVS test ");

  Wire.begin(8, 9);       // SDA, SCL
  Wire.setClock(100000);  //100KHz

  delay(100);

  Serial1.println("MCP16701 Init");
  // 1. Unlock VSET
  unlockVSET();
  Serial1.println("unlocked");
  //2.CFG the Discharge pull down register,slew rate
  writeReg(0x21D, 0x40);  //Discharge regiter enabled for buck1 
  writeReg(0x21E, 0xE3);  //on slverate0.048V/ms and the dvs slew rate as 1.562V/msfor buck1
  writeReg(0x25C, 0x40);  //Discharge regiter enabled for ldo1 
  writeReg(0x25D, 0xE3);  //on slverate0.048V/ms and the dvs slew rate as 1.562V/msfor ldo1
  Serial1.println("discharge registers and the slew rate configured");
}

void loop() {
  writeReg(0x219, 0x80);  //off buck1 for 15000ms
  writeReg(0x259, 0x00);  //off ldo1 for 15000ms
  delay(15000);
  writeReg(0x21F, 0x50);  // B1VSET0
  writeReg(0x220, 0x50);  // B1VSET1 1 V for 100 ms
  writeReg(0x219, 0x81);  //Turn ON BUCK 1
  writeReg(0x25E, 0x50);  // L1VSET0
  writeReg(0x25F, 0x50);  // L1VSET1 1 V for 100 ms
  writeReg(0x259, 0x01);  //Turn ON LDO1
  delay(100);
  writeReg(0x21F, 0x90);  // B1VSET0
  writeReg(0x220, 0x90);  // B1VSET1 2 V for 100 ms
  writeReg(0x25E, 0x90);  // L1VSET0
  writeReg(0x25F, 0x90);  // L1VSET1 1 V for 100 ms
  delay(100);
  writeReg(0x21F, 0x50);  // B1VSET0
  writeReg(0x220, 0x50);  // B1VSET1 1 V for 100 ms
  writeReg(0x25E, 0x50);  // L1VSET0
  writeReg(0x25F, 0x50);  // L1VSET1 1 V for 100 ms
  delay(100);
}

