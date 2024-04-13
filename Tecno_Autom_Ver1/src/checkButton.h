#include <Arduino.h>
#pragma once
//=================== VARIABLES DE LA FUNCION CHECKBUTTON ==========//
// Button timing variables
const int debounce = 20; // ms debounce period to prevent flickering when pressing or releasing the button
static int DCgap = 250; // max ms between clicks for a double click event
static int holdTime = 2000; // ms hold period: how long to wait for press+hold event
static int longHoldTime = 5000; // ms long hold period: how long to wait for press+hold event

// Other button variables
bool buttonVal = true; // value read from button
bool buttonLast = true; // buffered value of the button's previous state
bool DCwaiting = false; // whether we're waiting for a double click (down)
bool DConUp = false; // whether to register a double click on next release, or whether to wait and click
bool singleOK = true; // whether it's OK to do a single click
long downTime = -1; // time the button was pressed down
long upTime = -1; // time the button was released
bool ignoreUp = false; // whether to ignore the button release because the click+hold was triggered
bool waitForUp = false; // when held, whether to wait for the up event
bool holdEventPast = false; // whether or not the hold event happened already
bool longHoldEventPast = false;// whether or not the long hold event happened already
//=================== FIN VARIABLES DE LA FUNCION CHECKBUTTON ==========//

const int CLICK=1;
const int DOUBLE_CLICK=2;
const int HOLD=3;
const int LONG_HOLD=4;


/*
La función checkButton devuelve:
0: sin click
1: click
2: Doble click
3: click largo
4: click muy largo

*/
// AGREGAR ESTAS CONSTANTES EN EL ARCHIVO PRINCIPAL
/*
const int CLICK=1;
const int DOUBLE_CLICK=2;
const int HOLD=3;
const int LONG_HOLD=4;
*/

int checkButton(int PinSelect)
{ 
	
	
	int event = 0;
	// Read the state of the button
	buttonVal = digitalRead(PinSelect);
	// Button pressed down
	if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce) {
		downTime = millis();
		ignoreUp = false;
		waitForUp = false;
		singleOK = true;
		holdEventPast = false;
		longHoldEventPast = false;
		if ((millis()-upTime) < DCgap && DConUp == false && DCwaiting == true) DConUp = true;
		else DConUp = false;
		DCwaiting = false;
	}
	// Button released
	else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce) { 
		if (not ignoreUp) {
			upTime = millis();
			if (DConUp == false) DCwaiting = true;
			else {
				//event = 2;
				event = DOUBLE_CLICK;
				DConUp = false;
				DCwaiting = false;
				singleOK = false;
			}
		}
	}
	// Test for normal click event: DCgap expired
	if ( buttonVal == HIGH && (millis()-upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true) {
		//event = 1;
		event = CLICK;
		DCwaiting = false;
	}
	// Test for hold
	if (buttonVal == LOW && (millis() - downTime) >= holdTime) {
		// Trigger "normal" hold
		if (not holdEventPast) {
			//event = 3;
			event = HOLD;
			waitForUp = true;
			ignoreUp = true;
			DConUp = false;
			DCwaiting = false;
			//downTime = millis();
			holdEventPast = true;
		}
		// Trigger "long" hold
		if ((millis() - downTime) >= longHoldTime) {
			if (not longHoldEventPast) {
				//event = 4;
				event = LONG_HOLD;
				longHoldEventPast = true;
			}
		}
	}
	buttonLast = buttonVal;
	return event;
}
