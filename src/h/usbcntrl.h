/* usbcntrl.h

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

#ifndef USBCTRL_H
#define USBCTRL_H

/* Local Prototypes */
void clear_feature(void);
void set_address(void);
void get_descriptor(byte dsc);
void set_configuration(void);
void get_max_lun(void);
void check_std_request(void);

/* External Prototypes */
void send_csw(void);

/* External variables required by header */

//Endpoint control registers
extern volatile far BDT ep0Bo;
extern volatile far BDT ep0Bi;
extern volatile far BDT ep1Bo;
extern volatile far BDT ep1Bi;

//Transfer buffers
extern volatile far CTRL_TRF_SETUP SetupPkt; //control transfer host to dev
extern volatile far CTRL_TRF_DATA TrfData; //control transfer dev to host
extern volatile far USB_BUFFER msd_buffer; //bulk transfer host/dev to dev/host

extern PARITY parity; //parity data

extern TRANSFER_STATUS status; //transfer status data

//My device descriptors
extern rom usb_dev_dsc device_dsc; //device
extern rom usb_cfg_data usb_data; //configuration, interface, endpointa, endpointb

extern rom byte *gr_pSrc; //global rom byte pointer for data transfer

extern word bytes_to_send; //Holds bytes to send to/from host

//Handles checking standard USB requests
void check_std_request(void){
	switch(SetupPkt.bRequest){
		case CLR_FTR: clear_feature(); break;
		case SET_ADR: set_address(); break;
		case GET_DSC: get_descriptor(SetupPkt.bDscType); break;
		case SET_CFG: set_configuration(); break;
		case GET_MAX_LUN: get_max_lun(); break;
		default: break;
	}
}

//Clear feature
void clear_feature(void){
	parity.msdi_parity = 0; //reset ep1 in parity
	ep0Bi.CNT = 0; //send zero length packet
	ep0Bi.STAT = 0x80 | 0x40 | 0x08;
	
	send_csw(); //send csw
}

//set address - must not set address here, host needs a zero length packet first
void set_address(void){
	status.set_address = 1; //toggle set address bit
	
	ep0Bi.CNT = 0; //send zero length packet
	ep0Bi.STAT = 0x80 | 0x40 | 0x08;
}

//Handles sending device descriptors to host
void get_descriptor(byte dsc){
	byte x, *pDst;
	
	if(dsc == DEV_DSC){
		gr_pSrc = (rom byte *)&device_dsc;
		if(SetupPkt.wLength != sizeof(usb_dev_dsc)){
			SetupPkt.wLength = 8; //Windows will reset after receiving 8 bytes on first device desc. transfer
		}
	}else if(dsc == CFG_DSC){
		gr_pSrc = (rom byte *)&usb_data;
		if(SetupPkt.wLength != 9){
			SetupPkt.wLength = sizeof(usb_cfg_data);
		}
	}else if(dsc == DEV_QUA_DSC){ //stall on device qualifier descriptor since this device only runs in full speed
		ep0Bi.STAT = 0x80 | 0x04;
		ep0Bo.STAT = 0x80 | 0x04;
		return;
	}
	
	if(!bytes_to_send){ //set bytes to send
		bytes_to_send = SetupPkt.wLength;
	}
	
	if(bytes_to_send < 16){ //set current transfer amount - the buffer size is set to 16 bytes
		x = bytes_to_send;
	}else if(bytes_to_send > 16){
		x = 16;
	}else{
		x = 16;
		status.zero_packet = 1; //must send a zero length packet if bytes to send equals the buffer size
	}
	
	bytes_to_send -= x; //subtract the data about to be sent from total
	
	pDst = (byte *)&TrfData; //set data destination
	
	ep0Bi.CNT = x; //set bytes to be sent
	ep0Bi.ADR = (byte *)&TrfData; //set address of data to be sent
	 
	for(;x>0;x--, pDst++, gr_pSrc++){ //copy data from global data pointer into data destination(transfer buffer)
		*pDst = *gr_pSrc;
	}
	
	if(parity.con_parity){ //check parity and send data
		parity.con_parity = 0;
		ep0Bi.STAT = 0x80 | 0x40 | 0x08;
	}else{
		parity.con_parity = 1;
		ep0Bi.STAT = 0x80 | 0x08;
	}
}

//handles the set configuration request
void set_configuration(void){
	UEP1 = 0x1E; //setup EP1 as a bulk in/out endpoint
	
	ep0Bi.CNT = 0; //send zero length packet on control endpoint (EP0)
	ep0Bi.STAT = 0x80 | 0x40 | 0x08;
}

//Handles get max lun (logical unit number)
void get_max_lun(void){
	ep0Bi.CNT = 0; //set for zero length packet on ep0
	ep1Bo.CNT = 31; //set ep1 to receive 31 bytes of data from host
	ep1Bo.ADR = (byte *)&msd_buffer; //set buffer to receive data on ep1
	ep0Bi.STAT = 0x80 | 0x40 | 0x08;  //send zero length packet on ep0
	parity.msdo_parity = 1; //set ep1 out parity to 1
	ep1Bo.STAT = 0x80 | 0x08; //set to send data
}

#endif // USBCTRL_H
