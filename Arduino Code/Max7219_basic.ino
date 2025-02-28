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
  matrix.setIntensity(5);  // Set brightness (current limit) 0 to 15
}

void loop() {
  matrix.clear();  // Clear the display

  matrix.setDot(0,0, 0xFF); // set top to red

  matrix.setDot(1,0, 0xFF); 
  matrix.setDot(1,2, 0xFF);

  matrix.setDot(2,0, 0xFF);

  matrix.setDot(3,0, 0xFF);
  matrix.setDot(3,2, 0xFF);

  matrix.setDot(4,0, 0xFF);

  matrix.setDot(5,0, 0xFF);
  matrix.setDot(5,2, 0xFF);

  matrix.setDot(6,0, 0xFF);

  matrix.setDot(7,0, 0xFF);
  matrix.setDot(7,2, 0xFF);

  matrix.setDot(0,7, 0xFF); // set bottom to blue
  matrix.setDot(0,5, 0xFF);

  matrix.setDot(1,7, 0xFF); 

  matrix.setDot(2,7, 0xFF);
  matrix.setDot(2,5, 0xFF);

  matrix.setDot(3,7, 0xFF);

  matrix.setDot(4,5, 0xFF);
  matrix.setDot(4,7, 0xFF);

  matrix.setDot(5,7, 0xFF);

  matrix.setDot(6,7, 0xFF);
  matrix.setDot(6,5, 0xFF);

  matrix.setDot(7,7, 0xFF);



  delay(2000);
}
