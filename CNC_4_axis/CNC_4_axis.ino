#include <ArduinoJson.h>

int X_axisDirectionPin = 2;
int X_axisMovePin = 3;
int A_axisDirectionPin = 6;
int A_axisMovePin = 7;
int Y_axisDirectionPin = 4;
int Y_axisMovePin = 5;

int X_axisMovement = 0;
int A_axisMovement = 0;
int Y_axisMovement = 0;

void setup() {

  Serial.begin(250000);      
  
  pinMode(X_axisDirectionPin, OUTPUT);
  pinMode(X_axisMovePin, OUTPUT);
  pinMode(A_axisDirectionPin, OUTPUT);
  pinMode(A_axisMovePin, OUTPUT);
  pinMode(Y_axisDirectionPin, OUTPUT);
  pinMode(Y_axisMovePin, OUTPUT);
  
  digitalWrite(X_axisDirectionPin, LOW);
  digitalWrite(X_axisMovePin, LOW);
  digitalWrite(A_axisDirectionPin, LOW);
  digitalWrite(A_axisMovePin, LOW);
  digitalWrite(Y_axisDirectionPin, LOW);
  digitalWrite(Y_axisMovePin, LOW);
}

void loop() {

  readInstructions(&X_axisMovement, &A_axisMovement, &Y_axisMovement);

  X_axisMovement = moveStepperMotor(X_axisDirectionPin, X_axisMovePin, X_axisMovement);
  A_axisMovement = moveStepperMotor(A_axisDirectionPin, A_axisMovePin, A_axisMovement);
  Y_axisMovement = moveStepperMotor(Y_axisDirectionPin, Y_axisMovePin, Y_axisMovement);
  
  delayMicroseconds(100);
  digitalWrite(X_axisMovePin, LOW);
  digitalWrite(A_axisMovePin, LOW);
  digitalWrite(Y_axisMovePin, LOW);
  delayMicroseconds(100);
  
}

int moveStepperMotor(int axisDirectionPin, int axisMovePin, int axisMovement){
  if(axisMovement > 0){
    digitalWrite(axisDirectionPin, HIGH); 
    digitalWrite(axisMovePin, HIGH);
    return --axisMovement;
  }else if(axisMovement < 0){
    digitalWrite(axisDirectionPin, LOW); 
    digitalWrite(axisMovePin, HIGH);
    return ++axisMovement;
  }else {
      digitalWrite(axisMovePin, LOW);
      return 0;
  }
}


void readInstructions(int *X_axisMovement, int *A_axisMovement, int *Y_axisMovement){
  StaticJsonBuffer<200> jsonBuffer;
  String json;
  if(Serial.available() > 0) {
    json = Serial.readStringUntil('\n');
    JsonObject& commands = jsonBuffer.parseObject(json);
    if (!commands.success()) {
      Serial.println("parseObject() failed");
      return;
    }
  
    *X_axisMovement += commands["X_axisMovement"].as<long>();
    *A_axisMovement += commands["A_axisMovement"].as<long>();
    *Y_axisMovement += commands["Y_axisMovement"].as<long>();   

    //{"X_axisMovement":0, "A_axisMovement":0, "Y_axisMovement":0}
  }
}
