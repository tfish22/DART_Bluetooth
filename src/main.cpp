#include <Arduino.h>
#include <BluetoothSerial.h>
#include <PS4Controller.h>

#define ESC1_PIN 32
#define ESC2_PIN 22
#define ESC1_CH 0
#define ESC2_CH 1
#define FREQ 50
#define RES 16
#define JOY_DRIFT 5

// Get the duty cycle given PWM
int getDuty(int pwm) {
  int maxDuty = (1 << RES) - 1;
  return (pwm*maxDuty)/2000;
}

// Set up ESC to connected pins on ESP
void esc_setup() {
  ledcSetup(ESC1_CH, FREQ, RES);
  ledcAttachPin(ESC1_PIN, ESC1_CH);

  ledcSetup(ESC2_CH, FREQ, RES);
  ledcAttachPin(ESC2_PIN, ESC2_CH);

  ledcWrite(ESC1_CH, getDuty(1500));  // Initially set ESC to neutral
  ledcWrite(ESC2_CH, getDuty(1500));
}

// Convert joystick position to PWM
int processJoy(int joy_pos) {
  if(abs(joy_pos) < JOY_DRIFT){ // Ignore slight positions in joystick in neutral
    return 1500;
  }

  return map(joy_pos, -128, 127, 1000, 2000); // return mapped PWM
}

void setup() {
  Serial.begin(115200);
  esc_setup();

  // Connect controller
  if (!PS4.begin()) {
    Serial.println("PS4 Bluetooth Failed");
    return;
  }

  Serial.println("PS4 Bluetooth Ready");

  // BluetoothSerial BT;
  // BT.begin("ESP32");
  // Serial.println(BT.getBtAddressString());
}

void loop() {
  if (PS4.isConnected()) {

    // Get joysticks positions
    int left_joy = PS4.LStickY(); 
    int right_joy = PS4.RStickY();

    // Convert joystick position to PWM
    int left_speed = processJoy(left_joy);
    int right_speed = processJoy(right_joy);

    // Send duty cycles to pins
    ledcWrite(ESC1_CH, getDuty(left_speed));
    ledcWrite(ESC2_CH, getDuty(right_speed));

    Serial.printf("Left Speed: %d, Right Speed: %d\n", left_speed, right_speed);
  }
  else {
    // In case controller disconnects set the ESC to neutral
    ledcWrite(ESC1_CH, getDuty(1500));
    ledcWrite(ESC2_CH, getDuty(1500));
  }
  delay(20);
}

