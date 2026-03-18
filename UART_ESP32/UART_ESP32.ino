// // version1 
// #define RX1_PIN 18
// #define TX1_PIN 17

// void setup() {

//   // #ifdef RGB_BUILTIN
// //   rgbLedWrite(RGB_BUILTIN, 0, 0, 0);  // Off / black
// // #endif
//   Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);
  
//   // ADD THIS DELAY: Wait 2 seconds for the connection/reset to settle
//   delay(2000); 
  
//   Serial1.println("UART5 is ready!");
// }

// void loop() {
//   if (Serial1.available()) {
//     char data = Serial1.read();
//     Serial1.print(data); 
//   }
// }

// version 2

// #define RX1_PIN 18
// #define TX1_PIN 17

// // This string acts as our waiting room for incoming characters
// String inputBuffer = ""; 

// void setup() {
//   Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);
//   delay(2000); 
  
//   Serial1.println("Ready. Format: CMD,VALUE (e.g. SET,45)");
// }

// void loop() {
//   // While there is data coming in on UART1
//   while (Serial1.available()) {
//     char inChar = (char)Serial1.read();

//     // Check if the character is an 'Enter' key (\n or \r)
//     if (inChar == '\n' || inChar == '\r') {
      
//       // If the buffer has data, process it!
//       if (inputBuffer.length() > 0) {
//         processCommand(inputBuffer); 
//         inputBuffer = ""; // Clear the buffer for the next command
//       }
//     } 
//     else {
//       // It's a normal letter/number, add it to the waiting room
//       inputBuffer += inChar;
//     }
//   }
// }

// // I moved the parsing here to keep your loop() clean and readable
// void processCommand(String input) {
//   int commaIndex = input.indexOf(',');

//   if (commaIndex > 0) {
//     String command = input.substring(0, commaIndex);
//     int value = input.substring(commaIndex + 1).toInt();

//     if (command == "SET") {
//       Serial1.print("Set parameter to: ");
//       Serial1.println(value);
//     } 
//     else if (command == "ADD") {
//       Serial1.print("Added 10. Result: ");
//       Serial1.println(value + 10);
//     } 
//     else {
//       Serial1.println("Error: Unknown command.");
//     }
//   } else {
//     Serial1.println("Error: Missing comma. Use CMD,VALUE");
//   }
// }


// //version 3
// #include <Wire.h>

// // --- UART1 Pins ---
// #define RX1_PIN 18
// #define TX1_PIN 17

// // --- I2C Pins for MPU6050 ---
// #define I2C_SDA 8
// #define I2C_SCL 9
// const int MPU_ADDR = 0x68; 

// void setup() {
//   // 1. Initialize UART1
//   Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);
//   delay(2000); // Wait for the connection to settle
  
//   // 2. Initialize the I2C bus
//   Wire.begin(I2C_SDA, I2C_SCL);
  
//   // 3. Wake up the MPU-6050
//   Wire.beginTransmission(MPU_ADDR);
//   Wire.write(0x6B); // Target the PWR_MGMT_1 register
//   Wire.write(0x00); // Write 0 to wake it up
//   Wire.endTransmission(true);
  
//   Serial1.println("MPU6050 is awake and sending data over UART1!");
// }

// void loop() {
//   // 4. Tell the MPU6050 we want to read starting from register 0x3B
//   Wire.beginTransmission(MPU_ADDR);
//   Wire.write(0x3B); 
//   Wire.endTransmission(false); // Restart condition
  
//   // 5. Request 14 bytes in a row
//   Wire.requestFrom(MPU_ADDR, 14, true);
  
//   // 6. Read and combine the data (High byte << 8 | Low byte)
//   int16_t accelX = (Wire.read() << 8 | Wire.read()); 
//   int16_t accelY = (Wire.read() << 8 | Wire.read());
//   int16_t accelZ = (Wire.read() << 8 | Wire.read());
  
//   int16_t tempRaw = (Wire.read() << 8 | Wire.read());
  
//   int16_t gyroX = (Wire.read() << 8 | Wire.read());
//   int16_t gyroY = (Wire.read() << 8 | Wire.read());
//   int16_t gyroZ = (Wire.read() << 8 | Wire.read());

