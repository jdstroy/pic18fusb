/* typedefs.h

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

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#define EP0_BUFF 16
#define EP1_BUFF 32

#define DETACHED 0
#define ATTACHED 1
#define POWERED  2
#define DEFAULT  3

#define GET_MAX_LUN 0xFE
#define SET_CFG 9
#define GET_DSC	6
#define SET_ADR	5
#define CLR_FTR 1

#define DEV_DSC	1
#define CFG_DSC	2
#define DEV_QUA_DSC 6

#define INQ_RESP	0x12
#define READ_FOR_CAP	0x23
#define READ_CAP	0x25
#define READ		0x28
#define TEST_UNIT_RDY	0x00
#define MODE_SENSE	0x1A
#define REQ_SENSE	0x03
#define PREV_ALLOW_MED	0x1E
#define VERIFY		0x2F
#define WRITE		0x2A


#define usb_bus_sense     (PORTA & 0x02)

typedef unsigned char   byte;           // 8-bit
typedef unsigned int    word;           // 16-bit
typedef unsigned long   dword;          // 32-bit

#endif
