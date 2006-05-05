/* descriptors.h

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

#ifndef DSC_H
#define DSC_H

#pragma romdata

rom usb_dev_dsc device_dsc = {
	sizeof(usb_dev_dsc),    // Size of this descriptor in bytes
	1,                // DEVICE descriptor type
	0x0200,                 // USB Spec Release Number in BCD format
	0,                             /* bDeviceClass */
	0,                             /* bDeviceSubclass */
	0,                             /* bDeviceProtocol */
	16,                             /* bMaxPacketSize 16 Bytes */
	0x04D8,                        /* idVendor */
	0x0004,                        /* idProduct*/
	0x0000,                        /* bcdDevice */
	0,                             /* iManufacturer String Index */
	0,                             /* iProduct String Index */
	0,                             /* iSerialNumber String Index */
	1                              /* bNumberConfigurations */
};

rom usb_cfg_data usb_data={
    sizeof(usb_cfg_dsc), /* blength */
    2, /* bdescriptortype */
    (sizeof(usb_cfg_dsc) + sizeof(usb_intf_dsc) + sizeof(usb_ep_dsc) + sizeof(usb_ep_dsc)),       /* wtotallength */
    1,                             /* bnuminterfaces */
    1,                             /* bconfigurationvalue */
    0,                             /* iconfiguration string index */
    0x80,                          /* bmattributes bus powered, no remote wakeup */
    0x32,                           /* bmaxpower, 100ma */
    
    sizeof(usb_intf_dsc), /* blength */
    4,     /* bdescriptortype */
    0,                             /* binterface number */
    0,                             /* balternatesetting */
    2,                             /* bnumendpoints */
    0x08,                          /* binterfaceclass (mass storage) */
    0x06,                          /* binterfacesubclass */
    0x50,                          /* binterfaceprotocol */
    0,                              /* iinterface string index */
    
    sizeof(usb_ep_dsc),  /* blength */
    5,      /* bdescriptortype */
    0x81,                          /* bendpoint address ep1 in */
    0x02,                          /* bmattributes - interrupt */
    0x40,                        /* wmaxpacketsize */
    0x00,                           /* binterval */
    
    sizeof(usb_ep_dsc),  /* blength */
    5,      /* bdescriptortype */
    0x01,                          /* bendpoint address ep1 out */
    0x02,                          /* bmattributes - interrupt */
    0x40,                        /* wmaxpacketSize */
    0x00                           /* bInterval */
};

#pragma code

#endif DSC_H