//   // 7. Send the data out of UART1
//   Serial1.print("AX: "); Serial1.print(accelX);
//   Serial1.print(" | AY: "); Serial1.print(accelY);
//   Serial1.print(" | AZ: "); Serial1.print(accelZ);
  
//   Serial1.print(" || GX: "); Serial1.print(gyroX);
//   Serial1.print(" | GY: "); Serial1.print(gyroY);
//   Serial1.print(" | GZ: "); Serial1.println(gyroZ);
  
//   delay(200); // Wait a bit before blasting the next reading
// }

//version 4

#include <Wire.h>

// --- UART1 Pins ---
#define RX1_PIN 18
#define TX1_PIN 17

// --- I2C Pins & MPU6050 Address ---
#define I2C_SDA 8
#define I2C_SCL 9
const int MPU_ADDR = 0x68; 

// Buffer for incoming UART1 characters
String inputBuffer = ""; 

void setup() {
  // 1. Initialize UART1
  Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);
  delay(2000); // Allow reset/connection to settle
  
  // 2. Initialize the I2C bus
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // 3. Wake up the MPU-6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0x00); // Wake up
  Wire.endTransmission(true);
  
  Serial1.println("System Ready. Send: GET,ACCEL | GET,GYRO | GET,ALL");
}

void loop() {
  // 4. Non-blocking UART reader
  while (Serial1.available()) {
    char inChar = (char)Serial1.read();

    // Check for Enter key
    if (inChar == '\n' || inChar == '\r') {
      if (inputBuffer.length() > 0) {
        processCommand(inputBuffer); 
        inputBuffer = ""; // Clear for the next command
      }
    } 
    else {
      inputBuffer += inChar;
    }
  }
}

// --- Command Parser ---
void processCommand(String input) {
  int commaIndex = input.indexOf(',');

  if (commaIndex > 0) {
    String command = input.substring(0, commaIndex);
    String target = input.substring(commaIndex + 1);
    target.trim(); // Remove any stray spaces

    if (command == "GET") {
      if (target == "ACCEL") {
        readSensorData(true, false);
      } 
      else if (target == "GYRO") {
        readSensorData(false, true);
      }
      else if (target == "ALL") {
        readSensorData(true, true);
      }
      else {
        Serial1.println("Error: Unknown GET target. Use ACCEL, GYRO, or ALL.");
      }
    } 
    else {
      Serial1.println("Error: Unknown command.");
    }
  } else {
    Serial1.println("Error: Missing comma. Format -> CMD,TARGET");
  }
}

// --- Bare-Metal MPU6050 Reader ---
void readSensorData(bool getAccel, bool getGyro) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // Start at Accel X register
  Wire.endTransmission(false);
  
  // Request 14 bytes (Accel + Temp + Gyro)
  Wire.requestFrom(MPU_ADDR, 14, true);
  
  int16_t accelX = (Wire.read() << 8 | Wire.read()); 
  int16_t accelY = (Wire.read() << 8 | Wire.read());
  int16_t accelZ = (Wire.read() << 8 | Wire.read());
  
  int16_t tempRaw = (Wire.read() << 8 | Wire.read()); // We read it to advance the buffer, even if unused
  
  int16_t gyroX = (Wire.read() << 8 | Wire.read());
  int16_t gyroY = (Wire.read() << 8 | Wire.read());
  int16_t gyroZ = (Wire.read() << 8 | Wire.read());

  // Send requested data over UART1
  if (getAccel) {
    Serial1.print("AX:"); Serial1.print(accelX);
    Serial1.print(",AY:"); Serial1.print(accelY);
    Serial1.print(",AZ:"); Serial1.print(accelZ);
    if (getGyro) Serial1.print(" | "); // Separator if sending both
  }
  
  if (getGyro) {
    Serial1.print("GX:"); Serial1.print(gyroX);
    Serial1.print(",GY:"); Serial1.print(gyroY);
    Serial1.print(",GZ:"); Serial1.print(gyroZ);
  }
  
  Serial1.println(); // Final newline
}