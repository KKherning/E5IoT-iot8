#include "ADXL345.h"                                  // ADXL345 accelerometer
#include "HttpClient.h"                               // bruges til at hente vejudsigt fra internet
#include "Adafruit_ST7735.h"                          // TFT farve display 1.88 inch (128*160)
#include <..\lib\PietteTech_DHT\src\PietteTech_DHT.h> // DHT11 temp måler

const int photoresistor = A0; // Analogt input til lysfoelsom modstand
const int ADXL_ONOFF = A1;    // pin til at tænde og slukke for ADXL345 enheden
const int TFT_CS = A2;        // forbindes til CS (Chip Select) paa display
const int LYSMAAL_ONOFF = A3; // pin til at tænde og slukke for lysmålingen
const int ADXL_SDA = D0;      // Argon D0 SDA er forbundet til ADXL345 SDA
const int ADXL_SCL = D1;      // Argon D1 SCL er forbundet til ADXL345 SCL
const int DHT_ONOFF = D2;     // pin til at tænde og slukke for DHT11 enheden
const int DHT_PIN = D3;       // pin til DHT11 kommunikation (ben 2 på DHT11)
const int TFT_ONOFF = D4;     // pin til at tænde og slukke for TFT display
const int TFT_RST = D5;       // forbindes til RST (reset) paa display
const int GROEN_LED = D6;     // Grøn ekstern LED tilsluttet D6
const int INDBYG_LED = D7;    // Indbygget blaa LED som også er gpio D7
const int TFT_DC = D8;        // forbindes til DC (Data/Command) paa display
const int TFT_MOSI = D12;     // forbindes til SDA (SPI MOSI) paa display
const int TFT_SCLK = D13;     // forbindes til SCL (SPI-clock) paa display

int adxl345X = 0;                   // Variabel til at gemme ADXL345 x-retning
int adxl345Y = 0;                   // Variabel til at gemme ADXL345 y-retning
int lysniveau = 0;                  // Variabel til at gemme analogt lys-input
const int LYSNIVEAU_TRESHOLD = 100; // Lysniveau tærskelværdi (er det lyst nok)
float AarhusTemperature = 0.0;      // Århus-temp fra api.openweathermap.org
float copenhagenTemperature = 0.0;  // København-temp fra api.openweathermap.org
float tempValue = 0.0;              // variabel til temperatur fra internet
char charLysNiveau[20] = "";        // Global char buffer til lysdata
char charTempLokal[20] = "";        // Global char buffer til lokal temperatur
char charTempArh[20] = "";          // Global char buffer til temperatur Århus
char charTempCph[20] = "";          // Global char buffer til temperatur Københ.
// Min hemmelige vejr API-nøgle til api.openweathermap.org
const char *apiKey = "???????????????????????????????????";
// Grund-URL til brug ved hentning af vejr-data fra internet
const char *baseUrl = "http://api.openweathermap.org/data/2.5/weather?q=";
float LokalTemp = 0.0;   // variabel til at opbevare DHT11 temperatur data
char urlAarhus[256];     // komplette URL til at hente temperatur i Århus
char urlCopenhagen[256]; // komplette URL til at hente temperatur i København

ADXL345 accelerometer;                // ADXL245 accelerometer
PietteTech_DHT DHT(DHT_PIN, DHT11);   // lokal temperatursensor DHT11
SystemSleepConfiguration sleepconfig; // variablen sleepconfig er en soveinstans
HttpClient http;                      // variablen http, som er en instans af HttpClient-klassen.
http_request_t request;               // til at hente temperatur fra internet
http_response_t response;             // til at modtage temperatur fra internet
// Opretter en Adafruit_ST7735-objektinstans som hedder tft
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

enum State // Definerer min tilstandsmaskines 4 tilstande
{
  SLEEP,  // sov i 30 sekunder, vågn op og skift til MAAL
  MAAL,   // hvis det er lyst så mål lokal temp, og afgør Århus eller København
  VISARH, // vis overskrift, lokal DHT11 temp samt temp i Århus, i 15s
  VISCPH  // vis overskrift, lokal DHT11 temp samt temp i København, i 15s
};

State currentState = SLEEP; // Initialiser start tilstanden til SLEEP

