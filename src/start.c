/* start.c

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

/* Header files */
#include <p18f4550.h> //Header provided by Microchip.

/* Local prototypes */
void _entry(void);
void _high_isr(void);
void _low_isr(void);

/* External prototypes */
extern void main(void);
extern void high_isr_handler(void);
extern void low_isr_handler(void);

#pragma code _entry = 0x0 //Defines the following code section at address 0x0(reset interrupt vector).

//Entry point for program on startup/reset.
void _entry(void){
	while(1){ //Loop on main/user code.
		main(); 
	}
}

#pragma code _high_isr = 0x08 //Defines the following code section at address 0x08(high priority interrupt vector).

//Entry point on a high priority interrupt.
void _high_isr(void){
	INTCON = 0; //Disable interrupts.
	high_isr_handler(); //Call high_isr_handler to handle interrupt.
	_asm RETFIE 0 _endasm //Return from _high_isr and re-enable interrupts.
}

#pragma code _low_isr = 0x18 //Defines the following code section at address 0x18(low priority interrupt vector).

//Entry point on a low priority interrupt.
void _low_isr(void){
	INTCON = 0; //Disable interrupts.
	low_isr_handler(); //Call low_isr_handler to handle interrupt.
	_asm RETFIE 0 _endasm //Return from _low_isr and re-enable interrupts.
}
