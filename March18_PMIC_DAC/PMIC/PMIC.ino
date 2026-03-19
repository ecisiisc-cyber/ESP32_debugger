#include <Wire.h>

#define DEV_ADDR 0x5B   // MCP16701 I2C address

// -----------------------------
// Low-level write (single byte)
// -----------------------------
void writeReg(uint16_t reg, uint8_t data)
{
  Wire.beginTransmission(DEV_ADDR);

  uint8_t opcode_h = 0x00;        // simple single-byte write
  uint8_t opcode_l = reg & 0xFF;  // lower 8-bit address

  Wire.write(opcode_h);
  Wire.write(opcode_l);
  Wire.write(data);

  Wire.endTransmission();
}

// -----------------------------
// Unlock VSET registers
// -----------------------------
void unlockVSET()
{
  Wire.beginTransmission(DEV_ADDR);

  Wire.write(0x07);   // special command opcode
  Wire.write(0xD3);   // enable VSET writes
  Wire.write(0xDD);   // password

  Wire.endTransmission();

  delay(1);
}

// -----------------------------
// Set BUCK1 voltage
// -----------------------------
void setBuck1Voltage(float voltage)
{
  // Convert voltage to code
  uint8_t code = (voltage - 0.6) / 0.0125;

  writeReg(0x1F, code);  // B1VSET0 (volatile 0x21F → 0x1F in opcode)
  writeReg(0x20, code);  // B1VSET1

  Serial.print("BUCK1 set to code: ");
  Serial.println(code, HEX);
}

// -----------------------------
// Enable BUCK1
// -----------------------------
void enableBuck1()
{
  writeReg(0x19, 0x31);  // B1CFG1 → ENABLE=1, fixed mode
}

// -----------------------------
// Set LDO1 voltage
// -----------------------------
void setLDO1Voltage(float voltage)
{
  uint8_t code = (voltage - 0.6) / 0.025;  // LDO usually 25mV step

  writeReg(0x40, code);  // example LDO1 VSET (check exact address)

  Serial.print("LDO1 set to code: ");
  Serial.println(code, HEX);
}

// -----------------------------
// Enable LDO1
// -----------------------------
void enableLDO1()
{
  writeReg(0x38, 0x31);  // LDO1 CFG (example address)
}

// -----------------------------
// Setup
// -----------------------------
void setup()
{
  Serial.begin(115200);

  Wire.begin(21, 22);   // SDA, SCL (ESP32-S3 default)
  Wire.setClock(400000);

  delay(100);

  Serial.println("MCP16701 Control Start");

  // 1. Unlock VSET
  unlockVSET();

  // 2. Set voltages
  setBuck1Voltage(1.0);   // 1.0V core
  setLDO1Voltage(1.8);    // 1.8V IO/analog

  // 3. Enable rails
  enableBuck1();
  enableLDO1();

  Serial.println("Setup complete");
}

// -----------------------------
// Loop (example sweep)
// -----------------------------
void loop()
{
  // Example: sweep BUCK1
  for(float v = 0.9; v <= 1.2; v += 0.05)
  {
    setBuck1Voltage(v);

    delay(1);  // settling delay

    Serial.print("Sweeping BUCK1 to: ");
    Serial.println(v);

    delay(2000);  // observe
  }
}