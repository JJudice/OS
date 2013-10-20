/* loop.c
*	file used to test exec and exit
*/
#include "syscall.h"
int main()
{
	//Exec("../test/ma");
	Exec("../test/PTest");
	//Exec("../test/halt");
	Exit(0);
}
