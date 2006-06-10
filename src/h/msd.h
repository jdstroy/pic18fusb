/* msd.h

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

#ifndef MSD_H
#define MSD_H

#include "h/msd_data.h"

/* Local Prototypes */
void check_msd_request(void);
void msd_transfer(void);
void send_csw(void);
void inquiry_response_handler(void);
void read_format_capacity(void);
void read_capacity(void);
void request_sense_handler(void);
void read(void);
void mode_sense(void);
void test_unit_ready(void);
void prev_allow_med(void);
void verify(void);
void write(void);

/* External variables required by header */
extern word bytes_to_send;
extern dword datares;
extern rom byte *gr_pSrc;
extern byte *g_pSrc;
extern word tagL, tagH;
extern word mem_location;

void check_msd_request(void){
	switch(msd_buffer._CB[0]){
		case INQ_RESP: inquiry_response_handler(); break;
		case READ_FOR_CAP: read_format_capacity(); break;
		case READ_CAP: read_capacity(); break;
		case REQ_SENSE: request_sense_handler(); break;
		case READ: read(); break;
		case WRITE: write(); break;
		case MODE_SENSE: mode_sense(); break;
		case TEST_UNIT_RDY: test_unit_ready(); break;
		case PREV_ALLOW_MED: prev_allow_med(); break;
		case VERIFY: verify(); break;
		default: PORTD = 0x0F; while(1); break;
	}
}

void msd_transfer(void){
	byte x, *pDst;
	
	if(bytes_to_send < 64){
		x = bytes_to_send;
	}else{
		x = 64;
	}
	
	bytes_to_send -= x;
	
	if(!bytes_to_send && !datares){
		status.send_csw = 1;
	}else if(!bytes_to_send && datares){
		status.data_res = 1;
	}
	
	ep1Bi.CNT = x;
	ep1Bi.ADR = (byte *)&msd_buffer;
	
	pDst = (byte *)&msd_buffer;
	
	if(gr_pSrc){
		for(;x>0;x--){
			*pDst = *gr_pSrc;
			pDst++;
			gr_pSrc++;
		}
	}else if(g_pSrc){
		for(;x>0;x--, pDst++, g_pSrc++){
			*pDst = *g_pSrc;
		}
	}
	
	if(parity.msdi_parity){
		parity.msdi_parity = 0;
		ep1Bi.STAT = 0x80 | 0x40 | 0x08;
	}else{
		parity.msdi_parity = 1;
		ep1Bi.STAT = 0x80 | 0x08;
	}
}

void send_csw(void){
	
	msd_buffer.signL = 0x5355; //signature low byte
	msd_buffer.signH = 0x5342; //signature high byte
	msd_buffer.tagL = tagL; //tag low byte
	msd_buffer.tagH = tagH; //tag high byte
	msd_buffer.dataresL = (datares & 0x0000FFFF);
	msd_buffer.dataresH = ((datares & 0xFFFF0000) >> 16);
	if(status.csw_error){
		status.csw_error = 0;
		msd_buffer.stat = 1;
	}else{
		msd_buffer.stat = 0;
	}
	
	ep1Bi.CNT = 13;
	ep1Bi.ADR = (byte *)&msd_buffer;
	if(parity.msdi_parity){
		parity.msdi_parity = 0;
		ep1Bi.STAT = 0x80 | 0x40 | 0x08;
	}else{
		parity.msdi_parity = 1;
		ep1Bi.STAT = 0x80 | 0x08;
	}
}

void inquiry_response_handler(void){
	
	tagL = msd_buffer.tagL;
	tagH = msd_buffer.tagH;
	
	bytes_to_send = sizeof(INQUIRY_RESPONSE);
	gr_pSrc = (rom byte *)&inq_resp;
	
	datares = (((msd_buffer.dataresH << 16) | msd_buffer.dataresL) - bytes_to_send);
	
	msd_transfer();
}

void read_format_capacity(void){
	
	tagL = msd_buffer.tagL;
	tagH = msd_buffer.tagH;
	status.csw_error = 1;
	status.error1 = 1;
	
	bytes_to_send = 0;
	datares = (((msd_buffer.dataresH << 16) | msd_buffer.dataresL) - bytes_to_send);
	
	msd_transfer();
	
	/*
	bytes_to_send = sizeof(CAPACITY_LIST);
	gr_pSrc = (byte *)&cap_list;
	
	datares = (((msd_buffer.dataresH << 16) | msd_buffer.dataresL) - bytes_to_send);
	
	msd_transfer();
	*/
	
}

