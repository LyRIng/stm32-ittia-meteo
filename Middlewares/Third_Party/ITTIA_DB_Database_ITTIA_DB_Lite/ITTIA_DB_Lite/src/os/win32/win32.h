/**************************************************************************/
/*                                                                        */
/*      Copyright (c) 2005-2023 by ITTIA L.L.C. All rights reserved.      */
/*                                                                        */
/*  This software is copyrighted by and is the sole property of ITTIA     */
/*  L.L.C.  All rights, title, ownership, or other interests in the       */
/*  software remain the property of ITTIA L.L.C.  This software may only  */
/*  be used in accordance with the corresponding license agreement.  Any  */
/*  unauthorized use, duplication, transmission, distribution, or         */
/*  disclosure of this software is expressly forbidden.                   */
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */
/*  written consent of ITTIA L.L.C.                                       */
/*                                                                        */
/*  ITTIA L.L.C. reserves the right to modify this software without       */
/*  notice.                                                               */
/*                                                                        */
/*  info@ittia.com                                                        */
/*  https://www.ittia.com                                                 */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

#ifndef WIN32_H
#define WIN32_H

#include "ittia/os/os_config.h"

#if defined(OS_WIN32)

#include "ittia/os/std/windows.h"

C_HEADER_BEGIN

#define WIN_NT_PLATFORM 0x100
#define WIN_9X_PLATFORM 0x200
#define WIN_CE_PLATFORM 0x300

#define is_winnt() (get_win32_kind() == WIN_NT_PLATFORM)
#define is_win9x() (get_win32_kind() == WIN_9X_PLATFORM)

#ifdef HAVE_WIN32_THREADS

extern CRITICAL_SECTION os_win32_dynamic_init;
extern DWORD            os_win32_self;

#endif

dbstatus_t os_win32_error(void);
int get_win32_kind(void);

C_HEADER_END

#endif /* OS_WIN32 */

#endif /* WIN32_H */
