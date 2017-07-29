#include <ArduinoJson.h>
#include <Servo.h>
#include <math.h> 

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

int X_limitPin = 12;
int Y_limitPin = 13;

double X_axisMovement = 0;
double A_axisMovement = 0;
double Y_axisMovement = 0;
int gripperMovement = 0;
int B_axisMovement = 94;

int X_movementFactor = 1;
int Y_movementFactor = 1;
int A_movementFactor = 1;
int X_gap = 0;
int Y_gap = 0;
int A_gap = 0;

boolean motorsMoving = false;

void setup() {

  Serial.begin(250000);      
  
  pinMode(X_axisDirectionPin, OUTPUT);
  pinMode(X_axisMovePin, OUTPUT);
  pinMode(A_axisDirectionPin, OUTPUT);
  pinMode(A_axisMovePin, OUTPUT);
  pinMode(Y_axisDirectionPin, OUTPUT);
  pinMode(Y_axisMovePin, OUTPUT);

  pinMode(X_limitPin, INPUT);
  pinMode(Y_limitPin, INPUT);

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

  boolean maximun_X_Reached = (boolean) digitalRead(X_limitPin);
  boolean maximun_Y_Reached = (boolean) digitalRead(Y_limitPin);

  readInstructions();

  if(maximun_X_Reached && X_axisMovement < 0){
    X_axisMovement = 0;
  }else{
    X_axisMovement = moveStepperMotor(X_axisDirectionPin, X_axisMovePin, X_axisMovement, X_movementFactor, &X_gap);
  }

  if(maximun_Y_Reached && Y_axisMovement > 0){
    Y_axisMovement = 0;
  }else{
    Y_axisMovement = moveStepperMotor(Y_axisDirectionPin, Y_axisMovePin, Y_axisMovement, Y_movementFactor, &Y_gap);
  }
  
  A_axisMovement = moveStepperMotor(A_axisDirectionPin, A_axisMovePin, A_axisMovement, A_movementFactor, &A_gap);
  B_axisServo.write(B_axisMovement);
  gripperServo.write(gripperMovement);

  delayMicroseconds(100);
  digitalWrite(X_axisMovePin, LOW);
  digitalWrite(A_axisMovePin, LOW);
  digitalWrite(Y_axisMovePin, LOW);
  delayMicroseconds(100);

  if(X_axisMovement == 0 && A_axisMovement == 0 && Y_axisMovement == 0 && motorsMoving){
    //Reinitialize the factors and the gaps
    X_movementFactor = 1;
    Y_movementFactor = 1;
    A_movementFactor = 1;
    X_gap = 0;
    Y_gap = 0;
    A_gap = 0;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& feedbacks = jsonBuffer.createObject();
    feedbacks["success"] = true;  
    feedbacks.printTo(Serial);  
    Serial.print("\n");
    motorsMoving = false;
  }
  
}

double moveStepperMotor(int axisDirectionPin, int axisMovePin, double axisMovement, int factor, int* gap){

  /*
  if(axisMovement != 0){
    Serial.print("X_movementFactor : ");
    Serial.println(X_movementFactor);
    Serial.print("Y_movementFactor : ");
    Serial.println(Y_movementFactor);
    Serial.print("A_movementFactor : ");
    Serial.println(A_movementFactor);
    Serial.print("X axis Movement : ");
    Serial.println(X_axisMovement);
    Serial.print("Y axis Movement : ");
    Serial.println(Y_axisMovement);
    Serial.print("A axis Movement : ");
    Serial.println(A_axisMovement);
    Serial.print("X gap: ");
    Serial.println(X_gap);
    Serial.print("Y gap: ");
    Serial.println(Y_gap);
    Serial.print("A gap: ");
    Serial.println(A_gap);
    Serial.println("______________");
  }
  */
  
  if(axisMovement > 0){
    if(*gap%factor == 0){
      digitalWrite(axisDirectionPin, HIGH); 
      digitalWrite(axisMovePin, HIGH);
      *gap = *gap + 1;
      return --axisMovement;
    }else{
      digitalWrite(axisMovePin, LOW);
      *gap = *gap + 1;
      return axisMovement;
    }
  }else if(axisMovement < 0){
    if(*gap%factor == 0){ //Modulo are always positives
      digitalWrite(axisDirectionPin, LOW); 
      digitalWrite(axisMovePin, HIGH);
      *gap = *gap - 1;
      return ++axisMovement;
    }else{
      digitalWrite(axisMovePin, LOW);
      *gap = *gap - 1;
      return axisMovement;
    }
  }else {
    digitalWrite(axisMovePin, LOW);
    *gap = 0;
    return 0;
  }   
  
}

void readInstructions(){
  StaticJsonBuffer<500> jsonBuffer;
  String json;
  if(Serial.available() > 0) {
    json = Serial.readStringUntil('\n');
    JsonObject& commands = jsonBuffer.parseObject(json);
    if (!commands.success()) {
      Serial.println("parseObject() failed");
      return;
    }

    motorsMoving = true;
  
    X_axisMovement += commands["X_axisMovement"].as<double>();
    A_axisMovement += commands["A_axisMovement"].as<double>();
    Y_axisMovement += commands["Y_axisMovement"].as<double>();   
    B_axisMovement = commands["B_axisMovement"].as<int>(); 
    gripperMovement = commands["gripperMovement"].as<int>(); 

    //Factor calculation
    double maxAxisMovment = getMaxAxisMovment(X_axisMovement, A_axisMovement, Y_axisMovement);
    
    X_movementFactor = abs(round(maxAxisMovment / X_axisMovement));
    Y_movementFactor = abs(round(maxAxisMovment / Y_axisMovement));
    A_movementFactor = abs(round(maxAxisMovment / A_axisMovement));

    //{"X_axisMovement":0, "A_axisMovement":0, "Y_axisMovement":0, "B_axisMovement":95, "gripperMovement":60}
  }
}

double getMaxAxisMovment(double value1, double value2, double value3){
  
  value1=abs(value1);
  value2=abs(value2);
  value3=abs(value3);
 
  if(value1 >= value2 && value1 >= value3)
    return value1;
  if(value2 >= value1 && value2 >= value3)
    return value2;
  if(value3 >= value2 && value3 >= value1)
    return value3; 
} 

