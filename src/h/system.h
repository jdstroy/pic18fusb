/* system.h

 Copyright 2006 Andrew Holbrook

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

#ifndef SYS_H
#define SYS_H

/* External variables required byte header */
extern byte usb_state; // required by check_usb_bus()

void init(void){
	usb_state = DETACHED;
	ADCON1 |= 0x0F; //Set A/D pins(multiplexed with PORTA) to digital
	TRISD = 0; //Set PORTD pins to output
	PORTD = 0; //PORTD output 0
	UCFG = 0;  //Clear USB Configuration Register
	UCFGbits.UPUEN = 1; //Enable on-chip pull-up resistor
	UCFGbits.FSEN = 1; //Enable Full-Speed operation
}

void check_usb_bus(void){
	if(usb_bus_sense){
		UIE = 0; //clear usb interrupts
		UCONbits.USBEN = 1; //Turn on usb module
		usb_state = ATTACHED; //set usb device to attached
	}else if(!usb_bus_sense){
		UCON = 0; //clear usb control register
		UIE = 0; //disable all usb interrupts
		usb_state = DETACHED; //set usb device to detached
	}
	
	if(usb_state == ATTACHED){ //check if usb device is attached
		
		//Wait until D+ and D- to come out off single-ended zero condition
		while(UCONbits.SE0){
		}
		
		UIR = 0; //mask all usb interrupt flags
		UIE = 0; //disable all usb interrupts
		UIEbits.URSTIE = 1; //Turn on USB Reset interrupt
		UIEbits.IDLEIE = 1; //Turn on USB IDLE Interrupt
		usb_state = POWERED; //Set USB device to powered
		
		RCONbits.IPEN = 1; //Enable interrupt priority setting
		PIR2 = 0; //Mask all interrupt flags
		IPR2bits.USBIP = 1; //Set USB interrupts to high priority
		PIE2bits.USBIE = 1; //Enable USB interrupts
		INTCONbits.GIEH = 1; //Enable all high priority interrupts
	}
}

#endif // SYS_H
