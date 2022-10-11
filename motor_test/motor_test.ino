#include <AFMotor.h>

AF_DCMotor motor(1);

void setup() {
	motor.setSpeed(255);
	motor.run(FORWARD);

	pinMode(13, OUTPUT);
}

bool v = false;

void loop() {
	motor.run(v ? FORWARD : BACKWARD);
	// motor.run(FORWARD);

	digitalWrite(13, v = !v);
	delay(200);
}
