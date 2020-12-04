/* userret.c - userret */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 * userret  --  entered when a process exits by return
 *------------------------------------------------------------------------
 */
int userret()
{
        kprintf("\n Kill is called while returning process \n");
	kill( getpid() );
        return(OK);
}
