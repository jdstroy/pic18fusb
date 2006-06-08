/* interrupts.h

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

#ifndef INT_H
#define INT_H

#include <p18f4550.h>
#include "h/usbcntrl.h"
#include "h/msd.h"

/* External variables required byte header */
extern byte usb_state;
extern word bytes_to_send;
extern PARITY parity; // required by high_isr_handler
extern TRANSFER_STATUS status;
extern volatile far BDT ep0Bo;
extern volatile far BDT ep0Bi;
extern volatile far BDT ep1Bo;
extern volatile far BDT ep1Bi;
extern volatile far CTRL_TRF_SETUP SetupPkt;
extern volatile far CTRL_TRF_DATA TrfData;

//Handles high priority interrupts
void high_isr_handler(void){

	if(UIRbits.URSTIF && UIEbits.URSTIE){ //Reset
		usb_state = DEFAULT; //Set USB device to DEFAULT state
		UIR = 0; //Mask all USB interrupt flags
		UIE = 0x29; //Enable USB TRNIE and URSTIE (transfer and reset)
		UEIR = 0;
		UEIE = 0x0; //Disable all USB error interrupts
		
		UADDR = 0x00; //Set device to default address 0
		
		//Here all endpoints except 0 are disabled
		UEP1=0x00;UEP2=0x00;UEP3=0x00;
		UEP4=0x00;UEP5=0x00;UEP6=0x00;UEP7=0x00;
		UEP8=0x00;UEP9=0x00;UEP10=0x00;UEP11=0x00;
		UEP12=0x00;UEP13=0x00;UEP14=0x00;UEP15=0x00;
		
		//Endpoint 0 is configured as Input/Output/Control with Handshaking
		//enabled
		UEP0 = 0x06 | 0x10;
		
		parity.con_parity = 1;
		parity.msdo_parity = 0;
		parity.msdi_parity = 0;
		status.data = 0;
		bytes_to_send = 0;
		
		//Wait for USTAT FIFO buffer to flush
		while(UIRbits.TRNIF){
			UIRbits.TRNIF = 0;
		}
		
		UCONbits.PKTDIS = 0; //Enable packet transfers
		
		ep0Bo.CNT = 16; //Set SIE to transfer 16 bytes
		ep0Bo.ADR = (byte *)&SetupPkt; //Buffer to place bytes
		ep0Bi.STAT = 0; //Set input (DEV TO HOST) for CPU control
		ep0Bo.STAT = 0x80 | 0x08; //Set output for SIE control and DATA 0
	}
	
	if(UIRbits.TRNIF && UIEbits.TRNIE){ //Transfer complete
		if(!USTAT){ //Check if setup packet was received on endpoint 0
			UCONbits.PKTDIS = 0; //disable packet processing
			check_std_request(); //check request (see usbcntrl.h)
		}else if(USTAT == 0x04){ //check if data was sent to host on endpoint 0
			if(bytes_to_send){ //check if data is left to send
				get_descriptor(0); //call to send remaining data (see usbcntrl.h)
			}else{
				if(status.set_address){ //check if set address status bit was set
					status.set_address = 0; //clear set address bit
					UADDR = SetupPkt.bDevADR; //set the device address
				}else if(status.zero_packet){ //check if zero packet status bit was set
					status.zero_packet = 0; //clear
					ep0Bi.CNT = 0; //send a zero length packet
					ep0Bi.STAT = 0x80 | 0x40 | 0x08;
				}
				
				parity.con_parity = 1; //set parity for endpoint 0 to 1
				
				ep0Bo.CNT = 16; //setup to receive next setup packet
				ep0Bo.ADR = (byte *)&SetupPkt;
				ep0Bo.STAT = 0x80 | 0x08;
			}
		}else if(USTAT == 0x08){ //check if packet was received on endpoint 1
			if(status.disk_write){
				write();
			}else{
				check_msd_request(); //check request (see msd.h)
			}
		}else if(USTAT == 0x0C){ //check if packet was sent on endpoint 1
			if(bytes_to_send && !status.disk_read){ //check if data is left to send
				msd_transfer(); //send data
			}else if(status.send_csw){ //check if send csw status bit was set
				status.send_csw = 0; //clear
				send_csw(); //send csw to host
			}else if(status.data_res){ //check if there was data resolution (data expected - data sent)
				status.data_res = 0; //clear
				ep1Bi.STAT = 0x80 | 0x04; //stall endpoint 1
				ep1Bo.STAT = 0x80 | 0x04;
			}else if(status.disk_read){
				read();
			}else if(status.disk_write){
			}else{
				ep1Bo.CNT = 31; //setup to receive a new packet on endpoint 1
				ep1Bo.ADR = (byte *)&msd_buffer;
				if(parity.msdo_parity){
					parity.msdo_parity = 0;
					ep1Bo.STAT = 0x80 | 0x40 | 0x08;
				}else{
					parity.msdo_parity = 1;
					ep1Bo.STAT = 0x80 | 0x08;
				}
			}
		}
		
		UIRbits.TRNIF = 0; //clear transfer interrupt
	}
	
	if(UIRbits.STALLIF && UIEbits.STALLIE){
			ep0Bo.CNT = 16;
			ep0Bo.ADR = (byte *)&SetupPkt;
			ep0Bo.STAT = 0x80 | 0x08;
			UEP0bits.EPSTALL = 0;
			UIRbits.STALLIF = 0;
	}
	
	if(!UIRbits.TRNIF && !UIRbits.URSTIF && !UIRbits.STALLIF){ //if both interrupts are clear, clear USBIF interrupt flag (not sure if this is necessary)
		PIR2bits.USBIF = 0;
	}
}

//Handles low priority interrupts
void low_isr_handler(void){
}

#endif // INT_H
