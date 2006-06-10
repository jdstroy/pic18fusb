/* msd_data.h

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

#ifndef MSDDATA_H
#define MSDDATA_H

#include "h/msd_datatypes.h"

#pragma romdata

rom INQUIRY_RESPONSE inq_resp = {
	0x00,
	0x80,
	0x04,
	0x02,
	0x20,
	0x00,
	0x00,
	0x00,
	"Didlet",
	"MSD",
	"1"
};

rom CAPACITY capacity = {
	0x0000,
	0x3000,
	0x0000,
	0x0002
};

#pragma romdata disk=0x1000

/*rom MBR mbr = {
	{0},
	0x80,
	0x01,
	0x01,
	0x00,
	0x04,
	0x01,
	0x40,
	0x00,
	0x01000000,
	0x40000000,
	{0},
	0xAA55
};*/

#pragma code

#endif // MSDDATA_H
