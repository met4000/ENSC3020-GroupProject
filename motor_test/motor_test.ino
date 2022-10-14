#include <AFMotor.h>

AF_DCMotor motor(1);

void setup() {
	motor.setSpeed(255);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
	delay(500);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(500);
	digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
	motor.run(BACKWARD);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(3000);

	motor.run(RELEASE);
	digitalWrite(LED_BUILTIN, LOW);
	delay(3000);
}
