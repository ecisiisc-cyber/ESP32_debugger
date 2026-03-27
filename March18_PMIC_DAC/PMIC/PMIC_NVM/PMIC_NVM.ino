#include <Wire.h>

// --- UART 1 Pins (ESP32-S3 Hardware Defaults) ---
const int UART1_RX_PIN = 18; 
const int UART1_TX_PIN = 17;

#define DEV_ADDR 0x5B

//--------------------------------------
// Write register (DATA SHEET CORRECT)
//--------------------------------------
void writeReg(uint16_t reg, uint8_t data)
{
  // uint8_t opcode_h = ((1 << 5) | ((reg >> 8) & 0x03)); previous

  uint8_t opcode_h = ((1<< 2) | ((reg >> 8) & 0x03));
  uint8_t opcode_l = reg & 0xFF;

  Wire.beginTransmission(DEV_ADDR);
  Wire.write(opcode_h);
  Wire.write(opcode_l);
  Wire.write(data);
  Wire.endTransmission();
}

//--------------------------------------
// Unlock NVM (EEPROM) registers
//--------------------------------------
void unlockNVM()
{
  Wire.beginTransmission(DEV_ADDR);
  Wire.write(0x07);   // special opcode
  Wire.write(0xD1);   // unlock NVM key
  Wire.write(0xDD);   // password
  Wire.endTransmission();

  delay(5);
}

//--------------------------------------
// Lock NVM (EEPROM) registers
//--------------------------------------
void lockNVM()
{
  Wire.beginTransmission(DEV_ADDR);
  Wire.write(0x07);   // special opcode
  Wire.write(0xD2);   // lock NVM key
  Wire.write(0xDD);   // password
  Wire.endTransmission();

  delay(5);
}

//--------------------------------------
// Unlock VSET registers
//--------------------------------------
void unlockVSET()
{
  Wire.beginTransmission(DEV_ADDR);
  Wire.write(0x07);   // special opcode
  Wire.write(0xD3);   // enable VSET writes
  Wire.write(0xDD);   // password
  Wire.endTransmission();

  delay(1);
}

//--------------------------------------
// Setup
//--------------------------------------
void setup()
{
  
  // Initialize UART 1 for receiving commands (Baud rate: 115200)
  Serial1.begin(115200, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);
  Serial1.println("ESP32-S3 System Ready. ");
  Serial1.println("Test 2 ");

  Wire.begin(8, 9);   // SDA, SCL
  Wire.setClock(100000);//100KHz

  delay(100);

  Serial1.println("MCP16701 Init");

  //--------------------------------------
  // 0. Program NVM for Independent Bucks
  //--------------------------------------
  Serial1.println("Unlocking NVM...");
  unlockNVM();

  Serial1.println("Writing 0x80 to Bucks 2, 3, 4 NVM...");
  // Write 0x80 (Mr=1, ENABLE=0) to EEPROM CFG1 registers
  writeReg(0x021, 0x80);  // B2CFG1 (EEPROM)
  delay(10);              // 10ms delay for physical EEPROM write cycle
  writeReg(0x029, 0x80);  // B3CFG1 (EEPROM)
  delay(10);
  writeReg(0x031, 0x80);  // B4CFG1 (EEPROM)
  delay(10);
  // Separate Bucks 6, 7, 8 from Buck 5
  writeReg(0x041, 0x80);  delay(10);
  writeReg(0x049, 0x80);  delay(10);
  writeReg(0x051, 0x80);  delay(10);

  Serial1.println("Locking NVM...");
  lockNVM();

  // IMPORTANT: For the new NVM settings to become the active startup 
  // configuration, the PMIC must be power-cycled at least once after burning.
  Serial1.println("NVM Programming Complete.");

  // //--------------------------------------
  // // 1. Unlock VSET
  // //--------------------------------------
  // unlockVSET();
  // Serial1.println("VSET Unlocked");

  // //--------------------------------------
  // // 2. Set BUCK1 = 1.0V
  // //--------------------------------------
  // uint8_t bcode = 0x50;  

  // writeReg(0x21F, bcode);  // B1VSET0
  // writeReg(0x220, bcode);  // B1VSET1

  // //--------------------------------------
  // // 3. Enable BUCK1
  // //--------------------------------------
  // writeReg(0x219, 0x81);   // CFG1 (ENABLE + fixed mode)

  // //--------------------------------------
  // // 4. Set LDO1 = 2.0V
  // //--------------------------------------
  // uint8_t lcode = 0x90;   

  // writeReg(0x25E, lcode);  // L1VSET0
  // writeReg(0x25F, lcode);  // L1VSET1


  // //--------------------------------------
  // // 5. Enable LDO1
  // //--------------------------------------
  // writeReg(0x259, 0x01);   // L1CFG

  // //--------------------------------------
  // // 6. Wait for rails
  // //--------------------------------------
  // delay(2);

  // Serial1.println("BUCK1 and LDO1 set to 1.0V and 2.0V respectively");
}

void loop()
{
  // nothing
}