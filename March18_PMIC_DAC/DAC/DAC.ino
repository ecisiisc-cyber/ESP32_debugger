#include <SPI.h>

// --- UART 1 Pins (ESP32-S3 Hardware Defaults) ---
const int UART1_RX_PIN = 18; 
const int UART1_TX_PIN = 17;

// --- SPI Pins (ESP32-S3 Native FSPI Defaults) ---
const int SCK_PIN  = 12; // FSPICLK
const int MISO_PIN = 13; // FSPIQ
const int MOSI_PIN = 11; // FSPID
const int CS_PIN   = 10; // FSPICS0

void setup() {
  // Initialize standard Serial for debugging (USB Monitor)
  Serial.begin(115200);
  
  // Initialize UART 1 for receiving commands (Baud rate: 9600)
  Serial1.begin(9600, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);
  Serial1.println("ESP32-S3 System Ready. ");

  // Initialize Chip Select
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH); // Set Chip Select HIGH (inactive)

  // Initialize SPI using the S3's native pins
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
}

void loop(){
  sendSpiData(0x30, 0x66, 0x66);
  delay(10000);
  sendSpiData(0x30, 0x99, 0x99);
  delay(10000);
  sendSpiData(0x30, 0xFF, 0xFF);
  delay(10000);
}


// Helper function to send exactly 3 bytes over SPI
void sendSpiData(uint8_t byte1, uint8_t byte2, uint8_t byte3) {
  // Set SPI parameters: 1MHz speed, Most Significant Byte First, SPI Mode 0
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  
  digitalWrite(CS_PIN, LOW);   // Pull CS LOW to activate the SPI slave
  
  SPI.transfer(byte1);         // Send 1st byte
  SPI.transfer(byte2);         // Send 2nd byte
  SPI.transfer(byte3);         // Send 3rd byte
  
  digitalWrite(CS_PIN, HIGH);  // Pull CS HIGH to release the SPI slave
  
  SPI.endTransaction();

  // Print confirmation to the debug monitor
  Serial1.printf("SPI Sent: 0x%02X 0x%02X 0x%02X\n", byte1, byte2, byte3);
}