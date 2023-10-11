#include "application.h"
#include "ADXL345.h"                                  // ADXL345 accelerometer
#include "HttpClient.h"                               // vente vejudsigt fra internet
#include "Adafruit_ST7735.h"                          // TFT farve display 1.88 inch (128*160)
#include <..\lib\PietteTech_DHT\src\PietteTech_DHT.h> // DHT11

const int photoresistor = A0; // Analogt input til lysfoelsom modstand
// const int ADXL_ONOFF = A1;    // Digital pin til at tænde og slukke for ADXL345 enheden
const int TFT_CS = A2;        // forbindes til CS (Chip Select) paa display
                              // Argon D0 SDA er forbundet til ADXL345 SDA (standard inde i ADXL345.h)
                              // Argon D1 SCL er forbundet til ADXL345 SCL (standard inde i ADXL345.h)
const int LYSMAAL_ONOFF = A3; // Digital pin til at tænde og slukke for lysmålingen (lysmodstand)
// const int DHT_ONOFF = D2;     // Digital pin til at tænde og slukke for DHT11 enheden
const int DHT_PIN = D3; // Digital pin til DHT11 kommunikation (ben 2 på DHT11)
// const int TFT_ONOFF = D4;     // Digital pin til at tænde og slukke for TFT display
const int TFT_RST = D5;    // forbindes til RST (reset) paa display
const int GROEN_LED = D6;  // Groen ekstern LED tilsluttet D6
const int INDBYG_LED = D7; // Indbygget blaa LED
const int TFT_DC = D8;     // forbindes til DC (Data/Command) paa display
const int TFT_MOSI = D12;  // forbindes til SDA (SPI MOSI) paa display
const int TFT_SCLK = D13;  // forbindes til SCL (SPI-clock) paa display

int adxl345X = 0;                   // Variable til at gemme ADXL345 data for x-retning
int adxl345Y = 0;                   // Variable til at gemme ADXL345 data for x-retning
int lysniveau = 0;                  // Variable til at gemme data fra analogt input til lysfoelsom modstand
const int LYSNIVEAU_TRESHOLD = 100; // Lysniveau tærskelværdi (er det lyst nok til visning)
float AarhusTemperature = 0.0;      // variabel til Århus-temperatur fra api.openweathermap.org
float copenhagenTemperature = 0.0;  // variabel til København-temperatur fra api.openweathermap.org
float tempValue = 0.0;              // variabel til at gemme tempValue

char Buffer1[20] = ""; // Global buffer 1 til lysdata
char Buffer2[20] = ""; // Global buffer 2 til lokal temperatur
char Buffer3[20] = ""; // Global buffer 3 til temperatur Århus
char Buffer4[20] = ""; // Global buffer 4 til temperatur København

ADXL345 accelerometer;              // ADXL245 accelerometer
PietteTech_DHT DHT(DHT_PIN, DHT11); // lokal temperatursensor DHT11

SystemSleepConfiguration sleepconfig;
HttpClient http; // variabel http, som er en instans af HttpClient-klassen.
http_request_t request;
http_response_t response;

// Vejr-API-variabler
const char *apiKey = "?????????????????????????????????";                   // Min vejr API-noegle til api.openweathermap.org
const char *baseUrl = "http://api.openweathermap.org/data/2.5/weather?q="; // Grund URL
float LokalTemp = 0.0;                                                     // variabel til at opbevare DHT11 temperatur data
char urlAarhus[256];                                                       // variabel til den komplette URL til at hente temperatur i Århus
char urlCopenhagen[256];                                                   // variabel til den komplette URL til at hente temperatur i København

// Opretter en Adafruit_ST7735-objektinstans som hedder tft
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Definerer de 4 tilstande for min tilstandsmaskine
enum State
{
  SLEEP,   // sov i 30 sekunder, vågn op og skift til MAALLYS
  MAALLYS, // hvis det er lyst så mål lokal temp, og afgør Århus eller København
  VISARH,  // vis overskrift, lokal DHT11 temperatur samt temp. i Århus, i 15s
  VISCpH   // vis overskrift, lokal DHT11 temperatur samt temp. i København, i 15s
};

State currentState = MAALLYS; // Initialiser tilstanden til MAALLYS

