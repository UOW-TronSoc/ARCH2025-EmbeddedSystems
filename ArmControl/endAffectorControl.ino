const int mPin1 = 25;
const int mPin2 = 33;
const int enPin = 26;

const int potPin = 32;
const int buttonCWPin = 14;
const int buttonACWPin = 12;

const int maxPot = 4095;
const int margin=100;
void setup() {
  // sets the pins as outputs:
  pinMode(potPin, INPUT);
  pinMode(mPin1, OUTPUT);
  pinMode(mPin2, OUTPUT);
  pinMode(enPin, OUTPUT);
  pinMode(buttonCWPin, INPUT);
  pinMode(buttonACWPin, INPUT);
  Serial.begin(115200);

  // testing
  Serial.println("Testing DC Motor...");
  digitalWrite(enPin,HIGH);
}

void loop() {
  int potPos = analogRead(potPin);
  //Serial.println(analogRead(potPin));
  if (digitalRead(buttonCWPin)==HIGH && potPos<maxPot-margin){
    
    digitalWrite(mPin1,HIGH);
    digitalWrite(mPin2,LOW);
    //Serial.println("CW button pressed");
  }
  else if (digitalRead(buttonACWPin)==HIGH && potPos>margin){
    digitalWrite(mPin1,LOW);
    digitalWrite(mPin2,HIGH);
    //Serial.println("ACW button pressed");
    }
  else {
    digitalWrite(mPin1,LOW);
    digitalWrite(mPin2,LOW);
    //Serial.println("No buttons pressed");
  }

}
