#include <SoftwareSerial.h>
#include <Servo.h>
#include <Wire.h> 
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C LCD(0x27,4,5);
SoftwareSerial mySerial(5, 4); // RX, TX
Servo myServo;
 
const int buttonPin = 13;
int k = 123;
bool Open = false;
bool tapped = false;
int ledRedPin = 2;
int ledGreenPin = 3; 
int stage_display = 0;
float time_left = 0.0;


#define Password_Length 6 
char Data[Password_Length]; 
char Master[Password_Length] = "12345"; 
byte data_count = 0, master_count = 0;
bool Pass_is_good;
bool door_open = 0;
bool enter_maj_min = false;
char customKey;

String MainMajor = "1000";
String MainMinor = "1000";
String MainUUID = "";

const byte ROWS = 4;
const byte COLS = 4;

long int last_time = 0; 
long int last_time_success = 0;

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
  /*if(millis() - last_time > 2000){
    mySerial.print("AT+DISI?");
    last_time = millis();
    if(k == 1234){
      last_time_success = millis();
    }
  }
  if(millis() - last_time_success > 8000){
    k = 123;  
  }*/
  
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
        LCD.write(byte(0));
        LCD.setCursor(0, 1);  
        LCD.print("WAITING FOR USER");
        stage_display = 999;
        break;
     case 2:
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("ENTER PASSWORD");
        LCD.setCursor(0, 1);
        for(int i = 0; i < data_count; i++){
          LCD.print('*');
        }
        stage_display = 999;
        break;
     case 3:
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("ENTER MAJ. MIN.");
        LCD.setCursor(0, 1);
        for(int i = 0; i < data_count; i++){
          LCD.print('*');
        }
        delay(500);
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
      stage_display = 2; 
    }
  }

  if(enter_maj_min == true && data_count == 4){
    MainMajor = "";
    MainMajor += Data[0];
    MainMajor += Data[1];
    MainMinor = "";
    MainMinor += Data[2];
    MainMinor += Data[3];
    enter_maj_min = false;
    stage_display = 1;
    clearData();
  }
  
  if(data_count == Password_Length-1){
    
    if(Data[0] == '1' && Data[1] == '1' && Data[2] == '1' && Data[3] == '1' && Data[4] == '1'){
      stage_display = 3;
      enter_maj_min = true;
    }
    else if(!strcmp(Data, Master)){
      Serial.println("Correct");
      door_open = 1;
      stage_display = 1;
      door_opened();
    }
    else{
      stage_display = 1;
      door_closed_pin();
      Serial.println("Incorrect");
      delay(1000);
    }
    
    clearData();  
  }
  digitalWrite(ledRedPin, HIGH);
 
  bool  buttonState = digitalRead(buttonPin);
 
  if (digitalRead(buttonPin) && k != 1234 ) { // Если кнопка нажата...
    door_closed_touch();
    stage_display = 1;
  }
  if (digitalRead(buttonPin) && (k == 1234 && tapped == 0)) { // Если кнопка нажата...
    door_opened();
    stage_display = 1;
  }  
 
  if (mySerial.available()) {
    mySerial.setTimeout(50);
    String s = mySerial.readString();

    if(s.substring(0, 8) == "OK+DISC:"){
      String UUID = s.substring(17,49);
      int Major = 0;
      int Minor = 0;
      int degree = 1;
      
      for(int i = 53; i >= 50; i--){
        String t_c = "";
        t_c += s[i];
        int t_i;

        if(t_c == "A"){
          t_i = 10;
        } else if(t_c == "B"){
          t_i = 11;
        } else if(t_c == "C"){
          t_i = 12;
        } else if(t_c == "D"){
          t_i = 13;
        } else if(t_c == "E"){
          t_i = 14;
        } else if(t_c == "F"){
          t_i = 15;
        } else if(t_c == "1"){
          t_i = 1;
        } else if(t_c == "2"){
          t_i = 2;
        } else if(t_c == "3"){
          t_i = 3;
        } else if(t_c == "4"){
          t_i = 4;
        } else if(t_c == "5"){
          t_i = 5;
        } else if(t_c == "6"){
          t_i = 6;
        } else if(t_c == "7"){
          t_i = 7;
        } else if(t_c == "8"){
          t_i = 8;
        } else if(t_c == "9"){
          t_i = 9;
        } else if(t_c == "0"){
          t_i = 0;
        }
        
        Major += degree * t_i;

        degree*=16;
      }

      degree = 1;
      for(int i = 57; i >= 54; i--){
        String t_c = "";
        t_c += s[i];
        int t_i;

        if(t_c == "A"){
          t_i = 10;
        } else if(t_c == "B"){
          t_i = 11;
        } else if(t_c == "C"){
          t_i = 12;
        } else if(t_c == "D"){
          t_i = 13;
        } else if(t_c == "E"){
          t_i = 14;
        } else if(t_c == "F"){
          t_i = 15;
        } else if(t_c == "1"){
          t_i = 1;
        } else if(t_c == "2"){
          t_i = 2;
        } else if(t_c == "3"){
          t_i = 3;
        } else if(t_c == "4"){
          t_i = 4;
        } else if(t_c == "5"){
          t_i = 5;
        } else if(t_c == "6"){
          t_i = 6;
        } else if(t_c == "7"){
          t_i = 7;
        } else if(t_c == "8"){
          t_i = 8;
        } else if(t_c == "9"){
          t_i = 9;
        } else if(t_c == "0"){
          t_i = 0;
        }
        
        
        Minor += degree * t_i;

        degree*=16;
      }
      
      //Serial.println(UUID);
      //Serial.println(Major);
      //Serial.println(Minor);
      
      if(String(Minor) == MainMinor && String(Major) == MainMajor){
        MainUUID = UUID;
      }
      if(MainUUID == UUID){
        k = 1234;
        Serial.println("FIND DEVICE");
      }
    }
    
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

void door_closed_touch(){
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("KEY NOT FOUND");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 3.5 SEC");
  digitalWrite(ledRedPin, HIGH);
  digitalWrite(ledGreenPin, HIGH);  
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("KEY NOT FOUND");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 3.0 SEC");
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("KEY NOT FOUND");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 2.5 SEC");
  digitalWrite(ledRedPin, HIGH);
  digitalWrite(ledGreenPin, HIGH);  
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("KEY NOT FOUND");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 2.0 SEC");
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("KEY NOT FOUND");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 1.5 SEC");
  digitalWrite(ledRedPin, HIGH);
  digitalWrite(ledGreenPin, HIGH);  
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("KEY NOT FOUND");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 1.0 SEC");
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("KEY NOT FOUND");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 0.5 SEC");
  digitalWrite(ledRedPin, HIGH);
  digitalWrite(ledGreenPin, HIGH);  
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("KEY NOT FOUND");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 0.0 SEC");
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW); 
  clearData(); 
}

