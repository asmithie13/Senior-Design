//Simple application to test functionality of the HC-05 Bluetooth module:

//Initialize serial communication:
void setup() {
  //Set Baud rate:
  Serial.begin(9600);
  Serial1.begin(9600); //Baud rate for HC-05 (Connect to appropriate MEGA pins)

  //TEST:
  Serial.print("BEGIN");

  //Initialize output pin:
  pinMode(50, INPUT);
}

//Check for serial input:
void loop() {
  //Check if serial data is received:
  if(Serial1.available()){
    char data=Serial1.read(); //Read data
    Serial.print("Received from HC-05: "); //Print the data to the serial monitor
    Serial.println(data);

    //Blink the LED if serial data is recieved:
    digitalWrite(50, HIGH);
    delay(500);
    digitalWrite(50, LOW);
  } 
}

