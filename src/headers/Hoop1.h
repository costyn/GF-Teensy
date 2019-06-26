// Hoop
#define HOOP

// ---- LED stuff ----
#define APA_102_SLOW
#define NUM_LEDS 87
#define MY_DATA_PIN PIN_SPI_MOSI
#define MY_CLOCK_PIN PIN_SPI_SCK
// #define MY_DATA_PIN 7
// #define MY_CLOCK_PIN 8
#define DEFAULT_BRIGHTNESS 50 // low - try to make the batteries last longer
#define MAX_BRIGHTNESS 80
#define DEFAULT_LED_MODE 1  // what pattern do we start with?

// ---- Buttons ----
#define BUTTON_PIN 1
// #define BUTTON_LED_PIN 13
// #define BPM_BUTTON_PIN 7
// #define BUTTON_GND_PIN 16  // is the other end of the button connected to another PIN?

// ---- Misc ----
#define DEFAULT_BPM 130
//#define AUTOADVANCE

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
#define RT_DISCO_GLITTER
#define RT_FIRE2012
#define RT_FASTLOOP
#define RT_FASTLOOP2
#define RT_FASTLOOP3
// #define RT_FASTLOOP4
#define RT_PENDULUM
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
#define RT_QUAD_STROBE
#define RT_REDDOTS1
#define RT_REDDOTS2

// #define RT_REDDOTS3
//#define RT_CIRC_LOADER1
//#define RT_CIRC_LOADER3
//#define RT_DROPLETS2
//#define RT_POVPATTERNS
