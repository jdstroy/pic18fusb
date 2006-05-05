/* msd_datatypes.h

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

#ifndef MSDDATATYPES_H
#define MSDDATATYPES_H

typedef struct _INQUIRY_RESPONSE{
	byte peripheral_dev_type;
	byte rmd;
	byte version;
	byte resp_data_form;
	byte add_length;
	byte reserved[3];
	byte ven_info[8];
	byte prod_ident[16];
	byte prod_rev_lev[4];
} INQUIRY_RESPONSE;

typedef struct _CAPACITY_LIST{
	byte reserved[3];
	byte list_length;
	byte num_blocks[4];
	byte dc;
	byte block_length[3];
	byte for_cap_desc[8];
} CAPACITY_LIST;

typedef struct _CAPACITY{
	word last_logical_block_MSB;
	word last_logical_block_LSB;
	word block_length_MSB;
	word block_length_LSB;
} CAPACITY;

typedef union _REQUEST_SENSE{
	struct{
		byte data[18];
	};
	struct{
		byte error_code;
		byte reserved;
		byte sense_key;
		byte information[4];
		byte additional_sense_length;
		byte reserved2[4];
		byte additional_sense_code;
		byte additional_sense_code_qualifier;
		byte reserved3[4];
	};
} REQUEST_SENSE;

typedef struct _MBR{
	byte reserved0[446];
	byte boot_indicator;
	byte starting_head;
	byte starting_sector;
	byte starting_cylinder;
	byte system_id;
	byte ending_head;
	byte ending_sector;
	byte ending_cylinder;
	dword relative_sectors;
	dword total_sectors;
	byte reserved1[48];
	word signature;
} MBR;

#endif // MSDDATATYPES_H
