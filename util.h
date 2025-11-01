/* See LICENSE.dwm file for copyright and license details. */
typedef struct listens {
    struct wl_listener* listen;
    struct listens* next;
} listens;

void die(const char *fmt, ...);
void *ecalloc(size_t nmemb, size_t size);
int fd_set_nonblock(int fd);
struct listens* append_listener(struct wl_listener* l, struct listens* ls);
struct listens* remove_listener(struct wl_listener* l, struct listens* ls);
