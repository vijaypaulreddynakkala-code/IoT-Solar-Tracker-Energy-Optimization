#include <Arduino.h>
#include <Servo.h>

// Pin Definitions
#define LDR_EAST_PIN        A0
#define LDR_WEST_PIN        A1
#define BATTERY_SENSE_PIN   A2
#define TRACKER_SERVO_PIN   9
#define NIGHT_LOAD_RELAY    7
#define LOW_BATT_LED        13

// Tracking & Power Thresholds
#define LIGHT_DEADBAND      35      // Prevents jitter around equal irradiance
#define NIGHT_THRESHOLD     180     // Ambient ADC value below which it is night
#define SERVO_MIN_ANGLE     15      // East limit (degrees)
#define SERVO_MAX_ANGLE     165     // West limit (degrees)
#define BATT_LOW_CUTOFF     11.2f   // Minimum safe 12V battery voltage (V)

Servo solarTracker;
int currentAngle = 90;              // Start centered
uint32_t lastTelemetryMs = 0;

// Read scaled 12V battery voltage (assuming 10k / 3.3k voltage divider)
float readBatteryVoltage() {
    int raw = analogRead(BATTERY_SENSE_PIN);
    float vOut = (raw / 1023.0f) * 5.0f;
    return vOut * ((10.0f + 3.3f) / 3.3f);
}

void setup() {
    Serial.begin(115200);

    pinMode(NIGHT_LOAD_RELAY, OUTPUT);
    pinMode(LOW_BATT_LED, OUTPUT);

    // Default night load OFF (Active-Low relay)
    digitalWrite(NIGHT_LOAD_RELAY, HIGH);
    digitalWrite(LOW_BATT_LED, LOW);

    solarTracker.attach(TRACKER_SERVO_PIN);
    solarTracker.write(currentAngle);
    delay(500);
}

void loop() {
    int eastLight = analogRead(LDR_EAST_PIN);
    int westLight = analogRead(LDR_WEST_PIN);
    int avgLight  = (eastLight + westLight) / 2;
    float battVoltage = readBatteryVoltage();

    // Low-battery protection indicator
    bool batteryHealthy = (battVoltage > BATT_LOW_CUTOFF);
    digitalWrite(LOW_BATT_LED, batteryHealthy ? LOW : HIGH);

    if (avgLight < NIGHT_THRESHOLD) {
        // NIGHT MODE: Reset panel to East for sunrise & power night load from stored battery
        if (currentAngle > SERVO_MIN_ANGLE) {
            currentAngle--;
            solarTracker.write(currentAngle);
        }
        // Enable night load only if battery has safe charge
        digitalWrite(NIGHT_LOAD_RELAY, batteryHealthy ? LOW : HIGH);
    } else {
        // DAY MODE: Turn off night load and track sun from East to West
        digitalWrite(NIGHT_LOAD_RELAY, HIGH);

        int diff = eastLight - westLight;
        if (abs(diff) > LIGHT_DEADBAND) {
            if (diff > 0 && currentAngle > SERVO_MIN_ANGLE) {
                currentAngle--; // Rotate toward East
            } else if (diff < 0 && currentAngle < SERVO_MAX_ANGLE) {
                currentAngle++; // Rotate toward West
            }
            solarTracker.write(currentAngle);
        }
    }

    // Periodic UART telemetry for system monitoring
    if (millis() - lastTelemetryMs >= 1000) {
        lastTelemetryMs = millis();
        Serial.print("East: "); Serial.print(eastLight);
        Serial.print(" | West: "); Serial.print(westLight);
        Serial.print(" | Angle: "); Serial.print(currentAngle);
        Serial.print(" deg | Batt: "); Serial.print(battVoltage, 2);
        Serial.print(" V | Mode: ");
        Serial.println((avgLight < NIGHT_THRESHOLD) ? "NIGHT (Load ON)" : "DAY (Tracking)");
    }

    delay(50);
}
