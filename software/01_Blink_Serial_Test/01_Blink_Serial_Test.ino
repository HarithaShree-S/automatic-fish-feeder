/*
 * Project: ESP32 Automatic Fish Feeder
 * Module: 01_Blink_Serial_Test
 * Description: Basic hardware diagnostic verifying ESP32 LED control and UART
 *              Serial communication at 115200 baud.
 */

// Define the GPIO pin connected to the onboard LED
// Most ESP32 DevKit boards use GPIO 2 for the built-in blue LED
const int LED_PIN = 2;

/**
 * setup() function:
 * Executes once when the microcontroller boots or resets.
 * 
 * Purpose:
 * 1. Initialize serial communication over USB at 115200 baud rate.
 * 2. Configure the LED GPIO pin as an OUTPUT to drive the LED.
 * 3. Print an initial startup diagnostic message to the Serial Monitor.
 */
void setup() {
  // Initialize UART serial communication at 115200 bps
  Serial.begin(115200);

  // Short delay to allow the serial port and monitor to stabilize
  delay(1000);

  // Set the LED pin mode to OUTPUT
  pinMode(LED_PIN, OUTPUT);

  Serial.println("=========================================");
  Serial.println(" ESP32 Fish Feeder: 01_Blink_Serial_Test ");
  Serial.println(" Serial communication initialized @ 115200 baud");
  Serial.println("=========================================");
}

/**
 * loop() function:
 * Runs repeatedly in an infinite cycle after setup() completes.
 * 
 * Purpose:
 * 1. Turn the LED ON (HIGH) and log the state to the Serial Monitor.
 * 2. Pause for 1000 ms (1 second).
 * 3. Turn the LED OFF (LOW) and log the state to the Serial Monitor.
 * 4. Pause for 1000 ms (1 second) before repeating.
 */
void loop() {
  // Turn ON the LED
  digitalWrite(LED_PIN, HIGH);
  Serial.println("[STATUS] LED: ON  (GPIO 2 = HIGH)");
  delay(1000);

  // Turn OFF the LED
  digitalWrite(LED_PIN, LOW);
  Serial.println("[STATUS] LED: OFF (GPIO 2 = LOW)");
  delay(1000);
}
