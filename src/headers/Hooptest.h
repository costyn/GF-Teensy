// Hoop
#define HOOP

// ---- LED stuff ----
#define APA_102
#define NUM_LEDS 60
#define MY_DATA_PIN 11
#define MY_CLOCK_PIN 13

#define DEFAULT_BRIGHTNESS 150
#define MAX_BRIGHTNESS 150
#define DEFAULT_LED_MODE 1

// ---- Buttons ----
#define BUTTON_PIN 18
//#define BUTTON_LED_PIN 3
#define BPM_BUTTON_PIN 7
#define BUTTON_GND_PIN 16  // is the button connected to another PIN?

// ---- Misc ----
#define DEFAULT_BPM 30
//#define AUTOADVANCE

// ---- Patterns ----
//#define RT_DROPLETS2
//#define RT_RIPPLE
//#define RT_FADE_GLITTER
//#define RT_POVPATTERNS
#define RT_CIRC_LOADER3
//#define RT_FASTLOOP3



// ---- POV Patterns ----

#ifdef RT_POVPATTERNS
#include "abstractPattern1.h"
#endif