void setup()
{
  pinMode(photoresistor, INPUT); // A0 Analogt input til lysfølsom modstand
  // pinMode(ADXL_ONOFF, OUTPUT);    // A1 til at tænde og slukke for ADXL345 enheden
  pinMode(TFT_CS, OUTPUT);        // A2 forbindes til CS (Chip Select) på display
  pinMode(LYSMAAL_ONOFF, OUTPUT); // A3 til at tænde og slukke for lysmåling enheden
  // pinMode(DHT_ONOFF, OUTPUT);     // D2 til at tænde og slukke for DHT11 enheden
  pinMode(DHT_PIN, OUTPUT); // D3 skriver først til DHT11, og læser senere fra (dvs INPUT)
  // pinMode(TFT_ONOFF, OUTPUT);     // D4 til at tænde og slukke for display enheden
  pinMode(TFT_RST, OUTPUT);    // D5 forbindes til RST (reset) paa display
  pinMode(GROEN_LED, OUTPUT);  // D6 Grøn ekstern LED tilsluttet D6
  pinMode(INDBYG_LED, OUTPUT); // D7 Indbygget blå LED
  pinMode(TFT_DC, OUTPUT);     // D8 forbindes til DC (Data/Command) paa display
  pinMode(TFT_MOSI, OUTPUT);   // D12 forbindes til SDA (SPI MOSI) paa display
  pinMode(TFT_SCLK, OUTPUT);   // D13 forbindes til SCL (SPI-clock) paa display

  DHT.begin(); // Initialiser DHT11

  accelerometer.powerOn();           // Initialiser ADXL345
  accelerometer.setRangeSetting(16); // 16 g-range

  sleepconfig.mode(SystemSleepMode::STOP).duration(30000); // STOP i 30000ms = 30 sekunder

  // Initialiser vejr-API URL'er for Århus og København
  snprintf(urlAarhus, sizeof(urlAarhus), "%sAarhus,dk&units=metric&appid=%s", baseUrl, apiKey);
  snprintf(urlCopenhagen, sizeof(urlCopenhagen), "%sCopenhagen,dk&units=metric&appid=%s", baseUrl, apiKey);

  tft.initR(INITR_BLACKTAB);    // initialiserer TFT-displayet med en specifik konfiguration
  tft.fillScreen(ST7735_BLACK); // fylder hele skærmen med en sort farve
  tft.setRotation(4);           // 4 betyder at teksten vender væk fra forbindelses pins
}

void loop()
{
  switch (currentState)
  {
  case SLEEP:
    digitalWrite(INDBYG_LED, LOW); // sluk blå LED på vej ind i ny state
    tft.fillScreen(ST7735_BLACK);  // fylder hele skærmen med en sort farve
    digitalWrite(GROEN_LED, LOW);  // sluk grøn ekstern LED
    // digitalWrite(DHT_ONOFF, LOW);     // sluk for DHT11
    // digitalWrite(TFT_ONOFF, LOW);     // sluk for display
    // digitalWrite(ADXL_ONOFF, LOW);    // sluk for ADXL345 accelerometer
    digitalWrite(LYSMAAL_ONOFF, LOW); // sluk for lysmåling
    System.sleep(sleepconfig); // stop mode, STOP i 30 sekunder og vågn op igen
    currentState = MAALLYS;         // Skift tilstand til MAALLYS efter søvn
    digitalWrite(INDBYG_LED, HIGH); // tænd blå LED på vej ud af ny state
    break;

  case MAALLYS:
    digitalWrite(INDBYG_LED, LOW);         // sluk blå LED på vej ind i ny state
    digitalWrite(LYSMAAL_ONOFF, HIGH);     // tænd for lysmåling
    lysniveau = analogRead(photoresistor); // mål om det er lyst
    digitalWrite(LYSMAAL_ONOFF, LOW);      // sluk for lysmåling
    if (lysniveau < LYSNIVEAU_TRESHOLD)    // hvis det er mørkere end grænseværdien?
    {
      currentState = SLEEP; // hvis det er mørkt så sov igen i 30 sekunder
    }
    else // hvis det er lyst
    {
      digitalWrite(GROEN_LED, HIGH); // tænd grøn ekstern LED
      // digitalWrite(ADXL_ONOFF, HIGH);         // tænd for ADXL345 accelerometer
      maalADXL345Data(); // aflæs accelerometer
      // digitalWrite(ADXL_ONOFF, LOW);          // sluk for ADXL345 accelerometer
      // digitalWrite(DHT_ONOFF, HIGH);          // tænd for DHT11 lokal temperaturmåler
      delay(1000);     // giv DHT11 1 sekund til at starte op
      maalLokalTemp(); // aflæs lokal temperatur på DHT11
      // digitalWrite(DHT_ONOFF, LOW);           // sluk for DHT11 lokal temperaturmåler
      if ((adxl345Y > 10) or (adxl345X > 10)) // vipper brugeren med systemet
      {
        if (adxl345Y > adxl345X) // vipper brugeren mest i x-retning eller mest i y-retning?
        {
          currentState = VISCpH; // brugeren vipper mest i y-retning og vælger derved København
        }
        else
        {
          currentState = VISARH; // brugeren vipper mest i x-retning og vælger derved Århus
        }
      }
      else
        currentState = SLEEP;
    }
    digitalWrite(INDBYG_LED, HIGH); // tænd blå LED på vej ud af ny state
    break;

  case VISARH:
    digitalWrite(INDBYG_LED, LOW); // sluk blå LED på vej ind i ny state
    // digitalWrite(TFT_ONOFF, HIGH); // tænd for display
    AarhusTemperature = getTemperature(urlAarhus); // hent temp fra internet
    delay(1000);                                   // vent 1 sekund på at data hentes og behandles
    visTemperatur("Aarhus");
    // digitalWrite(TFT_ONOFF, LOW); // sluk for display
    currentState = SLEEP;
    digitalWrite(INDBYG_LED, HIGH); // tænd blå LED på vej ud af ny state
    break;

  case VISCpH:
    digitalWrite(INDBYG_LED, LOW); // sluk blå LED på vej ind i ny state
    // digitalWrite(TFT_ONOFF, HIGH); // tænd for display
    copenhagenTemperature = getTemperature(urlCopenhagen); // hent temp fra internet
    delay(1000);                                           // vent 1 sekund på at data hentes og behandles
    visTemperatur("Koebenhavn");
    // digitalWrite(TFT_ONOFF, LOW); // sluk for display
    currentState = SLEEP;
    digitalWrite(INDBYG_LED, HIGH); // tænd blå LED på vej ud af ny state
    break;
  }
}

