#include <AFMotor.h>

AF_DCMotor motor(1);
AF_DCMotor steering(4);

void setup() {
	motor.setSpeed(255);
	steering.setSpeed(255);
}

int n = 500;

void loop() {
	steering.run(FORWARD);

	motor.run(FORWARD);
	delay(n);
	motor.run(BACKWARD);
	delay(n);

	steering.run(BACKWARD);

	motor.run(FORWARD);
	delay(n);
	motor.run(BACKWARD);
	delay(n);
}
