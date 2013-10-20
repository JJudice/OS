/* PTest.c
 *	file used to test exec and exit
 */

#include "syscall.h"

int
main()
{
	Exec("../test/loop");
	Exec("../test/loop");
	Exec("../test/loop");
	
	Exit(0);
	
	
}
