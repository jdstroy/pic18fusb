/* usb.h

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

#ifndef USB_H
#define USB_H

typedef union _PARITY{
	struct{
		byte data;
	};
	struct{
		unsigned con_parity : 1;
		unsigned msdo_parity : 1;
		unsigned msdi_parity : 1;
		unsigned reserved : 6;
	};
} PARITY;

typedef union _STATUS{
	struct{
		word data;
	};
	struct{
		unsigned zero_packet : 1;
		unsigned set_address : 1;
		unsigned send_csw : 1;
		unsigned csw_error : 1;
		unsigned data_res : 1;
		unsigned error0 : 1;
		unsigned error1 : 1;
		unsigned error2 : 1;
		unsigned disk_read : 1;
		unsigned disk_write : 1;
		unsigned reserved : 6;
	};
} TRANSFER_STATUS;

typedef struct _USB_DEV_DSC{
    byte bLength;       byte bDscType;      word bcdUSB;
    byte bDevCls;       byte bDevSubCls;    byte bDevProtocol;
    byte bMaxPktSize0;  word idVendor;      word idProduct;
    word bcdDevice;     byte iMFR;          byte iProduct;
    byte iSerialNum;    byte bNumCfg;
} usb_dev_dsc;

typedef struct _USB_CFG_DSC{
    byte bLength;       byte bDscType;      word wTotalLength;
    byte bNumIntf;      byte bCfgValue;     byte iCfg;
    byte bmAttributes;  byte bMaxPower;
} usb_cfg_dsc;

typedef struct _USB_INTF_DSC{
    byte bLength;       byte bDscType;      byte bIntfNum;
    byte bAltSetting;   byte bNumEPs;       byte bIntfCls;
    byte bIntfSubCls;   byte bIntfProtocol; byte iIntf;
} usb_intf_dsc;

typedef struct _USB_EP_DSC{
    byte bLength;       byte bDscType;      byte bEPAdr;
    byte bmAttributes;  word wMaxPktSize;   byte bInterval;
} usb_ep_dsc;

typedef struct _USB_CFG_DATA{
	usb_cfg_dsc cfg;
	usb_intf_dsc inter;
	usb_ep_dsc ep1a;
	usb_ep_dsc ep1b;
} usb_cfg_data;

typedef union _BDT{
	struct{
		byte STAT;
		byte CNT;
		byte ADRL;
		byte ADRH;
	};
	struct{
		byte :8;
		byte :8;
		byte *ADR;
	};
	struct{
		unsigned UOWN : 1;
		unsigned DTS : 1;
		unsigned KEN : 1;
		unsigned INCDIS : 1;
		unsigned DTSEN : 1;
		unsigned BSTALL : 1;
		unsigned BC9 : 1;
		unsigned BC8 : 1;
	};
} BDT;

typedef union _CTRL_TRF_SETUP{
	struct
	{
		byte bmRequestType;
		byte bRequest;    
		word wValue;
		word wIndex;
		word wLength;
	};
	struct
	{
        unsigned Recipient:5;           //Device,Interface,Endpoint,Other
        unsigned RequestType:2;         //Standard,Class,Vendor,Reserved
        unsigned DataDir:1;             //Host-to-device,Device-to-host
        unsigned :8;
        byte bFeature;                  //DEVICE_REMOTE_WAKEUP,ENDPOINT_HALT
        unsigned :8;
        unsigned :8;
        unsigned :8;
        unsigned :8;
        unsigned :8;
	};
	struct
	{
        unsigned :8;
        unsigned :8;
        byte bCfgValue;                 //Configuration Value 0-255
        byte bCfgRSD;                   //Must equal zero (Reserved)
        unsigned :8;
        unsigned :8;
        unsigned :8;
        unsigned :8;
	};
	struct{
		unsigned : 8;
		unsigned : 8;
		byte bDscIndex;
		byte bDscType;
		word wLangID;
		unsigned : 8;
		unsigned : 8;
	};
	struct
	{
        unsigned :8;
        unsigned :8;
        byte bDevADR;                   //Device Address 0-127
        byte bDevADRH;                  //Must equal zero
        unsigned :8;
        unsigned :8;
        unsigned :8;
        unsigned :8;
	};
} CTRL_TRF_SETUP;

typedef union _CTRL_TRF_DATA{
	struct{
		byte _byte[16];
	};
} CTRL_TRF_DATA;

typedef union _usb_buffer{
	struct{
		byte _byte[64];
	};
	struct{
		word signL;
		word signH;
		word tagL;
		word tagH;
		word trnlenL;
		word trnlenH;
		byte flags;
		byte lun;
		byte CBlen;
		byte _CB[16];
	};
	struct{
		word signL;
		word signH;
		word tagL;
		word tagH;
		word dataresL;
		word dataresH;
		byte stat;
	};
} USB_BUFFER;

#endif // USB_H
