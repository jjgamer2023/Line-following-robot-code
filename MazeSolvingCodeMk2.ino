 /*
   Maze Solving Code

   Updated: 2021-10-11 Kim Cornett, John Brown University, kcornett@jbu.edu

   Parts List:
   - Arduino Nano
   - TB6612FNG Motor Driver

   Record your notes here:



*/
#include <Servo.h>
Servo myservo;

const int leftFarSensor = 0;
const int leftNearSensor = 1;
const int leftCenterSensor = 2;
const int rightCenterSensor = 3;
const int rightNearSensor = 4;
const int rightFarSensor = 5;
int pos = 0;
int leftCenterReading;
int leftNearReading;
int leftFarReading;
int rightCenterReading;
int rightNearReading;
int rightFarReading;

const int PWMA = 5;
const int STBY = 3;
const int AIN1  = 2;      //Right Motor
const int AIN2  = 8;
const int BIN1 =  4;      //Left Motor
const int BIN2 = 7;
const int PWMB = 6;
int AMotorOffset = 0;     //Offset for Right Motor, self adjusts

byte runSpeed = 155;  //variable called "runSpeed" from 0 to 255 to control PWM duty cycle -> motor speed when driving straight
byte turnSpeed = 160; //variable called "turnSpeed" from 0 to 255 to control the PWM duty cycle (motor speed) when turning
int lineWidth = 40;
byte turnArSpeed = 182; //variable called "turnArSpeed" from 0 to 255 to control the PWM duty cycle (motor speed) when turning around

void setup() {
  pinMode(leftCenterSensor, INPUT);
  pinMode(leftNearSensor, INPUT);
  pinMode(leftFarSensor, INPUT);
  pinMode(rightCenterSensor, INPUT);
  pinMode(rightNearSensor, INPUT);
  pinMode(rightFarSensor, INPUT);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(STBY, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

 // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  readSensors();
  delay(1000);
  enableMotors();

}

void loop() {
  
  //update sensor readings, by calling "readSensors" function
  readSensors();

  //If (left AND right outside sensor 'white') AND (either center sensors are 'dark'),
  //then robot is centered on-line and drive straight
  // NOTE: IR Sensor is <200 for "white", and a <200 for "dark"
  if (leftFarReading < 200 && rightFarReading < 200 &&
      (leftCenterReading > 200 || rightCenterReading > 200) ) {
    AMotorOffset = straight(runSpeed, AMotorOffset);      //calls straight function
  }
  // Otherwise, robot is not on-line or at a turn, so run "lefthandwall" function
  else leftHandWall(turnSpeed, AMotorOffset);
  myservo.write(pos);              // tell servo to go to a specific angle using variable 'pos'
}

//ReadSensors function
void readSensors() {
  leftCenterReading  = analogRead(leftCenterSensor);
  leftNearReading    = analogRead(leftNearSensor);
  leftFarReading     = analogRead(leftFarSensor);
  rightCenterReading = analogRead(rightCenterSensor);
  rightNearReading   = analogRead(rightNearSensor);
  rightFarReading    = analogRead(rightFarSensor);
}

//Straight function
int straight(int PWMvalue, int AMotorOffset) {
  pos = 10;
  myservo.write(pos);
    analogWrite(PWMA, PWMvalue+AMotorOffset);
    analogWrite(PWMB, PWMvalue);
  if ( rightNearReading > 200) {                  //if right near is going black, 
    if (AMotorOffset <5)                         //limit motorA offset to maximum +10
      AMotorOffset = AMotorOffset + 3;            //increase motorA offset by 2 to adjust for imbalance between motors 
      digitalWrite(BIN1, HIGH);                   //drive only left motor for 2ms, then no drive for 1ms 
      digitalWrite(BIN2, LOW);
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, LOW);
      delay(1);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, LOW);
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, LOW);
      delay(1);
  }
  else if (leftNearReading > 200) {               //if left near is going black, 
    if (AMotorOffset > -5)                       //limit motorA offset to minimum -10
      AMotorOffset = AMotorOffset - 3;            //decrease motorA offset by 2 to adjust for imbalance between motors  
      digitalWrite(BIN1, LOW);                    //drive only right motor for 2ms, then no drive for 1ms
      digitalWrite(BIN2, LOW);
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      delay(1);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, LOW);
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, LOW);
      delay(1);
   }
  else {//else (if neither center are white), drive straight for 2ms, then no drive for 1ms
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    delay(2);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    delay(1);
  } 
  return AMotorOffset;
}

//Enable Motors function
void enableMotors() {
  digitalWrite(STBY, HIGH);
  delay(100);
}

//Standby motors function
void standbyMotors() {
  digitalWrite(STBY, LOW);
  delay(100);
}

void brake(){
    pos = 35;
    myservo.write(pos);
    digitalWrite(PWMB, HIGH);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    digitalWrite(PWMA, HIGH);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    delay(50);
}

//Blinks LED as an indicator
void blinkCode(int num){
  for(int x = 0; x < 3; x++){
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(250); 
  for (int i = 0; i < num; i++){
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(250); 
    }
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(250);
  }
}

