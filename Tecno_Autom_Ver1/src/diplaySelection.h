// Usar estas variable para elegir entre los dos displays disponibles.
// El display de 0.96" usa el driver SSD1306.
// El display de 1.3" usa el driver SH110X.

#define USE_BIG_DISPLAY true

const int I2C_ADDRESS = 0x3C;
// OLED
const int SCREEN_WIDTH = 128; // OLED display width, in pixels
#if USE_BIG_DISPLAY
    const int SCREEN_HEIGHT = 64;
#else
    const int SCREEN_HEIGHT = 32; // OLED display height, in pixels
#endif