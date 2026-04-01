#include <SPI.h>

// --- UART 1 Pins (ESP32-S3 Hardware Defaults) ---
const int UART1_RX_PIN = 18; 
const int UART1_TX_PIN = 17;

// --- SPI Pins (ESP32-S3 Native FSPI Defaults) ---
const int SCK_PIN  = 12; // FSPICLK
const int MISO_PIN = 13; // FSPIQ
const int MOSI_PIN = 11; // FSPID
const int CNV_PIN  = 10; // FSPICS0 (Used as Convert Start)

// --- AD7689 CFG Word ---
// Single-ended, IN1, Full BW, Internal 2.5V Ref, left-aligned for 16-bit transfer
const uint16_t AD7689_CFG = 0xF304; 

// --- Timing Variables ---
unsigned long lastSampleTime = 0;
const unsigned long sampleInterval = 1000; // 1000 microseconds = 1 kHz

void setup() {
  // Initialize USB Serial for local PC debugging 
  Serial.begin(115200);
  
  // Initialize UART 1 for sending ADC data to your external device
  Serial1.begin(115200, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);
  
  // Initialize CNV pin (Active High to start conversion)
  pinMode(CNV_PIN, OUTPUT);
  digitalWrite(CNV_PIN, LOW); 

  // Initialize SPI
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CNV_PIN);
  
  // Send two dummy conversions to clear the ADC's pipeline
  readAD7689();
  readAD7689();
  
  // Status messages
  Serial.println("AD7689 Initialized. Sending 1kHz data out on UART1 (TX: GPIO 17)...");
  Serial1.println("AD7689 1kHz Data Stream Started");
}

void loop() {
  unsigned long currentMicros = micros();
  
  // Non-blocking 1 kHz timer
  if (currentMicros - lastSampleTime >= sampleInterval) {
    lastSampleTime = currentMicros;
    
    // Read the ADC value
    uint16_t adcValue = readAD7689();
    
    // Convert to voltage (Assuming 2.5V internal reference and 16-bit resolution)
    float voltage = (adcValue * 2.5) / 65535.0;
    
    // Send the data out over UART1 (Serial1) instead of the standard Serial monitor
    Serial1.printf("Raw: %u \t Voltage: %.4f V\r\n", adcValue, voltage);
  }
}

// Function to handle the AD7689 Convert and SPI Read/Write cycle
uint16_t readAD7689() {
  // 1. Initiate Conversion
  digitalWrite(CNV_PIN, HIGH);
  
  // Wait for conversion to complete. 
  // The max conversion time (t_CONV) for AD7689 is 3.2 microseconds.
  delayMicroseconds(4); 
  
  // 2. Bring CNV low to enable SDO and begin data transfer
  digitalWrite(CNV_PIN, LOW);
  
  // 3. Transfer 16 bits (SPI Mode 0, MSB First)
  // We send the CFG word for the *next* conversion while reading the *current* result.
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  uint16_t result = SPI.transfer16(AD7689_CFG);
  SPI.endTransaction();
  
  return result;
}