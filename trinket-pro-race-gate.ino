//#include <LowPower.h> // https://github.com/rocketscream/Low-Power
#include <FastLED.h>  // https://github.com/FastLED/FastLED
#include <Button.h>   // https://github.com/madleech/Button

#include "Palettes.h"

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    4
//#define CLK_PIN     2
//#define LED_TYPE    APA102
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    120
CRGB leds[NUM_LEDS];

#define FRAMES_PER_SECOND  60

const uint8_t PIN_BUTTON_PATTERN = 3;    // Pro Trinket: 12, Nano V3: 3
const uint8_t PIN_BUTTON_BRIGHTNESS = 2; // Pro Trinket: 11, Nano V3: 2
const uint8_t PIN_LED_STATUS = 13;

uint8_t brightnesses[] = { 255, 128, 64, 0, };
uint8_t currentBrightnessIndex = 0;

uint8_t currentPatternIndex = 0; // Index number of which pattern is current
uint8_t hue = 0; // rotating "base color" used by many of the patterns
uint8_t hueFast = 0; // faster rotating "base color" used by many of the patterns

uint8_t autoplay = 0;
uint8_t autoplayDuration = 10;
unsigned long autoPlayTimeout = 0;

uint8_t cyclePalettes = 1;
uint8_t paletteDuration = 5;
uint8_t currentPaletteIndex = 0;
unsigned long paletteTimeout = 0;

bool ledStatus = false;

Button buttonBrightness(PIN_BUTTON_BRIGHTNESS);
Button buttonPattern(PIN_BUTTON_PATTERN);

// maps for 16 horizontal LEDs, 12 vertical
//const uint8_t coordsX[NUM_LEDS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
//const uint8_t coordsY[NUM_LEDS] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//const uint8_t coordsX256[NUM_LEDS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 27, 40, 54, 67, 81, 94, 107, 121, 134, 148, 161, 174, 188, 201, 215, 228, 242, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 242, 228, 215, 201, 188, 174, 161, 148, 134, 121, 107, 94, 81, 67, 54, 40, 27, 13 };
//const uint8_t coordsY256[NUM_LEDS] = { 20, 39, 59, 78, 98, 118, 137, 157, 177, 196, 216, 235, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 235, 216, 196, 177, 157, 137, 118, 98, 78, 59, 39, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//// large gate, LEDS run up, across, down, across
//const uint8_t coordsHeat[NUM_LEDS] = { 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8 };

// small and medium gates, LEDs run across, up, across, down
//const uint8_t coordsHeat[NUM_LEDS] = { 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9 };

//#define MATRIX_WIDTH 20
//#define MATRIX_HEIGHT 14

// maps for 30 x 30
const uint8_t coordsX[NUM_LEDS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31 };
const uint8_t coordsY[NUM_LEDS] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

