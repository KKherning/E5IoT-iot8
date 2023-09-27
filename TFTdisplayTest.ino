#include "Adafruit_ST7735.h" // TFT farve display (ikke OLED)
// 1.88 inch (128*160) IPS 7P SPI HD 65K Full Color LCD Module,
// Driver IC:ST7735S.

// Nødvendige hardwarepinde til kommunikation med TFT-displayet
#define TFT_SCLK  D13 // forbindes til SCL (SPI-clock) på display
#define TFT_MOSI  D12 // forbindes til SDA (SPI MOSI) på display
#define TFT_RST   D5  // forbindes til RST (reset) på display
#define TFT_DC    D8  // forbindes til DC (Data/Command) på display
#define TFT_CS    A2  // forbindes til CS (Chip Select) på display

// Opretter en Adafruit_ST7735-objektinstans ved navn "tft" og konfigurerer
// det med de nødvendige hardwarepinde til kommunikation med TFT-displayet
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() { // Display af test information
  tft.initR(INITR_BLACKTAB); // initialiserer TFT-displayet med en specifik konfiguration
  tft.fillScreen(ST7735_BLACK); // fylder hele skærmen med en sort farve
  tft.setRotation(4); // 4 er at teksten vender væk fra pins
  tft.setTextColor(ST7735_MAGENTA); // Tekst farve sættes til Magenta (rød-blå)
  tft.setTextSize(2); // Tekst størrelse er stor
  tft.setCursor(10, 20); // Set cursor position
  tft.println("EH5IoT-KK");
  tft.setTextColor(ST7735_WHITE); // Tekst farve sættes til hvid
  tft.setTextSize(1); // Tekst størrelse er lille
  tft.setCursor(10, 50);
  tft.println("Lokalt 21.0 grader");
  tft.setCursor(10, 80);
  tft.println("Aarhus 20.0 grader");
  tft.setCursor(10, 110);
  tft.println("Kbh. 21.0 grader");
}

void loop() {
  // her skal værdi opdaterings koden senere indsættes
}
