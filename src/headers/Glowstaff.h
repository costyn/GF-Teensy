// Glowstaff
#define GLOWSTAFF

// ---- LED stuff ----
#define APA_102
#define MY_DATA_PIN 11
#define MY_CLOCK_PIN 13
#define NUM_LEDS 139
#define DEFAULT_BRIGHTNESS 10
#define MAX_BRIGHTNESS 100  // 278 LEDs use a LOT of power (measured max 5A)

// ---- Buttons ----
#define BUTTON_PIN 14
#define BUTTON_LED_PIN 3
#define BPM_BUTTON_PIN 7

// ---- Misc ----
#define DEFAULT_BPM 60
// #define USING_MPU
//#define AUTOADVANCE

// ---- MPU Calibration ----
#define X_ACCEL_OFFSET  -235
#define Y_ACCEL_OFFSET  2035
#define Z_ACCEL_OFFSET  1104
#define X_GYRO_OFFSET  100
#define Y_GYRO_OFFSET  0
#define Z_GYRO_OFFSET  36


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
#define RT_FIRE2012
#define RT_FADE_GLITTER
#define RT_DISCO_GLITTER
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
#define RT_BOUNCYBALLS
#define RT_DROPLETS
#define RT_RIPPLE
// --- MPU Patterns ----
#ifdef USING_MPU
#define RT_SHAKE_IT
#define RT_STROBE1
#define RT_GLED
#endif
