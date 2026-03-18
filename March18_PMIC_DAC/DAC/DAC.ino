// version1 
#define RX1_PIN 18
#define TX1_PIN 17

void setup() {

  // #ifdef RGB_BUILTIN
//   rgbLedWrite(RGB_BUILTIN, 0, 0, 0);  // Off / black
// #endif
  Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);
  
  // ADD THIS DELAY: Wait 2 seconds for the connection/reset to settle
  delay(2000); 
  
  Serial1.println("UART5 is ready!");
}

void loop() {
  if (Serial1.available()) {
    char data = Serial1.read();
    Serial1.print(data); 
  }
}