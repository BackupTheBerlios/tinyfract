#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	printf("%d\n",(getenv(argv[1]))==NULL ? 0 : 1);
	return 0;
}