void maalADXL345Data() // aflæs accelerometer og gem x og y værdierne
{
  int x, y, z;                         // variabler til de 3 ADXL345 accelerometer retninger
  accelerometer.readAccel(&x, &y, &z); // aflæs værdier, men jeg bruger kun x og y
  adxl345X = x;                        // hvor meget vipper brugeren i x-retningen
  adxl345Y = (65535 - y);              // hvor meget vipper brugeren i y-retningen
}

void visTemperatur(const char *sted) // tænd display og vis temperatur data
{
  tft.setTextColor(ST7735_MAGENTA); // Tekst farve sættes til Magenta (rød-blaa)
  tft.setTextSize(2);               // Tekst størrelse er stor
  tft.setCursor(10, 20);            // Set cursor position
  tft.println("EH5IoT-KK");         // Overskrift EH5IoT-KK
  tft.setTextColor(ST7735_WHITE);   // Tekst farve sættes til hvid
  tft.setTextSize(1);               // Tekst størrelse er lille
  snprintf(Buffer1, sizeof(Buffer1), "Lysniveau %.2f %%", (((double)lysniveau) / 30));
  // min. lys = 0, max. lysniveau er målt til 3000, så procent er (lysniveau/3000)*100
  tft.setCursor(10, 50); // Set cursor position
  tft.println(Buffer1);  // Udskriv Lysniveau 0 - > 2700
  snprintf(Buffer2, sizeof(Buffer2), "Lokalt %.2f grader", (double)LokalTemp);
  tft.setCursor(10, 80);
  tft.println(Buffer2); // Udskriv lokal DHT11 temperatur paa displayet

  if (strcmp(sted, "Aarhus") == 0)
  {
    snprintf(Buffer3, sizeof(Buffer3), "Aarhus %.2f grader", AarhusTemperature);
    tft.setCursor(10, 110);
    tft.println(Buffer3); // Udskriv Aarhus-temperaturen paa displayet
    delay(15000);         // Vis display data i 15 sekunder
  }
  else if (strcmp(sted, "Koebenhavn") == 0)
  {
    snprintf(Buffer4, sizeof(Buffer4), "Kbh. %.2f grader", copenhagenTemperature);
    tft.setCursor(10, 140);
    tft.println(Buffer4); // Udskriv Koebenhavn-temperaturen paa displayet
    delay(15000);         // Vis display data i 15 sekunder
  }
}

float getTemperature(const char *url) // hent temperatur fra internet
{
  request.hostname = "api.openweathermap.org";
  request.path = url;
  http.get(request, response);
  delay(1000); // Giv tid til at fuldføre HTTP-anmodningen
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

void maalLokalTemp() // måling med DHT11
{
  DHT.acquireAndWait(1000);     // request en værdi og vent i 1 sekund så data kan overføres
  delay(1000); // vent i 1 sekund før data udtrækkes
  LokalTemp = DHT.getCelsius(); // hent kun temperaturen fra DHT11, ikke luftfugtigheden
}
