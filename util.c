/* See LICENSE.dwm file for copyright and license details. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "util.h"

void
die(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		fputc('\n', stderr);
	}

	exit(1);
}

void *
ecalloc(size_t nmemb, size_t size)
{
	void *p;

	if (!(p = calloc(nmemb, size)))
		die("calloc:");
	return p;
}

int
fd_set_nonblock(int fd) {
	int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
		perror("fcntl(F_GETFL):");
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
		perror("fcntl(F_SETFL):");
		return -1;
    }

	return 0;
}

struct listens*
append_listener(struct wl_listener* new, struct listens* list)
{
    struct listens* l = malloc(sizeof(struct listens));
    l->listen = new;
    l->next = list;
    return l;
}

struct listens*
remove_listener(struct wl_listener* l, struct listens* ls)
{
    struct listens* out = ls;
    struct listens* f = NULL;
    for(struct listens* last = NULL; ls != NULL; ls = ls->next)
    {
        if (ls->listen == l)
        {
            if (last != NULL)
                last->next = ls->next;
            else
                out = ls->next;

            f = ls;
        }
        else
            last = ls;
    }

    free(f);

    return out;
}
