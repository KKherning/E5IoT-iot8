#include <C:\Particle\TFTtest2\lib\Adafruit_ST7735\src\Adafruit_ST7735.h>


// Pin konfiguration til TFT-display
#define TFT_BL  3
#define TFT_SCL 13
#define TFT_SDA 12
#define TFT_DC  6
#define TFT_RST 5
#define TFT_CS  A2

// Opret en ST7735 display instans med pindefineringer
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST, TFT_SDA, TFT_SCL);

void setup() {
  // Tænd baggrundslyset (BL)
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Tænd baggrundslyset

  // Initialiser TFT-displayet med en opløsning på 128x160
  tft.initR(INITR_BLACKTAB);
  
  // Initialiser ben som output
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_RST, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  
  // Tegn en enkel cirkel i midten af skærmen
  tft.setRotation(1); // Juster rotationsværdien efter behov (0, 1, 2 eller 3).
}

void loop() {
  tft.fillScreen(ST7735_BLACK); // Sort skærm
  delay(1000); // Vent 1 sekund

  tft.fillScreen(ST7735_WHITE); // Hvid skærm
  delay(1000); // Vent 1 sekund
}