void read_capacity(void){
	
	tagL = msd_buffer.tagL;
	tagH = msd_buffer.tagH;
	
	gr_pSrc = (rom byte *)&capacity;
	g_pSrc = 0;
	bytes_to_send = sizeof(CAPACITY);
	datares = (((msd_buffer.dataresH << 16) | msd_buffer.dataresL) - bytes_to_send);
	
	msd_transfer();
	
}

void request_sense_handler(void){
	byte x, *pDst;
	REQUEST_SENSE rs = {0};
	
	rs.error_code = 0x70;
	rs.additional_sense_length = 10;
	if(status.error0){
		status.error0 = 0;
		rs.sense_key = 0x03;
		rs.additional_sense_code = 0x10;
		rs.additional_sense_code_qualifier = 0x00;
	}else if(status.error1){
		status.error1 = 0;
		rs.sense_key = 0x05;
		rs.additional_sense_code = 0x20;
		rs.additional_sense_code_qualifier = 0x00;
	}else if(status.error2){
		status.error2 = 0;
		rs.sense_key = 0x05;
		rs.additional_sense_code = 0x24;
		rs.additional_sense_code_qualifier = 0x00;
	}
	
	status.send_csw = 1;
	datares = 0;
	tagL = msd_buffer.tagL;
	tagH = msd_buffer.tagH;
	
	pDst = (byte *)&msd_buffer;
	
	for(x=0;x<18;x++, pDst++){
		*pDst = rs.data[x];
	}
	
	ep1Bi.CNT = 18;
	ep1Bi.ADR = (byte *)&msd_buffer;
	if(parity.msdi_parity){
		parity.msdi_parity = 0;
		ep1Bi.STAT = 0x80 | 0x40 | 0x08;
	}else{
		parity.msdi_parity = 1;
		ep1Bi.STAT = 0x80 | 0x08;
	}
}

void read(void){
	
	byte x, y, *pDst;
	
	if(!status.disk_read){
		tagL = msd_buffer.tagL;
		tagH = msd_buffer.tagH;
		bytes_to_send = ((msd_buffer.dataresH << 16) | msd_buffer.dataresL);
		datares = ((msd_buffer.dataresH << 16) | msd_buffer.dataresL);
		mem_location = ((msd_buffer._CB[5] * 512) + 0x1000);
		status.disk_read = 1;
	}
	
	TBLPTR = (mem_location + (datares - bytes_to_send));
	
	if(bytes_to_send < 64){
		x = bytes_to_send;
	}else{
		x = 64;
	}
	
	bytes_to_send -= x;
	
	if(!bytes_to_send){
		status.disk_read = 0;
		status.send_csw = 1;
		datares = 0;
	}
	
	ep1Bi.CNT = x;
	ep1Bi.ADR = (byte *)&msd_buffer;
	
	pDst = (byte *)&msd_buffer;
	
	for(;x>0;x--, TBLPTR++, pDst++){
		_asm TBLRD _endasm
		*pDst = TABLAT;
	}
	
	if(parity.msdi_parity){
		parity.msdi_parity = 0;
		ep1Bi.STAT = 0x80 | 0x40 | 0x08;
	}else{
		parity.msdi_parity = 1;
		ep1Bi.STAT = 0x80 | 0x08;
	}
	
}

