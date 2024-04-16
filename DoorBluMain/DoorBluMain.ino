#include <SoftwareSerial.h>
#include <Servo.h>
#include <Wire.h> 
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C LCD(0x27,4,5);
SoftwareSerial mySerial(5, 4); // RX, TX
Servo myServo;
 
const int buttonPin = 13;
int k = 1234;
bool Open = false;
bool tapped = false;
int ledRedPin = 2;
int ledGreenPin = 3; 
int stage_display = 0;
float time_left = 0.0;


#define Password_Length 6 
char Data[Password_Length]; 
char Master[Password_Length] = "11111"; 
byte data_count = 0, master_count = 0;
bool Pass_is_good;
bool door_open = 0;
char customKey;

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte Heart[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};



byte rowPins[ROWS] = {7, 8, 9, 14};
byte colPins[COLS] = {10, 11, 12, 15};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

 
 
void setup()
{
  LCD.init(); 
  LCD.backlight();
  LCD.begin(16, 2);

  // Create new characters:
  LCD.createChar(0, Heart);
  
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);     
  pinMode(buttonPin, INPUT);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
 
  Serial.println("Goodnight moon!");
 
  delay(500);
 
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  myServo.attach(6);
  myServo.write(180);
  mySerial.print("Wake up wake up wake up wake up wake up! Wake up wake up wake up wake up wake up!");
}
 
void loop() // run over and over
{
  
  switch(stage_display){
     case 0:
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("WELCOME HOME"); 
        LCD.setCursor(6, 1);  
        LCD.print("-DOORBLU");
        LCD.write(byte(0));
        stage_display = 1;
        delay(2000);
        break;
     case 1:
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("DOORBLU"); 
        LCD.setCursor(0, 1);  
        LCD.print("WAITING FOR USER");
        stage_display = 999;
        break;
     
     default:
        break;
  }

  customKey = customKeypad.getKey();
  if (customKey){
    if (door_open){
      door_open = 0;
      digitalWrite(ledRedPin, HIGH);
      digitalWrite(ledGreenPin, LOW);
      myServo.write(180);
    }
    else {
      Data[data_count] = customKey; 
      Serial.println(Data[data_count]); 
      data_count++; 
    }
  }

  if(data_count == Password_Length-1){

    if(!strcmp(Data, Master)){
      Serial.println("Correct");
      door_open = 1;
      door_opened();
    }
    else{
      door_closed();
      Serial.println("Incorrect");
      delay(1000);
    }
    
    clearData();  
  }
  digitalWrite(ledRedPin, HIGH);
 
  bool  buttonState = digitalRead(buttonPin);
 
  if (digitalRead(buttonPin) && k != 1234 ) { // Если кнопка нажата...
    door_closed();
  }
  if (digitalRead(buttonPin) && (k == 1234 && tapped == 0)) { // Если кнопка нажата...
    door_opened();
  }  
 
  if (mySerial.available()) {
    mySerial.setTimeout(50);
    String s = mySerial.readString();
    
    Serial.println("Received: " + s);
    if (s[0] == '@') {
      k = s.substring(1, s.length() - 1).toInt();
      /*myServo.write(k);
        delay(500);*/
    }
  }
 
  if (Serial.available()) {
    String s = "";
    char c;
    while ((c = Serial.read()) != -1) {
      s += c;
      delay(10);
    }
 
    delay(10);
 
    Serial.println("Sent: " + s);
 
    mySerial.print(s);
  }
}

void door_closed(){
  digitalWrite(ledRedPin, HIGH);
  digitalWrite(ledGreenPin, HIGH);  
  delay(500);
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  delay(500);
  digitalWrite(ledRedPin, HIGH);
  digitalWrite(ledGreenPin, HIGH);  
  delay(500);
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  delay(500);
  digitalWrite(ledRedPin, HIGH);
  digitalWrite(ledGreenPin, HIGH);  
  delay(500);
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW); 
}

void door_opened(){
  digitalWrite(ledRedPin, LOW);
  myServo.write(90);
  digitalWrite(ledGreenPin, HIGH); 
  delay(2500);
  digitalWrite(ledGreenPin, LOW);
  delay(500);
  digitalWrite(ledGreenPin, HIGH); 
  delay(500);
  digitalWrite(ledGreenPin, LOW);
  delay(500);
  digitalWrite(ledGreenPin, HIGH); 
  delay(500);
  digitalWrite(ledGreenPin, LOW);
  delay(500);
  digitalWrite(ledGreenPin, HIGH); 
  delay(500);
  digitalWrite(ledGreenPin, LOW);
  myServo.write(180);
  digitalWrite(ledRedPin, HIGH);
}

void clearData(){
  while(data_count !=0){
    Data[data_count--] = 0; 
  }
  return;
}
