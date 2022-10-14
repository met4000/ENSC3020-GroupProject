#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS gps;
SoftwareSerial ss(3, 2);

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);

void setup() {
    Serial.begin(57600);

    Serial.print("Testing TinyGPS library v. ");
    Serial.println(TinyGPS::library_version());
    Serial.println("by Mikal Hart");
    Serial.println();
    Serial.println("Sats HDOP Latitude      Longitude      Fix  ");
    Serial.println("          (deg)         (deg)          Age  ");
    Serial.println("--------------------------------------------");

    ss.begin(9600);
}

void loop() {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);

    print_int(gps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
    print_int(gps.hdop(), TinyGPS::GPS_INVALID_HDOP, 5);

    print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 14, 6);
    print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 15, 6);
    print_int(age, TinyGPS::GPS_INVALID_AGE, 5);

    Serial.println();

    smartdelay(1000);
}

static void smartdelay(unsigned long ms) {
    unsigned long start = millis();
    do {
        while (ss.available())
            gps.encode(ss.read());
    } while (millis() - start < ms);
}

static void print_float(float val, float invalid, int len, int prec) {
    if (val == invalid) {
        while (len-- > 1)
            Serial.print('*');
        Serial.print(' ');
    } else {
        Serial.print(val, prec);
        int vi = abs((int)val);
        int flen = prec + (val < 0.0 ? 2 : 1);  // . and -
        flen += vi >= 1000 ? 4 : vi >= 100 ? 3
                             : vi >= 10    ? 2
                                           : 1;
        for (int i = flen; i < len; ++i)
            Serial.print(' ');
    }
    smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len) {
    char sz[32];
    if (val == invalid)
        strcpy(sz, "*******");
    else
        sprintf(sz, "%ld", val);
    sz[len] = 0;
    for (int i = strlen(sz); i < len; ++i)
        sz[i] = ' ';
    if (len > 0)
        sz[len - 1] = ' ';
    Serial.print(sz);
    smartdelay(0);
}