//Left hand wall function (maze solving)
void leftHandWall(int PWMvalue, int AMotorOffset) {
  analogWrite(PWMA, PWMvalue+AMotorOffset);
  analogWrite(PWMB, PWMvalue);

  //DONE and T-intersections
  //if left and right outside sensors are 'dark'
  if ( leftFarReading > 200 && rightFarReading > 200) {
    //drive straight left and right motors in the same direction
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    delay(lineWidth);                          // delay to drive past one 'lineWidth' (ms)
    brake();
    readSensors();                                       // get updated sensor readings
    if (leftFarReading > 200 && rightFarReading > 200) {  //if still black, then done
      done();
    }
    else if (leftFarReading < 200 && rightFarReading < 200) {
      //if left AND right outside sensor are 'white', then "T" and call 'turnleft' function
      turnLeft(PWMvalue);
    }
  }

  //Left Turn Detection
  else if (leftFarReading > 200) {  // if you can turn left then turn left
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    delay(lineWidth);                          // delay to drive past one 'lineWidth' (ms)
    brake();
    readSensors(); 
    if (leftFarReading > 200 && rightFarReading > 200) {
      done();
    }
    else if (leftFarReading < 200 && rightFarReading < 200) {
      //if both left and right far sensors are white, then turn left
      turnLeft(PWMvalue);
    }
  }

 //Right Far Detection
  // if previous conditions not met, check if right far is dark
  else if (rightFarReading > 200) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    delay(10);
    readSensors();                                        //quickly get an updated reading
    if (leftFarReading > 200 && rightFarReading > 200) {  //if both far sensors are black, either done or T-intersection
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, LOW);
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      delay(lineWidth);                              //drive rest of linewidth
      brake();
      readSensors();                                       //get updated sensor readings
      if (rightFarReading > 200 && leftFarReading > 200) { //if still black then done
        done();
      }
      else if (leftFarReading < 200 && rightFarReading < 200) { //if both left and right far sensors are white, then turn left
        turnLeft(PWMvalue);
        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        delay(10);
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, LOW);
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, LOW);
        delay(10);                                     //makes left turn
        return;
      }
    }
    else {    //not done or T-intersection
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, LOW);
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      delay(lineWidth);  //drive rest of linewidth
      brake();                //stop motors
      readSensors();          //get sensor update
      //if left far & center and right far & center are all white, then right turn
      if (leftFarReading < 200 && leftCenterReading < 200 && rightCenterReading < 200 && rightFarReading < 200) {
        turnRight(PWMvalue);
        return;
      }
    }
  }
  //end of rightFar Detection

  //Turn Around Detection
  //if all sensors white, then run turn around
  else if ((leftFarReading < 200 && leftCenterReading < 200) && (rightCenterReading < 200
           && rightFarReading < 200)) {
    brake();
    readSensors();
    if ((leftFarReading < 200 && leftCenterReading < 200) && (rightCenterReading < 200
        && rightFarReading < 200)) {
      turnAround(PWMvalue,AMotorOffset);
    }
  }
  //End of Turn Around Detection
  
  enableMotors();
} //end of leftHandWall()

void done() {
  brake();
  pos = 160;
  myservo.write(pos);
  readSensors();       // get updated sensor readings
  if (leftFarReading > 200 && rightFarReading > 200) {
    while (analogRead(leftFarSensor) > 200 || analogRead(rightFarSensor) > 200) { //flash LED low then high to indicate "done" 
      blinkCode(3);
    }
  }
}

void turnLeft(int PWMvalue) {
  //blinkCode(4);
  analogWrite(PWMA, PWMvalue);
  analogWrite(PWMB, PWMvalue);
  int i = 0;
  while ((analogRead(leftFarSensor) < 200)&& i < 100){
    //while left far sensor is white,
    //drive left motor backwards and right motor forwards
    //for 10ms, then coast for 20ms
    //will exit when left far sensor is close to black
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    delay(10);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    delay(10);
    i++;
  }
  
  i=0;
  while ((analogRead(leftCenterSensor) < 100 && analogRead(rightCenterSensor) < 100)&& i<100) {
    //while center sensors are white,
    //drive left motor backwards and right motor forwards
    //for 10ms, then coast for 20ms
    //will exit when both center sensors are on black
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    delay(10);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    delay(10);
    i++;
  }
  delay(100);
} //end of leftTurn

void turnAround(int PWMvalue, int AMotorOffset) {
  //blinkCode(5);
  analogWrite(PWMA, PWMvalue+AMotorOffset);
  analogWrite(PWMB, PWMvalue);
  int i = 0;
  while ((analogRead(leftFarSensor) < 300)&& i < 100){
    //while left far sensor is white,
    //drive left motor backwards and right motor forwards
    //for 10ms, then coast for 20ms
    //will exit when left far sensor is close to black
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    delay(10);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    delay(10);
    i++;
  }
  
  i=0;
  brake();
  while ((analogRead(leftCenterSensor) < 100 && analogRead(rightCenterSensor) < 100)&& i<100) {
    //while center sensors are white,
    //drive left motor backwards and right motor forwards
    //for 10ms, then coast for 20ms
    //will exit when both center sensors are on black
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    delay(10);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    delay(30);
    i++;
  }
  delay(100);
} //end of turnAround

void turnRight(int PWMvalue) {
  //blinkCode(6);
  analogWrite(PWMA, PWMvalue);
  analogWrite(PWMB, PWMvalue);
  int i = 0;
  while ((analogRead(rightFarSensor) < 200)&& i < 100){
    //while right far sensor is white,
    //drive right motor backwards and left motor forwards
    //for 10ms, then coast for 20ms
    //will exit when left far sensor is close to black
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    delay(10);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    delay(10);
    i++;
  }
  
  i=0;
  while ((analogRead(leftCenterSensor) < 100 && analogRead(rightCenterSensor) < 100)&& i<100) {
    //while center sensors are white,
    //drive left motor backwards and right motor forwards
    //for 10ms, then coast for 20ms
    //will exit when both center sensors are on black
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    delay(10);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    delay(10);
    i++;
  }
  delay(100);
} //end of rightTurn
