/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the system configuration definition.
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

// Compiler switch (do not change these settings)
#include "compiler.h"             // Compiler definitions
#include <avr/io.h>                    // Use AVR-GCC library


//! @defgroup global_config Application configuration
//! @{

#include "conf_scheduler.h" //!< Scheduler tasks declaration

// Board defines (do not change these settings)
#define  STK526   1
#define  STK600   2
#define  USBIO    3

//! Enable or not the ADC usage
#undef  USE_ADC
//! To include proper target hardware definitions, select
//! target board (STK526 or STK600)
#define TARGET_BOARD USBIO

#if (TARGET_BOARD==STK526)
   #include "lib_board\stk_526\stk_526.h"
#elif (TARGET_BOARD==STK600)
   #include "lib_board\stk_600\stk_600.h"
#elif (TARGET_BOARD==USBIO)
   #include "usbio.h"
#else
   #error TARGET_BOARD must be defined somewhere
#endif

//! CPU core frequency in kHz
#define FOSC 8000


// -------- END Generic Configuration -------------------------------------

// UART Sample configuration, if we have one ... __________________________
#define BAUDRATE        38400
#define USE_UART2

#define uart_putchar putchar
#define r_uart_ptchar int
#define p_uart_ptchar int

//! @}

#endif // _CONFIG_H_

