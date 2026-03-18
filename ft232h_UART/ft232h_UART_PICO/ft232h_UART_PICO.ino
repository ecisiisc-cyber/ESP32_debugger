// version 1 (hello buddy)

// void setup() {
//   // Initialize the USB Serial (optional, for debugging on computer)
//   Serial.begin(115200);

//   // Initialize Hardware UART 0 on GP0/GP1
//   // Default pins for Serial1 are TX=GP0, RX=GP1
//   Serial1.begin(9600); 
// }

// void loop() {
//   // Send "Hello World" to the hardware UART pins
//   Serial1.println("Hello World");

//   // Optional: Send to USB Serial Monitor as well
//   Serial.println("Sent: Hello World");

//   // Wait for 1 second
//   delay(1000);
// }



//version 2 (pos or negative teller)
// UART Communication Checker
// Pico GP0 (TX) <--> FTDI RX
// Pico GP1 (RX) <--> FTDI TX
// Pico GND      <--> FTDI GND

void setup() {
  // Initialize the UART for the FTDI connection
  // We use Serial1 because Serial is usually reserved for USB
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(115200); // Make sure your FTDI terminal matches this baud rate
  
  // Set a short timeout so the code doesn't hang waiting for numbers
  Serial1.setTimeout(50); 
  
  // Optional: Send a "Ready" message on startup so you know it reset
  Serial1.println("Pico Ready via UART!");
}

void loop() {
  // Check if data is available in the buffer
  if (Serial1.available() > 0) {
    
    // Look for the next valid integer in the incoming stream
    // parseInt() skips non-digit characters (like newlines) automatically
    long receivedNumber = Serial1.parseInt();

    // To differentiate between "0" and "No data/Timeout" (which parseInt returns on fail),
    // we only process if the stream actually had data (checked by .available above).
    // However, parseInt will consume the buffer. 
    // If you send "0", it returns 0. If you send "abc", it returns 0.
    
    // We double check if the character that triggered this was actually a digit or sign
    // But for this simple test, we assume you are sending valid numbers.

    // --- The Logic ---
    if (receivedNumber > 0) {
      Serial1.print("Received: ");
      Serial1.print(receivedNumber);
      Serial1.println(" -> Positive");
    } 
    else if (receivedNumber < 0) {
      Serial1.print("Received: ");
      Serial1.print(receivedNumber);
      Serial1.println(" -> Negative");
    } 
    else {
      // receivedNumber is 0
      // Note: If you just hit 'Enter' without a number, parseInt might return 0. 
      // This block handles the explicit "0" case.
      Serial1.println("Received: 0 -> Zero");
    }
    
    // Clear the rest of the buffer (like newlines) to prevent double triggering
    while(Serial1.available()) {
      Serial1.read(); 
    }
  }
}