const uint8_t coordsX256[NUM_LEDS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 16, 25, 33, 41, 49, 58, 66, 74, 82, 90, 99, 107, 115, 123, 132, 140, 148, 156, 165, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };
const uint8_t coordsY256[NUM_LEDS] = { 8, 16, 25, 33, 41, 49, 58, 66, 74, 82, 90, 99, 107, 115, 123, 132, 140, 148, 156, 165, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 247, 239, 230, 222, 214, 206, 197, 189, 181, 173, 165, 156, 148, 140, 132, 123, 115, 107, 99, 90, 82, 74, 66, 58, 49, 41, 33, 25, 16, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// large gate, LEDS run up, across, down, across
const uint8_t coordsHeat[NUM_LEDS] = { 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };

#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 32

#define MAX_DIMENSION ((MATRIX_WIDTH > MATRIX_HEIGHT) ? MATRIX_WIDTH : MATRIX_HEIGHT)

#include "Noise.h"

void wakeUp()
{
  Serial.println("Waking up...");
}

void setup() {
  Serial.begin(115200);
  Serial.println("setup");

  // delay(3000); // 3 second delay for recovery

  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //  FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER,DATA_RATE_MHZ(12)>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(brightnesses[currentBrightnessIndex]);

  FastLED.show(CRGB::Black);

  buttonBrightness.begin();
  buttonPattern.begin();

  pinMode(PIN_LED_STATUS, OUTPUT);
  digitalWrite(PIN_LED_STATUS, LOW);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList patterns = {

  fire,
  water,

  chaseRainbow,
  chaseRainbow2,

  pride,
  colorWaves,

  // noise patterns
  //  fireNoise,
  //  fireNoise2,

  //  paletteNoise,
  //  lavaNoise,
  //  rainbowNoise,
  //  rainbowStripeNoise,
  //  partyNoise,
  //  forestNoise,
  //  cloudNoise,
  //  oceanNoise,
  //  blackAndWhiteNoise,
  //  blackAndBlueNoise,

  // palette shifting/blending patterns
  //  verticalRainbow,
  //  horizontalRainbow,
  //  diagonalRainbow,
  //  verticalPaletteBlend,
  //  horizontalPaletteBlend,

  //  chaseRainbow3,
  //  chasePalette,
  //  chasePalette2,
  //  chasePalette3,
  //  solidPalette,
  //  solidRainbow,
  //  rainbow,
  //  rainbowWithGlitter,
  //  confetti,
  //  sinelon,
  //  juggle,
  //  bpm,

};

void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  patterns[currentPatternIndex]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS( 40 ) {
    hue++;  // slowly cycle the "base color" through the rainbow

    // slowly blend the current palette to the next
    nblendPaletteTowardPalette(currentPalette, targetPalette, 8);
  }

  EVERY_N_MILLISECONDS( 4 ) {
    hueFast++;  // slowly cycle the "base color" through the rainbow
  }

  if (autoplay == 1 && (millis() > autoPlayTimeout)) {
    nextPattern();
    autoPlayTimeout = millis() + (autoplayDuration * 1000);
  }

  if (cyclePalettes == 1 && (millis() > paletteTimeout)) {
    nextPalette();
    paletteTimeout = millis() + (paletteDuration * 1000);
  }

  handleInput();

  //  if (brightnesses[currentBrightnessIndex] == 0) {
  //    Serial.println("Going down...");
  //    Serial.flush();
  //
  //    fill_solid(leds, NUM_LEDS, CRGB::Black);
  //    FastLED.show();
  //
  //    // Allow wake up pin to trigger interrupt on low.
  //    attachInterrupt(0, wakeUp, LOW);
  //    attachInterrupt(1, wakeUp, LOW);
  //
  //    // Enter power down state with ADC and BOD module disabled.
  //    // Wake up when wake up pin is low.
  //    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  //
  //    // Disable external pin interrupt on wake up pin.
  //    detachInterrupt(0);
  //    detachInterrupt(1);
  //
  //    Serial.println("I have awoken!");
  //
  //    nextBrightness();
  //  }
}

void handleInput() {
  if (buttonPattern.pressed()) {
    digitalWrite(PIN_LED_STATUS, HIGH);
  }
  else if (buttonPattern.released())
  {
    ledStatus = !ledStatus;
    digitalWrite(PIN_LED_STATUS, LOW);
    nextPattern();
  }

  if (buttonBrightness.pressed()) {
    digitalWrite(PIN_LED_STATUS, HIGH);
  }
  else if (buttonBrightness.released()) {
    ledStatus = !ledStatus;
    digitalWrite(PIN_LED_STATUS, LOW);
    nextBrightness();
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// returns an 8-bit value that
// rises and falls in a sawtooth wave, 'BPM' times per minute,
// between the values of 'low' and 'high'.
//
//       /|      /|
//     /  |    /  |
//   /    |  /    |
// /      |/      |
//
uint8_t beatsaw8( accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255,
                  uint32_t timebase = 0, uint8_t phase_offset = 0)
{
  uint8_t beat = beat8( beats_per_minute, timebase);
  uint8_t beatsaw = beat + phase_offset;
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8( beatsaw, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

void nextBrightness()
{
  // add one to the current brightness number, and wrap around at the end
  currentBrightnessIndex = (currentBrightnessIndex + 1) % ARRAY_SIZE(brightnesses);
  FastLED.setBrightness(brightnesses[currentBrightnessIndex]);
  Serial.print("brightness: ");
  Serial.println(brightnesses[currentBrightnessIndex]);
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  currentPatternIndex = (currentPatternIndex + 1) % ARRAY_SIZE(patterns);
  Serial.print("pattern: ");
  Serial.println(currentPatternIndex);
}

void nextPalette()
{
  // add one to the current palette number, and wrap around at the end
  currentPaletteIndex = (currentPaletteIndex + 1) % paletteCount;
  targetPalette = palettes[currentPaletteIndex];
  Serial.print("palette: ");
  Serial.println(currentPaletteIndex);
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, hue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( hue + random8(64), 200, 255);
}

// Updated sinelon (no visual gaps) by Mark Kriegsman
// https://gist.github.com/kriegsman/261beecba85519f4f934
void sinelon()
{
  // a colored dot sweeping
  // back and forth, with
  // fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  CHSV color = CHSV(hue, 220, 255);
  int pos = beatsin16(120, 0, NUM_LEDS - 1);
  static int prevpos = 0;
  if ( pos < prevpos ) {
    fill_solid(leds + pos, (prevpos - pos) + 1, color);
  } else {
    fill_solid(leds + prevpos, (pos - prevpos) + 1, color);
  }
  prevpos = pos;
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = RainbowColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, hue + (i * 2), beat - hue + (i * 10));
  }
}

void juggle() {
  const uint8_t dotCount = 3;
  uint8_t step = 192 / dotCount;
  // colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < dotCount; i++) {
    CRGB color = CHSV(dothue, 255, 255);
    if (dotCount == 3) {
      if (i == 1) {
        color = CRGB::Green;
      }
      else if (i == 2) {
        color = CRGB::Blue;
      }
    }
    leds[beatsin16(i + 15, 0, NUM_LEDS)] += color;
    dothue += step;
  }
}

void solidRainbow()
{
  fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));
}

