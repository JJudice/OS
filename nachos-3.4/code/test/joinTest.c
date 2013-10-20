/* joinTest.c
 * file used to test join, exec, and exit
 */

#include "syscall.h"

int
main()
{
	Join("../test/loop");
	//Exec("../test/halt");
	//Exec("../test/halt");
	Exit(0);
    /* not reached */
}
