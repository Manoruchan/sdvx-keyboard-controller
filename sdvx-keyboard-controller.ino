// Pro Micro sketch for SDVX keyboard controller
#include <Keyboard.h>

// define pin inputs
const uint8_t PIN_VOL_L_A = 3;   // L-A        D3, INT0
const uint8_t PIN_VOL_L_B = 2;   // L-B        D2, INT1
const uint8_t PIN_VOL_R_A = 0;   // R-A   RX1, D0, INT2
const uint8_t PIN_VOL_R_B = 1;   // R-B   TX0, D1, INT3
const uint8_t PIN_BT_A = 4;      // BT-A  4,   D4
const uint8_t PIN_BT_B = 5;      // BT-B  5,   D5
const uint8_t PIN_BT_C = 6;      // BT-C  6,   D6
const uint8_t PIN_BT_D = 7;      // BT-D  7,   D7
const uint8_t PIN_FX_L = 8;      // FX-L  8,   D8
const uint8_t PIN_FX_R = 9;      // FX-R  9,   D9
const uint8_t PIN_BT_START = 10; // START 10,  D10

// define key inputs
#define VOL_L_CW 'w'
#define VOL_L_CCW 'q'
#define VOL_R_CW 'p'
#define VOL_R_CCW 'o'
#define BT_A 'd'
#define BT_B 'f'
#define BT_C 'j'
#define BT_D 'k'
#define FX_L 'c'
#define FX_R 'm'
#define BT_START KEY_RETURN

// usability settings
const uint8_t KEY_DELAY = 2;           // determine the delay between keyinputs
const uint8_t VOL_RELEASE_DELAY = 10;  // determine sensitivity of the encoder

// encoder data
volatile uint8_t encCurrentL;
volatile uint8_t encPreviousL;
volatile uint8_t encPastL;

volatile uint8_t encCurrentR;
volatile uint8_t encPreviousR;
volatile uint8_t encPastR;

// counterclockwise [-128, 0)
// nothing          0
// clockwise        (0, 127]
volatile int8_t encStateL = 0;
volatile int8_t encStateR = 0;


void setup() {
	// pin settings

	// rorary encoders
	pinMode(PIN_VOL_L_A, INPUT_PULLUP);
	pinMode(PIN_VOL_L_B, INPUT_PULLUP);
	pinMode(PIN_VOL_R_A, INPUT_PULLUP);
	pinMode(PIN_VOL_R_B, INPUT_PULLUP);

	// buttons
	pinMode(PIN_BT_A, INPUT_PULLUP);
	pinMode(PIN_BT_B, INPUT_PULLUP);
	pinMode(PIN_BT_C, INPUT_PULLUP);
	pinMode(PIN_BT_D, INPUT_PULLUP);
	pinMode(PIN_FX_L, INPUT_PULLUP);
	pinMode(PIN_FX_R, INPUT_PULLUP);
	pinMode(PIN_BT_START, INPUT_PULLUP);

	// enc interrupts
	attachInterrupt(digitalPinToInterrupt(PIN_VOL_L_A), updateEncStateL, CHANGE);
	attachInterrupt(digitalPinToInterrupt(PIN_VOL_L_B), updateEncStateL, CHANGE);
	attachInterrupt(digitalPinToInterrupt(PIN_VOL_R_A), updateEncStateR, CHANGE);
	attachInterrupt(digitalPinToInterrupt(PIN_VOL_R_B), updateEncStateR, CHANGE);

	// use as keyboard
	Keyboard.begin();
}