void solidPalette()
{
  fill_solid(leds, NUM_LEDS, ColorFromPalette(currentPalette, hue));
}

void chaseRainbow() {
  static uint8_t h = 0;

  for (uint16_t i = NUM_LEDS - 1; i > 0; i--) {
    leds[i] = leds[i - 1];
  }
  leds[0] = ColorFromPalette(RainbowStripeColors_p, h++);
}

void chaseRainbow2() {
  const uint8_t step = 255 / NUM_LEDS;

  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(RainbowStripeColors_p, hueFast + i * step);
  }
}

void chaseRainbow3()
{
  fadeToBlackBy(leds, NUM_LEDS, 20);
  CHSV color = CHSV(hue, 220, 255);
  int pos = beatsaw8(120, 0, NUM_LEDS - 1);
  static int prevpos = 0;
  if ( pos < prevpos ) {
    fill_solid(leds + prevpos, NUM_LEDS - prevpos, color);
    fill_solid(0, pos + 1, color);
  } else {
    fill_solid(leds + prevpos, (pos - prevpos) + 1, color);
  }
  prevpos = pos;
}

void chasePalette() {
  for (uint16_t i = NUM_LEDS - 1; i > 0; i--) {
    leds[i] = leds[i - 1];
  }
  leds[0] = ColorFromPalette(currentPalette, hue);
}

void chasePalette2() {
  const uint8_t step = 255 / NUM_LEDS;

  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(currentPalette, hueFast + i * step);
  }
}

void chasePalette3()
{
  fadeToBlackBy(leds, NUM_LEDS, 20);
  CRGB color = ColorFromPalette(currentPalette, hue);
  int pos = beatsaw8(120, 0, NUM_LEDS - 1);
  static int prevpos = 0;
  if ( pos < prevpos ) {
    fill_solid(leds + prevpos, NUM_LEDS - prevpos, color);
    fill_solid(0, pos + 1, color);
  } else {
    fill_solid(leds + prevpos, (pos - prevpos) + 1, color);
  }
  prevpos = pos;
}

// Pride2015 by Mark Kriegsman: https://gist.github.com/kriegsman/964de772d64c502760e5
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride()
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//hue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV( hue8, sat8, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;

    nblend( leds[pixelnumber], newcolor, 64);
  }
}

void colorWaves()
{
  colorwaves(leds, NUM_LEDS, currentPalette);
}