void setup()
{
  pinMode(photoresistor, INPUT);  // A0 Analogt input til lysfølsom modstand
  pinMode(ADXL_ONOFF, OUTPUT);    // A1 til at tænde og slukke for ADXL345
  pinMode(LYSMAAL_ONOFF, OUTPUT); // A3 til at tænde og slukke for lysmåling
  pinMode(DHT_ONOFF, OUTPUT);     // D2 til at tænde og slukke for DHT11 enheden
  pinMode(TFT_ONOFF, OUTPUT);     // D4 til at tænde og slukke for display
  pinMode(GROEN_LED, OUTPUT);     // D6 Grøn ekstern LED tilsluttet D6
  pinMode(INDBYG_LED, OUTPUT);    // D7 Indbygget blå LED

  // Til at starte med slukkes alle sensorer og alle aktuatorer
  digitalWrite(INDBYG_LED, LOW);    // sluk blå LED på vej ind i ny state
  digitalWrite(GROEN_LED, LOW);     // sluk grøn ekstern LED
  digitalWrite(DHT_ONOFF, LOW);     // sluk for DHT11
  digitalWrite(TFT_ONOFF, HIGH);    // sluk for display via PNP transistor
  digitalWrite(ADXL_ONOFF, LOW);    // sluk for ADXL345 accelerometer
  digitalWrite(LYSMAAL_ONOFF, LOW); // sluk for lysmåling

  sleepconfig.mode(SystemSleepMode::STOP).duration(30000); // STOP i 30 sekunder
  // Real Time Clock RTC bruges til at holde øje med sove-tiden

  // Initialiser vejr-API URL'er for Århus og København
  snprintf(urlAarhus, sizeof(urlAarhus),
           "%sAarhus,dk&units=metric&appid=%s", baseUrl, apiKey);
  snprintf(urlCopenhagen, sizeof(urlCopenhagen),
           "%sCopenhagen,dk&units=metric&appid=%s", baseUrl, apiKey);
}

void loop()
{
  switch (currentState) // tilstandsmaskine med 4 tilstande
  {
  case SLEEP:
    digitalWrite(INDBYG_LED, LOW); // sluk blå LED som også er pin D7
    digitalWrite(GROEN_LED, LOW);  // sluk grøn ekstern LED
    System.sleep(sleepconfig);     // stop mode, sov i 30s og vågn op igen
    currentState = MAAL;           // skift tilstand til MAAL efter søvn
    break;

  case MAAL:
    digitalWrite(INDBYG_LED, HIGH);     // tænd blå LED som også er pin D7
    maalLysStyrke();                    // måler lysstyrken og opdaterer variablen lysniveau
    if (lysniveau < LYSNIVEAU_TRESHOLD) // hvis det er mørkere end grænsev.
    {
      currentState = SLEEP; // hvis det er mørkt så sov igen i 30 sekunder
    }
    else // hvis det er lyst nok så udfør følgende
    {
      maalADXL345Data();                      // aflæs accelerometer og opdater adxl345X og adxl345Y
      if ((adxl345Y > 10) or (adxl345X > 10)) // vipper brugeren med systemet
      {
        digitalWrite(GROEN_LED, HIGH); // tænd grøn ekstern LED
        maalLokalTemp();               // aflæs lokal temperatur på DHT11 og opdater LokalTemp
        if (adxl345Y > adxl345X)       // vipper brugeren mest i x-retning eller mest i y-retning?
        {
          currentState = VISCPH; // brugeren vipper mest i y-retning og vælger derved København
        }
        else
        {
          currentState = VISARH; // brugeren vipper mest i x-retning og vælger derved Århus
        }
      }
      else
        currentState = SLEEP; // hvis ingen vip så sov igen i 30 sekunder
    }
    break;

  case VISARH:
    // hent Århus temperatur fra internet
    AarhusTemperature = getTemperature(urlAarhus);
    visTemperatur("Aarhus"); // vis data i display i 15 sekunder
    currentState = SLEEP;
    break;

  case VISCPH:
    // hent København temperatur fra internet
    copenhagenTemperature = getTemperature(urlCopenhagen);
    visTemperatur("Koebenhavn"); // vis data i display i 15 sekunder
    currentState = SLEEP;
    break;
  }
}

void maalLysStyrke() // måling af lysstyrke med lysmodstand
{
  digitalWrite(LYSMAAL_ONOFF, HIGH);     // tænd for lysmåling
  lysniveau = analogRead(photoresistor); // mål og opdater variablen lysniveau
  digitalWrite(LYSMAAL_ONOFF, LOW);      // sluk for lysmåling
}

void maalLokalTemp() // måling med DHT11
{
  digitalWrite(DHT_ONOFF, HIGH); // tænd for DHT11 lokal temperaturmåler
  delay(1000);                   // giv DHT11 1 sekund til at starte op
  pinMode(DHT_PIN, OUTPUT);      // D3 skriver først til DHT11, læser senere
  DHT.begin();                   // Initialiser DHT11
  DHT.acquireAndWait(1000);      // request en værdi og vent i 1 sekund så data kan overføres
  LokalTemp = DHT.getCelsius();  // hent kun temperaturen fra DHT11, ikke luftfugtigheden
  pinMode(DHT_PIN, INPUT);       // D3 ændres til input for at spare strøm
  digitalWrite(DHT_ONOFF, LOW);  // sluk for DHT11 lokal temperaturmåler
}

