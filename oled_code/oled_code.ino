#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "HX711.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define DOUT 6
#define CLK 7

HX711 scale;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float calibration_factor = 0; // Initial calibration factor

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place a known weight on the scale");

  scale.begin(DOUT, CLK);
  scale.set_scale();

  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale.tare(); // Reset the scale to 0
  long zero_factor = scale.read_average(); // Get a baseline reading
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);

  display.setCursor(0, 0);
  display.print("Calibration Factor:");
  display.setCursor(0, 16);
  display.print(calibration_factor);

  display.display();
}

void loop() {
  char input = Serial.read();
  if (input == '+' || input == 'a') {
    calibration_factor += 10;
  } else if (input == '-' || input == 'z') {
    calibration_factor -= 10;
  }

  scale.set_scale(calibration_factor);

  float weight = scale.get_units(5);
  if (weight < 0) {
    weight = 0.00;
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("Remaining Weight:");

  display.setCursor(0, 16);
  display.print(String(weight) + " Kg");

  display.setCursor(0, 48);
  display.print("People Allowed: 5");

  display.display();

  delay(1000);
}