// ColorWavesWithPalettes by Mark Kriegsman: https://gist.github.com/kriegsman/8281905786e8b2632aeb
// This function draws color waves with an ever-changing,
// widely-varying set of parameters, using a color palette.
void colorwaves( CRGB* ledarray, uint16_t numleds, CRGBPalette16& palette)
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  // uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//hue * 256;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( uint16_t i = 0 ; i < numleds; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if ( h16_128 & 0x100) {
      hue8 = 255 - (h16_128 >> 1);
    } else {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    uint8_t index = hue8;
    //index = triwave8( index);
    index = scale8( index, 240);

    CRGB newcolor = ColorFromPalette( palette, index, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (numleds - 1) - pixelnumber;

    nblend( ledarray[pixelnumber], newcolor, 128);
  }
}

CRGB scrollingHorizontalPalette( uint8_t x, uint8_t y, unsigned long timeInMillis)
{
  return ColorFromPalette(currentPalette, x + (timeInMillis / 10), 255, LINEARBLEND);
}

void horizontalPaletteBlend()
{
  unsigned long t = millis();

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = scrollingHorizontalPalette(coordsX256[i], coordsY256[i], t);
  }
}

CRGB scrollingVerticalPalette( uint8_t x, uint8_t y, unsigned long timeInMillis)
{
  return ColorFromPalette(currentPalette, y + (timeInMillis / 10), 255, LINEARBLEND);
}

void verticalPaletteBlend()
{
  unsigned long t = millis();

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = scrollingVerticalPalette(coordsX256[i], coordsY256[i], t);
  }
}

CRGB scrollingHorizontalWashColor( uint8_t x, uint8_t y, unsigned long timeInMillis)
{
  return CHSV( x + (timeInMillis / 10), 255, 255);
}

void horizontalRainbow()
{
  unsigned long t = millis();

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = scrollingHorizontalWashColor(coordsX256[i], coordsY256[i], t);
  }
}

CRGB scrollingVerticalWashColor( uint8_t x, uint8_t y, unsigned long timeInMillis)
{
  return CHSV( y + (timeInMillis / 10), 255, 255);
}

void verticalRainbow()
{
  unsigned long t = millis();

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = scrollingVerticalWashColor(coordsX256[i], coordsY256[i], t);
  }
}

CRGB scrollingDiagonalWashColor( uint8_t x, uint8_t y, unsigned long timeInMillis)
{
  return CHSV( x + y + (timeInMillis / 10), 255, 255);
}

void diagonalRainbow()
{
  unsigned long t = millis();

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = scrollingDiagonalWashColor(coordsX256[i], coordsY256[i], t);
  }
}

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
const uint8_t COOLING = 80;

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
const uint8_t SPARKING = 120;

// based on FastLED example Fire2012WithPalette: https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
void heatMap(CRGBPalette16 palette, bool up)
{
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy(random(256));

  const uint16_t kMatrixHeight = NUM_LEDS / 2;
  const uint16_t maxY = kMatrixHeight - 1;

  // Array of temperature readings at each simulation cell
  static byte heat[2][kMatrixHeight];

  for (int x = 0; x < 2; x++)
  {
    // Step 1.  Cool down every cell a little
    for ( int i = 0; i < kMatrixHeight; i++) {
      heat[x][i] = qsub8( heat[x][i],  random8(0, ((COOLING * 10) / kMatrixHeight) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for ( int k = kMatrixHeight - 1; k >= 2; k--) {
      heat[x][k] = (heat[x][k - 1] + heat[x][k - 2] + heat[x][k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if ( random8() < SPARKING ) {
      int y = random8(3);
      heat[x][y] = qadd8( heat[x][y], random8(160, 255) );
    }
  }

  // Step 4.  Map from heat cells to LED colors
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    uint8_t colorIndex = 0;

    uint8_t x = 0;

    if (i >= kMatrixHeight) {
      x = 1;
    }

    uint16_t y = coordsHeat[i];

    if (!up)
      colorIndex = heat[x][(maxY) - y];
    else
      colorIndex = heat[x][y];

    // Recommend that you use values 0-240 rather than
    // the usual 0-255, as the last 15 colors will be
    // 'wrapping around' from the hot end to the cold end,
    // which looks wrong.
    colorIndex = scale8(colorIndex, 190);

    leds[i] = ColorFromPalette(palette, colorIndex, 255, LINEARBLEND);
  }
}

void fire()
{
  heatMap(HeatColors_p, true);
}

CRGBPalette16 IceColors_p = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);

void water()
{
  heatMap(IceColors_p, false);
}