void maalADXL345Data() // aflæs accelerometer og gem x og y værdierne
{
  int x, y, z;                         // variabler til de 3 ADXL345 accelerometer retninger
  digitalWrite(ADXL_ONOFF, HIGH);      // tænd for ADXL345 accelerometer
  pinMode(ADXL_SDA, OUTPUT);           // D0 til ADXL345 SDA
  pinMode(ADXL_SCL, OUTPUT);           // D1 til ADXL345 SCL
  accelerometer.powerOn();             // Initialiser ADXL345
  accelerometer.setRangeSetting(16);   // Sæt 16 g-range
  accelerometer.readAccel(&x, &y, &z); // aflæs værdier, men jeg bruger kun x og y
  adxl345X = x;                        // hvor meget vipper brugeren i x-retningen
  adxl345Y = (65535 - y);              // hvor meget vipper brugeren i y-retningen
  pinMode(ADXL_SDA, INPUT);            // D0 til INPUT for at spare strøm ADXL345 SDA
  pinMode(ADXL_SCL, INPUT);            // D1 til INPUT for at spare strøm ADXL345 SCL
  digitalWrite(ADXL_ONOFF, LOW);       // sluk for ADXL345 accelerometer
}

float getTemperature(const char *url) // hent temperatur fra internet
{
  request.hostname = "api.openweathermap.org";
  request.path = url;
  http.get(request, response);
  if (response.status == 200)
  {
    String body = response.body;
    int startPos = body.indexOf("\"temp\":");
    if (startPos != -1)
    {
      int endPos = body.indexOf(',', startPos);
      if (endPos != -1)
      {
        String tempStr = body.substring(startPos + 7, endPos);
        tempValue = tempStr.toFloat(); // her gemmes temperatur værdien
      }
    }
  }
  return tempValue;
}

void visTemperatur(const char *sted) // tænd display og vis temperatur data
{
  digitalWrite(TFT_ONOFF, LOW);     // tænd for display
  pinMode(TFT_CS, OUTPUT);          // A2 til CS (Chip Select) på display
  pinMode(TFT_RST, OUTPUT);         // D5 forbindes til RST (reset) paa display
  pinMode(TFT_DC, OUTPUT);          // D8 til DC (Data/Command) paa display
  pinMode(TFT_MOSI, OUTPUT);        // D12 til SDA (SPI MOSI) paa display
  pinMode(TFT_SCLK, OUTPUT);        // D13 til SCL (SPI-clock) paa display
  tft.initR(INITR_BLACKTAB);        // initialiserer TFT-displayet
  tft.fillScreen(ST7735_BLACK);     // fylder hele skærmen med en sort farve
  tft.setRotation(4);               // 4 betyder at teksten vender væk fra pins
  tft.setTextColor(ST7735_MAGENTA); // Tekst farve sættes til Magenta (rød-blaa)
  tft.setTextSize(2);               // Tekst størrelse er stor
  tft.setCursor(10, 20);            // Set cursor position
  tft.println("EH5IoT-KK");         // Overskrift EH5IoT-KK
  tft.setTextColor(ST7735_WHITE);   // Tekst farve sættes til hvid
  tft.setTextSize(1);               // Tekst størrelse er lille
  snprintf(charLysNiveau, sizeof(charLysNiveau), "Lysniveau %.2f %%", (((double)lysniveau) / 30));
  // min. lys = 0, max. lysniveau er målt til 3000, så procent er valgt til (lysniveau/3000)*100
  tft.setCursor(10, 50);      // Set cursor position
  tft.println(charLysNiveau); // Udskriv Lysniveau 0 - > 2700
  snprintf(charTempLokal, sizeof(charTempLokal), "Lokalt %.2f grader", (double)LokalTemp);
  tft.setCursor(10, 80);
  tft.println(charTempLokal); // Udskriv lokal DHT11 temperatur paa displayet

  if (strcmp(sted, "Aarhus") == 0)
  {
    snprintf(charTempArh, sizeof(charTempArh), "Aarhus %.2f grader", AarhusTemperature);
    tft.setCursor(10, 110);
    tft.println(charTempArh); // Udskriv Århus-temperaturen på displayet
  }
  else if (strcmp(sted, "Koebenhavn") == 0)
  {
    snprintf(charTempCph, sizeof(charTempCph), "Kbh. %.2f grader", copenhagenTemperature);
    tft.setCursor(10, 140);
    tft.println(charTempCph); // Udskriv København-temperaturen på displayet
  }
  delay(15000);                  // Vis display data i 15 sekunder
  pinMode(TFT_CS, INPUT);        // A2 til CS (Chip Select) på display
  pinMode(TFT_RST, INPUT);       // D5 forbindes til RST (reset) paa display
  pinMode(TFT_DC, INPUT);        // D8 til DC (Data/Command) paa display
  pinMode(TFT_MOSI, INPUT);      // D12 til SDA (SPI MOSI) paa display
  pinMode(TFT_SCLK, INPUT);      // D13 til SCL (SPI-clock) paa display
  digitalWrite(TFT_ONOFF, HIGH); // sluk for display
}

