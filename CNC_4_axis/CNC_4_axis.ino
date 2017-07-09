#include <ArduinoJson.h>
#include <Servo.h>

int X_axisDirectionPin = 2;
int X_axisMovePin = 3;
int A_axisDirectionPin = 6;
int A_axisMovePin = 7;
int Y_axisDirectionPin = 4;
int Y_axisMovePin = 5;
int gripperPin = 9;
Servo gripperServo;
int B_axisPin = 10;
Servo B_axisServo;

int X_limitPin = 13;
int Y_limitPin = 12;

double X_axisMovement = 0;
double A_axisMovement = 0;
double Y_axisMovement = 0;
int gripperMovement = 0;
int B_axisMovement = 0;

boolean motorsMoving = false;

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

  gripperServo.attach(gripperPin);
  B_axisServo.attach(B_axisPin);
}

void loop() {

  boolean maximun_X_Reached = !((boolean) digitalRead(X_limitPin));
  boolean maximun_Y_Reached = (boolean) digitalRead(Y_limitPin);

  readInstructions(&X_axisMovement, &A_axisMovement, &Y_axisMovement, &B_axisMovement, &gripperMovement);

  if(maximun_X_Reached && X_axisMovement < 0){
    X_axisMovement = 0;
  }else{
    X_axisMovement = moveStepperMotor(X_axisDirectionPin, X_axisMovePin, X_axisMovement);
  }

  if(maximun_Y_Reached && Y_axisMovement > 0){
    Y_axisMovement = 0;
  }else{
    Y_axisMovement = moveStepperMotor(Y_axisDirectionPin, Y_axisMovePin, Y_axisMovement);
  }
  
  A_axisMovement = moveStepperMotor(A_axisDirectionPin, A_axisMovePin, A_axisMovement);
  B_axisServo.write(B_axisMovement);
  gripperServo.write(gripperMovement);
  
  delayMicroseconds(100);
  digitalWrite(X_axisMovePin, LOW);
  digitalWrite(A_axisMovePin, LOW);
  digitalWrite(Y_axisMovePin, LOW);
  delayMicroseconds(100);

  if(X_axisMovement == 0 && A_axisMovement == 0 && Y_axisMovement == 0 && motorsMoving){
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& feedbacks = jsonBuffer.createObject();
    feedbacks["success"] = true;  
    feedbacks.printTo(Serial);  
    Serial.print("\n");
    motorsMoving = false;
  }
  
}

double moveStepperMotor(int axisDirectionPin, int axisMovePin, double axisMovement){
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

void readInstructions(double *X_axisMovement, double *A_axisMovement, double *Y_axisMovement, int *B_axisMovement, int *gripperMovement){
  StaticJsonBuffer<200> jsonBuffer;
  String json;
  if(Serial.available() > 0) {
    json = Serial.readStringUntil('\n');
    JsonObject& commands = jsonBuffer.parseObject(json);
    if (!commands.success()) {
      Serial.println("parseObject() failed");
      return;
    }

    motorsMoving = true;
  
    *X_axisMovement += commands["X_axisMovement"].as<double>();
    *A_axisMovement += commands["A_axisMovement"].as<double>();
    *Y_axisMovement += commands["Y_axisMovement"].as<double>();   
    *B_axisMovement = commands["B_axisMovement"].as<int>(); 
    *gripperMovement = commands["gripperMovement"].as<int>(); 

    //{"X_axisMovement":0, "A_axisMovement":0, "Y_axisMovement":0, "B_axisMovement":20, "gripperMovement":60}
  }
}


