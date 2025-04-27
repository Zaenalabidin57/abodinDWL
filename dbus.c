#include "dbus.h"

#include <dbus/dbus.h>
#include <wayland-server-core.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#if defined __linux__
#include <sys/eventfd.h>
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
#include <sys/event.h>
#endif
#include <unistd.h>

int efd = -1;

static int
dwl_dbus_dispatch(int fd, unsigned int mask, void *data)
{
	DBusConnection *conn = data;

	uint64_t dispatch_pending;
	DBusDispatchStatus status;

	status = dbus_connection_dispatch(conn);

	/*
	 * Don't clear pending flag if message queue wasn't
	 * fully drained
	 */
	if (status != DBUS_DISPATCH_COMPLETE)
		return 0;

	if (read(fd, &dispatch_pending, sizeof(uint64_t)) < 0)
		perror("read");

	return 0;
}

static int
dwl_dbus_watch_handle(int fd, uint32_t mask, void *data)
{
	DBusWatch *watch = data;

	uint32_t flags = 0;

	if (!dbus_watch_get_enabled(watch))
		return 0;

	if (mask & WL_EVENT_READABLE)
		flags |= DBUS_WATCH_READABLE;
	if (mask & WL_EVENT_WRITABLE)
		flags |= DBUS_WATCH_WRITABLE;
	if (mask & WL_EVENT_HANGUP)
		flags |= DBUS_WATCH_HANGUP;
	if (mask & WL_EVENT_ERROR)
		flags |= DBUS_WATCH_ERROR;

	dbus_watch_handle(watch, flags);

	return 0;
}

static dbus_bool_t
dwl_dbus_add_watch(DBusWatch *watch, void *data)
{
	struct wl_event_loop *loop = data;

	int fd;
	struct wl_event_source *watch_source;
	uint32_t mask = 0, flags;

	if (!dbus_watch_get_enabled(watch))
		return TRUE;

	flags = dbus_watch_get_flags(watch);
	if (flags & DBUS_WATCH_READABLE)
		mask |= WL_EVENT_READABLE;
	if (flags & DBUS_WATCH_WRITABLE)
		mask |= WL_EVENT_WRITABLE;

	fd = dbus_watch_get_unix_fd(watch);
	watch_source = wl_event_loop_add_fd(loop, fd, mask,
	                                    dwl_dbus_watch_handle, watch);

	dbus_watch_set_data(watch, watch_source, NULL);

	return TRUE;
}

static void
dwl_dbus_remove_watch(DBusWatch *watch, void *data)
{
	struct wl_event_source *watch_source = dbus_watch_get_data(watch);

	if (watch_source)
		wl_event_source_remove(watch_source);
}

static int
dwl_dbus_timeout_handle(void *data)
{
	DBusTimeout *timeout = data;

	if (dbus_timeout_get_enabled(timeout))
		dbus_timeout_handle(timeout);

	return 0;
}

static dbus_bool_t
dwl_dbus_add_timeout(DBusTimeout *timeout, void *data)
{
	struct wl_event_loop *loop = data;

	int r, interval;
	struct wl_event_source *timeout_source;

	if (!dbus_timeout_get_enabled(timeout))
		return TRUE;

	interval = dbus_timeout_get_interval(timeout);

	timeout_source = wl_event_loop_add_timer(
		loop, dwl_dbus_timeout_handle, timeout);

	r = wl_event_source_timer_update(timeout_source, interval);
	if (r < 0) {
		wl_event_source_remove(timeout_source);
		return FALSE;
	}

	dbus_timeout_set_data(timeout, timeout_source, NULL);

	return TRUE;
}

static void
dwl_dbus_remove_timeout(DBusTimeout *timeout, void *data)
{
	struct wl_event_source *timeout_source;

	timeout_source = dbus_timeout_get_data(timeout);

	if (timeout_source) {
		wl_event_source_timer_update(timeout_source, 0);
		wl_event_source_remove(timeout_source);
	}
}

static void
dwl_dbus_adjust_timeout(DBusTimeout *timeout, void *data)
{
	int interval;
	struct wl_event_source *timeout_source;

	timeout_source = dbus_timeout_get_data(timeout);

	if (timeout_source) {
		interval = dbus_timeout_get_interval(timeout);
		wl_event_source_timer_update(timeout_source, interval);
	}
}

static void
dwl_dbus_dispatch_status(DBusConnection *conn, DBusDispatchStatus status, void *data)
{
	if (status == DBUS_DISPATCH_DATA_REMAINS) {
		uint64_t dispatch_pending = 1;
		if (write(efd, &dispatch_pending, sizeof(uint64_t)) < 0)
			perror("write");
	}
}

struct wl_event_source *
startbus(DBusConnection *conn, struct wl_event_loop *loop)
{
	struct wl_event_source *bus_source = NULL;
	uint64_t dispatch_pending = 1;

	dbus_connection_set_exit_on_disconnect(conn, FALSE);

#if defined __linux__
	efd = eventfd(0, EFD_CLOEXEC);
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
	efd = kqueue();
#endif
	if (efd < 0)
		goto fail;

	dbus_connection_set_dispatch_status_function(conn, dwl_dbus_dispatch_status, NULL, NULL);

	if (!dbus_connection_set_watch_functions(conn, dwl_dbus_add_watch,
	                                         dwl_dbus_remove_watch,
	                                         NULL, loop, NULL)) {
		goto fail;
	}

	if (!dbus_connection_set_timeout_functions(
		    conn, dwl_dbus_add_timeout, dwl_dbus_remove_timeout,
		    dwl_dbus_adjust_timeout, loop, NULL)) {
		goto fail;
	}

	bus_source = wl_event_loop_add_fd(loop, efd, WL_EVENT_READABLE, dwl_dbus_dispatch, conn);
	if (!bus_source)
		goto fail;

	if (dbus_connection_get_dispatch_status(conn) == DBUS_DISPATCH_DATA_REMAINS)
		if (write(efd, &dispatch_pending, sizeof(uint64_t)) < 0)
			perror("write");

	return bus_source;

fail:
	if (bus_source)
		wl_event_source_remove(bus_source);
	if (efd >= 0) {
		close(efd);
		efd = -1;
	}
	dbus_connection_set_timeout_functions(conn, NULL, NULL, NULL, NULL, NULL);
	dbus_connection_set_watch_functions(conn, NULL, NULL, NULL, NULL, NULL);
	dbus_connection_set_dispatch_status_function(conn, NULL, NULL, NULL);

	return NULL;
}

void
stopbus(DBusConnection *conn, struct wl_event_source *bus_source)
{
	wl_event_source_remove(bus_source);
	close(efd);
	efd = -1;

	dbus_connection_set_watch_functions(conn, NULL, NULL, NULL, NULL, NULL);
	dbus_connection_set_timeout_functions(conn, NULL, NULL, NULL, NULL, NULL);
	dbus_connection_set_dispatch_status_function(conn, NULL, NULL, NULL);
}
