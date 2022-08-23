/*joystic car control project.
 * http://srituhobby.com
 */
 
#include <AFMotor.h>
#define x A0
#define y A1
#define Speed 180

AF_DCMotor motor1(3);
AF_DCMotor motor2(4);

void setup() {
  Serial.begin(9600);
  motor1.setSpeed(Speed);
  motor2.setSpeed(Speed);
}

void loop() {
  int X = analogRead(x);
  int Y = analogRead(y);

  Serial.print(X);
  Serial.print("\t");
  Serial.println(Y);

  if (X >= 800) {
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
  } else if (X <= 200) {
    motor1.run(FORWARD);
    motor2.run(FORWARD);
  } else if (Y >= 800) {
    motor1.run(FORWARD);
    motor2.run(BACKWARD);
  } else if (Y <= 200) {
    motor1.run(BACKWARD);
    motor2.run(FORWARD);
  } else {
    motor1.run(RELEASE);
    motor2.run(RELEASE);
  }
}
