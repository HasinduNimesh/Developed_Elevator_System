#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Display "Remaining Weight" text
  display.setCursor(0, 0);
  display.print("Remaining Weight:");

  // Display the initial data for "People Allowed"
  display.setCursor(0, 32);
  display.print("People Allowed: 5");

  display.display();
}

void loop() {
  int remainingWeight = 300;  // Replace with your actual sensor or calculation data
  int peopleAllowed = 4;      // Replace with your actual sensor or calculation data

  updateDisplay(remainingWeight, peopleAllowed);

  delay(1000);  // Refresh every second or adjust as needed
}

void updateDisplay(int weight, int people) {
  display.clearDisplay();

  // Set text size to 1 (small text) for "Remaining Weight" and the value
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Display "Remaining Weight" text
  display.setCursor(0, 0);
  display.print("Remaining Weight:");

  // Display updated data for "Remaining Weight" (small text)
  display.setCursor(0, 16);  // Adjust the vertical position for smaller text
  display.print(String(weight) + " lbs");

  // Reset the text size to 2 for the rest of the text (larger text)
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Display updated data for "People Allowed" (larger text)
  display.setCursor(0, 48);  // Adjust the vertical position for larger text
  display.print("People Allowed: " + String(people));

  // Display the updated content on the screen
  display.display();
}