void write(void){
	byte x, y, z, *pDst;
	word TBLPTR_tmp0, TBLPTR_tmp1;
	
	if(!status.disk_write){ //first call to write function?
		tagL = msd_buffer.tagL; //setup tags
		tagH = msd_buffer.tagH;
		bytes_to_send = ((msd_buffer.dataresH << 16) | msd_buffer.dataresL); //get number of bytes to receive
		datares = ((msd_buffer.dataresH << 16) | msd_buffer.dataresL); //also number of bytes to receive
		mem_location = ((msd_buffer._CB[5] * 512) + 0x1000);
		
		status.disk_write = 1; //set write flag
		
		TBLPTR = (mem_location + (datares - bytes_to_send));
		x = bytes_to_send / 64;
		for(;x>0;x--, TBLPTR+=64){
			EECON1bits.EEPGD = 1;
			EECON1bits.CFGS = 0;
			EECON1bits.WREN = 1;
			EECON1bits.FREE = 1;
			EECON2 = 0x55;
			EECON2 = 0x0AA;
			EECON1bits.WR = 1;
		}
		
		if(bytes_to_send < 64){ //check if we will receive less than 64 bytes
			ep1Bo.CNT = bytes_to_send;
		}else{
			ep1Bo.CNT = 64;
		}
		
		//setup to receive data
		ep1Bo.ADR = (byte *)&msd_buffer;
		if(parity.msdo_parity){
			parity.msdo_parity = 0;
			ep1Bo.STAT = 0x80 | 0x40 | 0x08;
		}else{
			parity.msdo_parity = 1;
			ep1Bo.STAT = 0x80 | 0x08;
		}
		
		return; //leave write function
	}
	
	if(mem_location == 0x6000){
		TBLPTR = 0x6000;
		_asm TBLRD _endasm
		PORTD |= (TABLAT & 1);
	}
	if(mem_location == 0x6200){
		TBLPTR = 0x6200;
		_asm TBLRD _endasm
		PORTD |= (TABLAT & 2);
	}
	
	TBLPTR = (mem_location + (datares - bytes_to_send));
	TBLPTR_tmp0 = TBLPTR;
	
	bytes_to_send -= ep1Bo.CNT; //subtract number of bytes just received from total
	
	y = 0;
	if(ep1Bo.CNT < 32){
		x = ep1Bo.CNT;
	}else if(ep1Bo.CNT > 32){
		x = 32;
		y = (ep1Bo.CNT - 32);
	}else{
		x = 32;
	}
	
	for(z=0;x>0;x--, z++, TBLPTR++){
		TABLAT = msd_buffer._byte[z];
		_asm TBLWT _endasm
	}
	
	TBLPTR_tmp1 = TBLPTR;
	
	TBLPTR = TBLPTR_tmp0;
	
	EECON1bits.EEPGD = 1;
	EECON1bits.CFGS = 0;
	EECON1bits.WREN = 1;
	EECON2 = 0x55;
	EECON2 = 0x0AA;
	EECON1bits.WR = 1;
	
	TBLPTR = TBLPTR_tmp1;
	
	if(y){
		for(;y>0;y--, z++, TBLPTR++){
			TABLAT = msd_buffer._byte[z];
			_asm TBLWT _endasm
		}
		TBLPTR = TBLPTR_tmp1;
		EECON1bits.EEPGD = 1;
		EECON1bits.CFGS = 0;
		EECON1bits.WREN = 1;
		EECON2 = 0x55;
		EECON2 = 0x0AA;
		EECON1bits.WR = 1;
	}
	
	if(!bytes_to_send){
		status.disk_write = 0;
		datares = 0;
		send_csw();
		return;
	}
	
	if(bytes_to_send < 64){ //check if we will receive less than 64 bytes
		ep1Bo.CNT = bytes_to_send;
	}else{
		ep1Bo.CNT = 64;
	}
	
	//setup to receive data
	ep1Bo.ADR = (byte *)&msd_buffer;
	if(parity.msdo_parity){
		parity.msdo_parity = 0;
		ep1Bo.STAT = 0x80 | 0x40 | 0x08;
	}else{
		parity.msdo_parity = 1;
		ep1Bo.STAT = 0x80 | 0x08;
	}
}

void mode_sense(void){
	
		tagL = msd_buffer.tagL;
		tagH = msd_buffer.tagH;
		
		bytes_to_send = 4;
		
		datares = (((msd_buffer.dataresH << 16) | msd_buffer.dataresL) - bytes_to_send);
		
		msd_buffer._byte[0] = 3;
		msd_buffer._byte[1] = 0;
		msd_buffer._byte[2] = 0;
		msd_buffer._byte[3] = 0;
		
		gr_pSrc = 0;
		g_pSrc = 0;
		
		msd_transfer();
}

void test_unit_ready(void){
	
	tagL = msd_buffer.tagL;
	tagH = msd_buffer.tagH;
	
	bytes_to_send = 0;
	datares = 0;
	
	send_csw();
}

void prev_allow_med(void){
	
	tagL = msd_buffer.tagL;
	tagH = msd_buffer.tagH;
	
	bytes_to_send = 0;
	datares = 0;
	
	if(msd_buffer._CB[4]){
		status.csw_error = 1;
		status.error2 = 1;
	}
	
	send_csw();
}

void verify(void){
	
	tagL = msd_buffer.tagL;
	tagH = msd_buffer.tagH;
	bytes_to_send = 0;
	datares = 0;
	
	send_csw();
}

#endif // MSD_H
