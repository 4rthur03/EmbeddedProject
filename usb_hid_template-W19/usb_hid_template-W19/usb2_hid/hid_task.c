/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file manages the generic HID IN/OUT task.
//!
//! - Compiler:           IAR EWAVR and GNU GCC for AVR
//! - Supported devices:  AT90USB162, AT90USB82
//!
//! \author               Atmel Corporation: http://www.atmel.com \n
//!                       Support and FAQ: http://support.atmel.no/
//!
//! ***************************************************************************

/* Copyright (c) 2007, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//_____  I N C L U D E S ___________________________________________________

#include "config.h"
#include "conf_usb.h"
#include "hid_task.h"
#include "usb_drv.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "usb_specific_request.h"



//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

void hid_report_out  (void);
void hid_report_in   (void);

void hid_task_init(void)
{
}

//! @brief Entry point of the HID generic communication task
//! This function manages IN/OUT report management.
//!
void hid_task(void)
{
   if(!Is_device_enumerated())          // Check USB HID is enumerated
      return;

   hid_report_out();
   hid_report_in();
}

//! @brief Get data report from Host
//!
void hid_report_out(void)
{
	U8 control;
	U8 report[LENGTH_OF_REPORT_OUT-1];
	U8 i;
	
	Usb_select_endpoint(EP_HID_OUT);
	if(Is_usb_receive_out())
	{

		// CPHE412: ADD CODE HERE TO PROCESS DATA COMING FROM HOST
		if (Usb_byte_counter() >= LENGTH_OF_REPORT_OUT)
		{
			control = Usb_read_byte();	// Read the control byte

			for (i = 0 ; i < (LENGTH_OF_REPORT_OUT-1) ; i++)
			{
				report[i] = Usb_read_byte();
			}

			if (control & IO_WRITE_DDRB)
				DDRB = report[0];
			if (control & IO_WRITE_DDRC)
				DDRC = report[1];
			if (control & IO_WRITE_DDRD)
				DDRD = report[2];
			if (control & IO_WRITE_PORTB)
				PORTB = report[3];
			if (control & IO_WRITE_PORTC)
				PORTC = report[4];
			if (control & IO_WRITE_PORTD)
				PORTD = report[5];
		}
	
		Usb_ack_receive_out();

   }
}


//! @brief Send data report to Host
//!
void hid_report_in(void)
{
	Usb_select_endpoint(EP_HID_IN);
	if(!Is_usb_write_enabled())
		return;                                // Not ready to send report
   
	Usb_write_byte(DDRB);
	Usb_write_byte(DDRC);
	Usb_write_byte(DDRD);
	Usb_write_byte(PINB);
	Usb_write_byte(PINC);
	Usb_write_byte(PIND);


	// CPHE412: ADD CODE HERE TO SEND DATA TO HOST
	// Gets called every time the device needs an IN
	Usb_ack_in_ready();			// Send data over the USB
}
