#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <AFMotor.h>

#define DEBUG true

AF_DCMotor drive(1); // reversed
AF_DCMotor steering(4);

TinyGPS gps;
SoftwareSerial ss(3, 2);


// parameters
float cone_lat = -31.979427, cone_lon = 115.817939;
float target_distance_threshold = 10;

float kp = 10;
float min_err = 15;
float gps_cycle_delay = 3500, max_turn_delay = 1000;


static void smart_delay(unsigned long ms);

float lat, lon, last_lat, last_lon;
unsigned long age;
float target_lat, target_lon, init_lat, init_lon;
int target_count = 0, target_count_max = 2;

float distance;
float heading, bearing;
int turn_delay;

float err, absErr;

void setup() {
#if DEBUG
    Serial.begin(9600);
    Serial.println("Startup...");
#endif
    delay(5000); // can remove if too slow

    drive.setSpeed(255);
    steering.setSpeed(255);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // start listening to GPS coms
    ss.begin(9600);

#if DEBUG
    Serial.println("waiting for gps...");
#endif

    // wait for GPS data
    smart_delay(5000);
    gps.f_get_position(&lat, &lon, &age);

    digitalWrite(LED_BUILTIN, HIGH);

    // set initial pos
    init_lat = lat;
    init_lon = lon;

    // set target pos
    target_lat = cone_lat;
    target_lon = cone_lon;

    drive.run(BACKWARD);
#if DEBUG
    Serial.println("waiting for gps (2)...");
#endif
    smart_delay(gps_cycle_delay);
    digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
    last_lat = lat;
    last_lon = lon;

    gps.f_get_position(&lat, &lon, &age);

    heading = TinyGPS::course_to(last_lat, last_lon, lat, lon);
    distance = TinyGPS::distance_between(lat, lon, target_lat, target_lon);
    bearing = TinyGPS::course_to(lat, lon, target_lat, target_lon);
#if DEBUG
    Serial.print("heading: ");
    Serial.print(heading);
    Serial.print("\tdistance: ");
    Serial.print(distance);
    Serial.print("\tbearing: ");
    Serial.print(bearing);
#endif

    if (distance < target_distance_threshold) {
        // approx. at target
        drive.run(RELEASE);

        // check if done
        target_count++;
        while (target_count >= target_count_max) delay(10000);

        // change target
        target_lat = init_lat;
        target_lon = init_lon;

        // TODO: perform some manoeuver - e.g. turn around?
        delay(5000);

        smart_delay(gps_cycle_delay);
        drive.run(BACKWARD);
    }

    err = bearing - heading;
    err = fmod((err + 180) + 360, 360) - 180;

#if DEBUG
    Serial.print("\terr: ");
    Serial.print(err);
#endif

    absErr = fabs(err);
    if (absErr > min_err) {
        turn_delay = fmin(absErr * kp, max_turn_delay);
#if DEBUG
        Serial.print("\tturn_delay: ");
        Serial.print(turn_delay);
#endif
        steering.run(err < 0 ? FORWARD : BACKWARD); // TODO: check direction
        delay(turn_delay);
        steering.run(RELEASE);
    }

#if DEBUG
    Serial.println("");
#endif

    smart_delay(max(gps_cycle_delay - turn_delay, 0));
}

static void smart_delay(unsigned long ms) {
    unsigned long start = millis();
    do {
        while (ss.available())
            gps.encode(ss.read());
    } while (millis() - start < ms);
}
