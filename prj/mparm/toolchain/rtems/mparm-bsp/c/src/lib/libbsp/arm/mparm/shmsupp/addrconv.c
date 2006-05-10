/*  Shm_Convert_address
 *
 *  No address range conversion is required.
 *
 *  Input parameters:
 *    address - address to convert
 *
 *  Output parameters:
 *    returns - converted address
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  addrconv.c,v 1.4 1999/11/30 19:57:53 joel Exp
 */

#include <rtems.h>
#include <bsp.h>
#include <shm_driver.h>

void *Shm_Convert_address(
  void *address
)
{
  return ( address );
}
