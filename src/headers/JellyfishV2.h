// Jellyfish Umbrella
#define JELLY_V2

// https://github.com/FastLED/FastLED/wiki/Multiple-Controller-Examples#one-array-many-strips
// https://github.com/FastLED/FastLED/wiki/ESP8266-notes
// ---- LED stuff ----
#define NEO_PIXEL_MULTI
#define LED_PIN_1    12
#define LED_PIN_2    27
#define LED_PIN_3    33
#define LED_PIN_4    15
#define LED_PIN_5    32
#define NUM_LEDS_PER_STRIP 41
#define NUM_STRIPS 5
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS
#define DEFAULT_BRIGHTNESS 100
#define MAX_BRIGHTNESS 150

// Extra's
#define LED_PIN_6    39
#define LED_PIN_7    36
#define LED_PIN_8    4

// ---- Buttons ----
#define BUTTON_PIN 21
#define BUTTON_LED_PIN 13
// #define BPM_BUTTON_PIN 7

// ---- Misc ----
#define DEFAULT_BPM 120
#define AUTOADVANCE
#define BATTERY_PIN A13
#define USING_MPU

// ---- MPU Calibration: TODO! ----
#define X_ACCEL_OFFSET  -1527
#define Y_ACCEL_OFFSET  -1127
#define Z_ACCEL_OFFSET  2339
#define X_GYRO_OFFSET   51
#define Y_GYRO_OFFSET   4
#define Z_GYRO_OFFSET   -7

// ---- Patterns ----
#define RT_P_RB_STRIPE
#define RT_P_OCEAN
#define RT_P_HEAT
#define RT_P_LAVA
#define RT_P_PARTY
#define RT_P_FOREST
#define RT_TWIRL1
#define RT_TWIRL2
#define RT_TWIRL4
#define RT_TWIRL6
#define RT_TWIRL2_O
#define RT_TWIRL4_O
#define RT_TWIRL6_O
#define RT_FADE_GLITTER
#define RT_DISCO_GLITTER
#define RT_HEARTBEAT
#define RT_FASTLOOP
#define RT_FASTLOOP2
#define RT_PENDULUM
#define RT_BOUNCEBLEND
#define RT_JUGGLE_PAL
#define RT_NOISE_LAVA
#define RT_NOISE_PARTY
#define RT_PULSE_5_1
#define RT_PULSE_5_2
#define RT_PULSE_5_3
#define RT_THREE_SIN_PAL
#define RT_COLOR_GLOW
