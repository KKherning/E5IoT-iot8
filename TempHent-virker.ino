#include <HttpClient.h> //  inkluderer eksternt bibliotek, så HTTP-kommunikation kan bruges.
// Indeholder funktioner og klasser, der gør det muligt at foretage HTTP-anmodninger og
// arbejde med webbaserede tjenester 

HttpClient http; // Vi opretter en variabel ved navn http, som er en instans af HttpClient-klassen.

const char* apiKey = "?????????????????????"; // Min vejr API-nøgle til api.openweathermap.org
const char* baseUrl = "http://api.openweathermap.org/data/2.5/weather?q="; // grund URL

char urlAarhus[256];
char urlCopenhagen[256];

float aarhusTemperature = 0.0;
float copenhagenTemperature = 0.0;

void setup() {
  Serial.begin(9600);
  snprintf(urlAarhus, sizeof(urlAarhus), "%sAarhus,dk&units=metric&appid=%s", baseUrl, apiKey); // urlAarhus
  snprintf(urlCopenhagen, sizeof(urlCopenhagen), "%sCopenhagen,dk&units=metric&appid=%s", baseUrl, apiKey); // urlCopenhagen
}

void loop() {
  if (millis() % 30000 == 0) {
    getTemperature(urlAarhus, aarhusTemperature);
    getTemperature(urlCopenhagen, copenhagenTemperature);
  }
}

void getTemperature(const char* url, float& temperature) {
  http_request_t request;
  http_response_t response;
  request.hostname = "api.openweathermap.org"; // Sæt værtsnavnet uden "http://"
  request.path = url; // Sæt stien uden "http://"
  float tempValue = 0.0; // Midlertidig variabel til at opbevare temperaturen
  http.get(request, response); // Foretag HTTP GET-anmodningen 
  if (response.status == 200) { //  check om HTTP-anmodningen var vellykket (HTTP-statuskode 200) 
    String body = response.body;
    int startPos = body.indexOf("\"temp\":"); // find der hvor temperaturen står
    if (startPos != -1) {
      int endPos = body.indexOf(',', startPos);
      if (endPos != -1) {
        String tempStr = body.substring(startPos + 7, endPos);
        tempValue = tempStr.toFloat();
        // Offentliggør temperaturdataen som en Particle-event
        String event_name = "temperature_data";
        String data = String("Temperature: ") + String(tempValue) + String("°C");
        Particle.publish(event_name, data);
      }
    }
    // Opdater den eksterne temperature-variabel
    temperature = tempValue;
  } else {
    Serial.print("HTTP GET failed. Status code: ");
    Serial.println(response.status);
  }
  delay(1000); // Give tid til at fuldføre HTTP-anmodningen
}