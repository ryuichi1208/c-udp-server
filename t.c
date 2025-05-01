#define t 1

#ifdef t
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#endif

// macOSかどうかを判定する
#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_MAC
#define IS_MACOS 1
#else

int main(int argc, char *argv[])
{
    int i;
    char *p;
    char buf[1024];

    if (argc < 2) {
	fprintf(stderr, "Usage: %s <string>\n", argv[0]);
	return 1;
    }

    for (i = 1; i < argc; i++) {
	p = argv[i];
	while (*p) {
	    if (*p == ' ') {
		*p = '\0';
	    }
	    p++;
	}
	printf("%s\n", argv[i]);
    }

    return 0;
}
