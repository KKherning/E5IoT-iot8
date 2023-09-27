# E5IoT projekt vedrørende iot8 efterår 2023.

## Komponenter til projektopgaven
* en Particle Argon
* en temperatur sensor
* en lysfølsom modstand
* en ADXL345
* et TFT farve display.

## Funktionsbeskrivelse

Når man i x-akse retningen vipper/tilter mit breadboard med de 4 enheder opkoblet, så kan man se vejrudsigten i Århus. 

Når man i y-akse retningen vipper/tilter mit breadboard med de 4 enheder opkoblet, så kan man se vejrudsigten i København. 

Jeg vil ikke bruge ADXL345 z-aksen, men kun x og y-aksen.

Kun hvis det er lyst i lokalet skal der hentes en vejrudsigt. Hvis der er mørkt i lokalet, så sker der intet når en bruger vipper/tilter mit breadboard.

Desuden vil jeg tilføje en temperatursensor, som viser temperaturen aktuelt hvis min Particle Argon ikke er forbundet til internettet. 
