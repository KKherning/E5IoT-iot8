# E5IoT projektopgave vedrørende AU-kurset E5IoT efterår 2023.

## Introduktion
Projektet omhandler et IoT system med en Particle Argon enhed, der bruger forskellige sensorer og en tilstandsmaskine til at måle lysniveau, accelerometerdata og temperatur og vise vejrtjeneste-data på et farvedisplay baseret på brugerens interaktion med enheden.

## Komponenter til projektopgaven
*	Particle Argon med Wi-Fi adgang
*	Sensor1: en temperatur sensor DHT11
*	Sensor2: en lysfølsom modstand
*	Sensor3: et ADXL345 accelerometer
*	Actuator1: et TFT farve display (ikke OLED) 1.88 inch (128*160) IPS 7P SPI HD 65K Full Color LCD.
*	Actuator2: en grøn ekstern LED som Particle Argon kan tænde via et GPIO ben. Den grønne LED tændes hvis der er lys nok, og brugeren har vippet systemet. Den grønne LED slukkes når systemet går i SLEEP mode.
*	Ekstern USB powerbank 5V 5000mAh og et eksternt LiPo genopladeligt batteri 3.7V 1800mAh med JST-PH stik hvor polariteten skulle vendes.
*	Breadboard, ledninger, LED, lysfølsommodstand, transistor og modstande.


## Funktionsbeskrivelse
Particle Argon systemet kan altid vise temperaturen lokalt på et display, når det er lyst, men systemet forbedres hvis der er internetadgang, for så kan brugeren også se vejrudsigt temperatur i Århus eller i København, baseret på hvilken vej brugeren tipper systemet.
Systemet sover i 30 sekunder hvorefter lys niveauet måles. En bruger kan eventuelt vippe systemet i x-akse eller y-akseretningen. Systemet bruger ikke ADXL345 z-aksen, men kun x og y-akseretningerne. Hvis det er mørkt så bringes systemet til at sove igen. Så hvis der er mørkt i lokalet, så sker der intet når en bruger vipper/tilter mit breadboard. Kun hvis det er lyst i lokalet, og brugeren har vippet systemet, skal der hentes en vejrudsigt vedrørende Århus eller København. 
Hvis det er lyst og brugeren vippede i x-akseretningen, så kan man i et display se vejrudsigt temperaturen i Århus. Hvis det er lyst og man vippede i y-akseretningen, så kan man se vejrudsigt temperaturen i København. Desuden er der tilføjet en DHT11 temperatursensor, som viser temperaturen lokalt. I displayet kan man desuden se lys niveauet i procent.
Den lokale temperatur fra DHT11 samt lys niveau, er det eneste der kan vises hvis Particle Argon ikke har forbindelse til internet.
