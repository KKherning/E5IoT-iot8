
// Skriver antal sekunder og blinker D6 (rød LED) og D7 (indbygget blå LED)
// compile app local -> flash app local -> serial monitor

const pin_t led = D6; // rød ekstern LED tilsluttet D6
const pin_t INDBYG_LED = D7; // indbygget blå LED
const pin_t photoresistor = A0;
const pin_t powerHIGH = A5; // bruges til at tænde ved måling
int analogvalue;
double voltage; // long float
//int count = 0;
SYSTEM_THREAD(ENABLED);
// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  pinMode(led, OUTPUT);
  pinMode(INDBYG_LED, OUTPUT);
  pinMode(photoresistor,INPUT);
  pinMode(powerHIGH, OUTPUT);
  Particle.function("led",ledToggle);
  Particle.variable("analogvalue", &analogvalue, INT);
  Particle.variable("voltage", voltage);
}

int ledToggle(String command) {

    if ((command=="on") || (command=="On") || (command=="ON"))  {
        digitalWrite(led,HIGH);
        return 1;
    }
    else if ((command=="off") || (command=="Off") || (command=="OFF")) {
        digitalWrite(led,LOW);
        return 0;
    }
    else {
        return -1;
    }

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // Toggle led hvert 1/2 sekund, som er 500ms tændt og 500ms slukket
  // dvs periode tiden er 1000ms
  //digitalWrite(MY_LED, HIGH); // giver 3.3V uden belastning, men falder til 2.6V med belastning
  digitalWrite(INDBYG_LED, LOW);
  delay(500);
  //digitalWrite(MY_LED, LOW);
  digitalWrite(INDBYG_LED, HIGH);
  delay(500);
  // increment count var, print to serial port
  //count++;
  //digitalWrite(powerHIGH, HIGH); // tænder kun når der skal måles (giver 3.3V)
  analogvalue = analogRead(photoresistor); // måler lysstyrken fra 0 til 4096
  //digitalWrite(powerHIGH, LOW); // slukker igen for at spare strøm (giver 0 V)
  voltage = analogvalue * 3.3 / 4095;
  //Serial.printf(" Efter %d sekunder er lysstyrken %lf V\n", count, voltage); // ny linje med /n
}