void door_closed_pin(){
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("INCORRECT PIN");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 3.5 SEC");
  digitalWrite(ledRedPin, HIGH);
  digitalWrite(ledGreenPin, HIGH);  
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("INCORRECT PIN");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 3.0 SEC");
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("INCORRECT PIN");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 2.5 SEC");
  digitalWrite(ledRedPin, HIGH);
  digitalWrite(ledGreenPin, HIGH);  
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("INCORRECT PIN");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 2.0 SEC");
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("INCORRECT PIN");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 1.5 SEC");
  digitalWrite(ledRedPin, HIGH);
  digitalWrite(ledGreenPin, HIGH);  
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("INCORRECT PIN");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 1.0 SEC");
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("INCORRECT PIN");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 0.5 SEC");
  digitalWrite(ledRedPin, HIGH);
  digitalWrite(ledGreenPin, HIGH);  
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("INCORRECT PIN");
        LCD.setCursor(0, 1);
        LCD.print("RETRY IN 0.0 SEC");
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW); 
  clearData(); 
}

void door_opened(){
  myServo.write(90);
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, HIGH); 
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("ACCESS GRANTED");
        LCD.setCursor(0, 1);
        LCD.print("OPEN FOR 5.0 SEC");
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("ACCESS GRANTED");
        LCD.setCursor(0, 1);
        LCD.print("OPEN FOR 4.5 SEC");
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("ACCESS GRANTED");
        LCD.setCursor(0, 1);
        LCD.print("OPEN FOR 4.0 SEC");
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("ACCESS GRANTED");
        LCD.setCursor(0, 1);
        LCD.print("OPEN FOR 3.5 SEC");
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("ACCESS GRANTED");
        LCD.setCursor(0, 1);
        LCD.print("OPEN FOR 3.0 SEC");
  digitalWrite(ledGreenPin, LOW);
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("ACCESS GRANTED");
        LCD.setCursor(0, 1);
        LCD.print("OPEN FOR 2.5 SEC");
  digitalWrite(ledGreenPin, HIGH); 
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("ACCESS GRANTED");
        LCD.setCursor(0, 1);
        LCD.print("OPEN FOR 2.0 SEC");
  digitalWrite(ledGreenPin, LOW);
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("ACCESS GRANTED");
        LCD.setCursor(0, 1);
        LCD.print("OPEN FOR 1.5 SEC");
  digitalWrite(ledGreenPin, HIGH); 
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("ACCESS GRANTED");
        LCD.setCursor(0, 1);
        LCD.print("OPEN FOR 1.0  SEC");
  digitalWrite(ledGreenPin, LOW);
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("ACCESS GRANTED");
        LCD.setCursor(0, 1);
        LCD.print("OPEN FOR 0.5 SEC");
  digitalWrite(ledGreenPin, HIGH); 
  delay(500);
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("ACCESS GRANTED");
        LCD.setCursor(0, 1);
        LCD.print("OPEN FOR 0.0 SEC");
  digitalWrite(ledGreenPin, LOW);
  myServo.write(180);
  digitalWrite(ledRedPin, HIGH);
  stage_display = 1;
  clearData();
}

void clearData(){
  while(data_count !=0){
    Data[data_count--] = 0; 
  }
  return;
}
