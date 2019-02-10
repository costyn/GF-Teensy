// ==== Glow Fur (the original) ====
#define NEWFAN

// ---- LED stuff ----
#define APA_102
#define MY_DATA_PIN 11  /// DATA_PIN and CLOCK_PIN are reserved.
#define MY_CLOCK_PIN 13
#define NUM_LEDS 72
#define DEFAULT_BRIGHTNESS 40
#define MAX_BRIGHTNESS 70

// ---- Buttons ----
#define BUTTON_PIN 9
#define BUTTON_LED_PIN 3
#define BPM_BUTTON_PIN 19

// ---- Misc ----
#define DEFAULT_BPM 120
//#define USING_MPU

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
#define RT_FAN_WIPE
#define NUM_BLADES 14
// --- MPU Patterns ----
#ifdef USING_MPU
#define RT_SHAKE_IT
#define RT_STROBE1
#define RT_GLED
#endif