void loop() {
	// button
	if (digitalRead(PIN_BT_A) == LOW) {
		Keyboard.press(BT_A);
		delay(KEY_DELAY);
	} else if (digitalRead(PIN_BT_A) == HIGH) {
		Keyboard.release(BT_A);
	}

	if (digitalRead(PIN_BT_B) == LOW) {
		Keyboard.press(BT_B);
		delay(KEY_DELAY);
	} else if (digitalRead(PIN_BT_B) == HIGH) {
		Keyboard.release(BT_B);
	}

	if (digitalRead(PIN_BT_C) == LOW) {
		Keyboard.press(BT_C);
		delay(KEY_DELAY);
	} else if (digitalRead(PIN_BT_C) == HIGH) {
		Keyboard.release(BT_C);
	}

	if (digitalRead(PIN_BT_D) == LOW) {
		Keyboard.press(BT_D);
		delay(KEY_DELAY);
	} else if (digitalRead(PIN_BT_D) == HIGH) {
		Keyboard.release(BT_D);
	}

	if (digitalRead(PIN_FX_L) == LOW) {
		Keyboard.press(FX_L);
		delay(KEY_DELAY);
	} else if (digitalRead(PIN_FX_L) == HIGH) {
		Keyboard.release(FX_L);
	}

	if (digitalRead(PIN_FX_R) == LOW) {
		Keyboard.press(FX_R);
		delay(KEY_DELAY);
	} else if (digitalRead(PIN_FX_R) == HIGH) {
		Keyboard.release(FX_R);
	}

	if (digitalRead(PIN_BT_START) == LOW) {
		Keyboard.press(KEY_RETURN);
	} else if (digitalRead(PIN_BT_START) == HIGH) {
		Keyboard.release(KEY_RETURN);
	}

	// convert enc input to keyboard input
	// clockwise
	if (encStateL > 0) {
		Keyboard.release(VOL_L_CCW);
		Keyboard.press(VOL_L_CW);
		delay(VOL_RELEASE_DELAY);
		Keyboard.release(VOL_L_CW);
		encStateL--;
	}
	// counterclockwise
	if (encStateL < 0) {
		Keyboard.release(VOL_L_CW);
		Keyboard.press(VOL_L_CCW);
		delay(VOL_RELEASE_DELAY);
		Keyboard.release(VOL_L_CCW);
		encStateL++;
	}

	// clockwise
	if (encStateR > 0) {
		Keyboard.release(VOL_R_CCW);
		Keyboard.press(VOL_R_CW);
		delay(VOL_RELEASE_DELAY);
		Keyboard.release(VOL_R_CW);
		encStateR--;
	}
	// counterclockwise
	if (encStateR < 0) {
		Keyboard.release(VOL_R_CW);
		Keyboard.press(VOL_R_CCW);
		delay(VOL_RELEASE_DELAY);
		Keyboard.release(VOL_R_CCW);
		encStateR++;
	}
}


void updateEncStateL() {
	uint8_t pinA = digitalRead(PIN_VOL_L_A);
	uint8_t pinB = digitalRead(PIN_VOL_L_B);

	encCurrentL = (pinA << 1) | pinB;

	if (!(encCurrentL == encPreviousL)) {
		uint8_t encState = encPastL << 4 | encPreviousL << 2 | encCurrentL;

		// if clockwise
		if (encState == 0b111000) {
			if (encStateL != 1) {
				encStateL++;
			}
		}
		// if conterclockwise
		else if (encState == 0b001011) {
			if (encStateL != -1) {
				encStateL--;
			}
		}

		encPastL = encPreviousL;
		encPreviousL = encCurrentL;
	}
}

void updateEncStateR() {
	uint8_t pinA = digitalRead(PIN_VOL_R_A);
	uint8_t pinB = digitalRead(PIN_VOL_R_B);

	encCurrentR = (pinA << 1) | pinB;

	if (!(encCurrentR == encPreviousR)) {
		uint8_t encState = encPastR << 4 | encPreviousR << 2 | encCurrentR;

		// if clockwise
		if (encState == 0b111000) {
			if (encStateR != 1) {
				encStateR++;
			}
		}
		// if conterclockwise
		else if (encState == 0b001011) {
			if (encStateR != -1) {
				encStateR--;
			}
		}

		encPastR = encPreviousR;
		encPreviousR = encCurrentR;
	}
}
