// hey hari need to do this after ms delay turn on which rises the voltage with slew rate of on slev and after 100ms rise voltage form 0.6 to 1 volt with dvsr and after 100ms stepdown voltage from 1 to 0.6 v with dvsr
//after 100 ms tur off  the channel with DISEN 
//in setup break the replica modes of the buck 1 with others 
//physical en is high mode is high 
//version 2

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
  
  // Initialize UART 1 for receiving commands (Baud rate: 9600)
  Serial1.begin(115200, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);
  Serial1.println("ESP32-S3 System Ready. ");
  Serial1.println("Test 2 ");

  Wire.begin(8, 9);   // SDA, SCL
  Wire.setClock(100000);//100KHz

  delay(100);

  Serial1.println("MCP16701 Init");

  //--------------------------------------
  // 1. Unlock VSET
  //--------------------------------------
  unlockVSET();
Serial1.println("unlocked");

  //--------------------------------------
  // 2. Set BUCK1 = 1.0V
  //--------------------------------------
  uint8_t bcode = 0x50;  

  writeReg(0x21F, bcode);  // B1VSET0
  writeReg(0x220, bcode);  // B1VSET1

  //--------------------------------------
  // 3. Enable BUCK1
  //--------------------------------------
  writeReg(0x219, 0x81);   // CFG1 (ENABLE + fixed mode)

  //--------------------------------------
  // 4. Set LDO1 = 2.0V
  //--------------------------------------
  uint8_t lcode = 0x90;   

  writeReg(0x25E, lcode);  // L1VSET0
  writeReg(0x25F, lcode);  // L1VSET1


  //--------------------------------------
  // 5. Enable LDO1
  //--------------------------------------
  writeReg(0x259, 0x01);   // L1CFG

  //--------------------------------------
  // 6. Wait for rails
  //--------------------------------------
  delay(2);

  Serial1.println("BUCK1 and LDO1 set to 1.0V and 2.0V respectively");
}

void loop()
{
  // nothing
}