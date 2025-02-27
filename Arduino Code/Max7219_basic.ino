#include <MaxMatrix.h>
//link to download library
//https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/arudino-maxmatrix-library/MaxMatrix.zip

#define maxDisplays 1  // Adjust for the number of MAX7219 chips
#define dataPin 11     // DIN pin
#define loadPin 10     // CS pin
#define clockPin 13    // CLK pin

MaxMatrix matrix(dataPin, loadPin, clockPin, maxDisplays);

void setup() {
  matrix.init();
  matrix.setIntensity(8);  // Set brightness (current limit) 0 to 15
}

void loop() {
  matrix.clear();  // Clear the display
  matrix.setDot(0,1, 0xFF); // set top right blue
  matrix.setDot(1,1, 0xFF);
  matrix.setDot(2,1, 0xFF);
  matrix.setDot(0,3, 0xFF);
  matrix.setDot(1,3, 0xFF);
  matrix.setDot(2,3, 0xFF);
  matrix.setDot(0,5, 0xFF);
  matrix.setDot(1,5, 0xFF);
  matrix.setDot(2,5, 0xFF);
  matrix.setDot(0,7, 0xFF);
  matrix.setDot(1,7, 0xFF);
  matrix.setDot(2,7, 0xFF);

  matrix.setDot(7,0, 0xFF);
  matrix.setDot(6,0, 0xFF);
  matrix.setDot(5,0, 0xFF);
  matrix.setDot(7,2, 0xFF);
  matrix.setDot(6,2, 0xFF);
  matrix.setDot(5,2, 0xFF);
  matrix.setDot(7,4, 0xFF);
  matrix.setDot(6,4, 0xFF);
  matrix.setDot(5,4, 0xFF);
  matrix.setDot(7,6, 0xFF);
  matrix.setDot(6,6, 0xFF);
  matrix.setDot(5,6, 0xFF); // set bottom left red



  delay(500);
}
