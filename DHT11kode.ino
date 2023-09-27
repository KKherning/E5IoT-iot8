#include <C:\Particle\LokalTemp3\lib\PietteTech_DHT\src\PietteTech_DHT.h>

const int DHTTYPE = DHT11;       // Sensor type DHT11/21/22/AM2301/AM2302
const int DHT_PIN = D3;          // Digital pin for communications

PietteTech_DHT DHT(DHT_PIN, DHTTYPE);

void setup()
{
  Serial.begin(9600);
  DHT.begin();
  delay(1000);
}

void loop()
{
  int result = DHT.acquireAndWait(1000); // wait up to 1 sec (default indefinitely)

  switch (result) {
  case DHTLIB_OK:
    Serial.println("OK");
    break;
  case DHTLIB_ERROR_CHECKSUM:
    Serial.println("Error\n\r\tChecksum error");
    break;
  case DHTLIB_ERROR_ISR_TIMEOUT:
    Serial.println("Error\n\r\tISR time out error");
    break;
  case DHTLIB_ERROR_RESPONSE_TIMEOUT:
    Serial.println("Error\n\r\tResponse time out error");
    break;
  case DHTLIB_ERROR_DATA_TIMEOUT:
    Serial.println("Error\n\r\tData time out error");
    break;
  case DHTLIB_ERROR_ACQUIRING:
    Serial.println("Error\n\r\tAcquiring");
    break;
  case DHTLIB_ERROR_DELTA:
    Serial.println("Error\n\r\tDelta time to small");
    break;
  case DHTLIB_ERROR_NOTSTARTED:
    Serial.println("Error\n\r\tNot started");
    break;
  default:
    Serial.println("Unknown error");
    break;
  }
  Serial.print("Humidity (%): ");
  Serial.println(DHT.getHumidity(), 8);

  Serial.print("Temperature (oC): ");
  Serial.println(DHT.getCelsius(), 8);

  delay(5000);
}