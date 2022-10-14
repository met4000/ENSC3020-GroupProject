#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <AFMotor.h>

AF_DCMotor motor(1);

TinyGPS gps;
SoftwareSerial ss(3, 2);

float target_lat = -31.979427, target_lon = 115.817939;
float threshold = 0.0001;

static void smartdelay(unsigned long ms);

void setup() {
    Serial.begin(57600);

    motor.setSpeed(255);
    motor.run(BACKWARD);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    ss.begin(9600);
}

void loop() {
    float lat, lon;
    unsigned long age;
    gps.f_get_position(&lat, &lon, &age);

    Serial.print("lat: ");
    Serial.print(lat, 6);
    Serial.print("\tlon: ");
    Serial.print(lon, 6);

    Serial.print("\tdist:");
    float distance = TinyGPS::distance_between(lat, lon, target_lat, target_lon);
    Serial.print(distance);
    Serial.print("\tcourse:");
    Serial.print(TinyGPS::course_to(lat, lon, target_lat, target_lon));

    Serial.println("");

    if (distance < 10) {
        motor.run(RELEASE);
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        motor.run(BACKWARD);
        digitalWrite(LED_BUILTIN, LOW);
    }

    smartdelay(1000);
}

static void smartdelay(unsigned long ms) {
    unsigned long start = millis();
    do {
        while (ss.available())
            gps.encode(ss.read());
    } while (millis() - start < ms);
}
