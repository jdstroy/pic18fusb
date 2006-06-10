/* main.c

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
#include "h/typedefs.h" //Defines types and defined constants.
#include "h/usb.h" //Defines data collection types(structs/unions).
#include "h/system.h" //Functions related to system.
#include "h/interrupts.h" //Functions for handling interrupts.
#include "h/descriptors.h" //USB standard device descriptors.

#pragma udata usbram4 = 0x400 //Defines the following section at ram location 0x400(USB ram).

//Endpoint 0(default - control endpoint) control registers.
volatile far BDT ep0Bo; //out.
volatile far BDT ep0Bi; //in.

//Endpoint 1(used as bulk in and out for mass storage class).
volatile far BDT ep1Bo; //out.
volatile far BDT ep1Bi; //in.

volatile far CTRL_TRF_SETUP SetupPkt; //Buffer for control transfer setup packet.
volatile far CTRL_TRF_DATA TrfData; //Buffer for data being sent to host.
volatile far USB_BUFFER msd_buffer; //Buffer for data in/out during bulk transfers.

/* uninitialized variables */
#pragma udata //Define as uninizialized data section.

byte usb_state; //Holds current USB state.
PARITY parity; //Parity information for transfers.
TRANSFER_STATUS status; //Holds conditional information during transfers.
rom byte *gr_pSrc; //Global pointer for rom data.
byte *g_pSrc; //Global pointer for data.
word bytes_to_send; //Holds bytes to send during transfer.
dword datares; //Data resolution (data expected - data sent).
word mem_location;
word tagL, tagH; //Tag words for CBW and CSW (bulk transfers).

#pragma code //Define as code section.

void main(void){
	TBLPTR = 0;
	init(); //Run initialize routine (see system.h).
	check_usb_bus(); //Check usb bus for connectivity (see system.h).
	while(1);
}
