/*
 * XWayland fixes for dwl - addresses BadWindow errors and race conditions
 * Apply these patches to dwl.c to fix XWayland hanging issues
 */

#include <errno.h>
#include <sys/wait.h>

/* Add these variables to the global section */
static struct wl_event_source *xwayland_health_check_source;
static int xwayland_restarts = 0;
static const int MAX_XWAYLAND_RESTARTS = 3;

/* Improved getatom function with proper error checking */
static xcb_atom_t
getatom_safe(xcb_connection_t *xc, const char *name)
{
	xcb_atom_t atom = 0;
	xcb_intern_atom_reply_t *reply;
	xcb_generic_error_t *error = NULL;
	xcb_intern_atom_cookie_t cookie = xcb_intern_atom(xc, 0, strlen(name), name);
	
	if (!(reply = xcb_intern_atom_reply(xc, cookie, &error))) {
		if (error) {
			wlr_log(WLR_ERROR, "XCB error getting atom %s: error_code=%d", 
			         name, error->error_code);
			free(error);
		} else {
			wlr_log(WLR_ERROR, "Failed to get atom %s: no reply", name);
		}
		return 0;
	}
	
	atom = reply->atom;
	free(reply);
	return atom;
}

/* Enhanced XWayland health monitoring */
static int
check_xwayland_health(void *data)
{
#ifdef XWAYLAND
	if (!xwayland || !xwayland->server) {
		return 1000; // Check again in 1 second
	}
	
	/* Check if XWayland server is still alive */
	int status;
	pid_t result = waitpid(xwayland->server->pid, &status, WNOHANG);
	
	if (result == -1) {
		wlr_log(WLR_ERROR, "XWayland health check failed: %s", strerror(errno));
		return 1000;
	} else if (result == xwayland->server->pid) {
		wlr_log(WLR_ERROR, "XWayland server (PID %d) died, status %d", 
		         xwayland->server->pid, status);
		
		/* Clean up dead XWayland */
		wlr_xwayland_destroy(xwayland);
		xwayland = NULL;
		
		/* Attempt restart if we haven't exceeded limit */
		if (xwayland_restarts < MAX_XWAYLAND_RESTARTS) {
			xwayland_restarts++;
			wlr_log(WLR_INFO, "Attempting XWayland restart %d/%d", 
			         xwayland_restarts, MAX_XWAYLAND_RESTARTS);
			
			/* Schedule restart after short delay */
			wl_event_loop_add_idle(event_loop, restart_xwayland_idle, NULL);
		} else {
			wlr_log(WLR_ERROR, "XWayland restart limit exceeded, disabling XWayland");
		}
		return 1000;
	}
	
	/* Test XCB connection health */
	if (xwayland->display_name) {
		xcb_connection_t *test_xc = xcb_connect(xwayland->display_name, NULL);
		if (xcb_connection_has_error(test_xc)) {
			wlr_log(WLR_ERROR, "XWayland XCB connection test failed");
			xcb_disconnect(test_xc);
			return 1000;
		}
		xcb_disconnect(test_xc);
	}
#endif
	
	return 5000; // Check every 5 seconds when healthy
}

/* Idle handler for XWayland restart */
static void
restart_xwayland_idle(void *data)
{
#ifdef XWAYLAND
	if (!xwayland) {
		setup_xwayland();
	}
#endif
}

/* Enhanced XWayland setup with better error handling */
static void
setup_xwayland(void)
{
#ifdef XWAYLAND
	wlr_log(WLR_INFO, "Setting up XWayland (attempt %d)", xwayland_restarts + 1);
	
	/* Clean up any existing XWayland */
	if (xwayland) {
		wlr_xwayland_destroy(xwayland);
		xwayland = NULL;
	}
	
	/* Create new XWayland instance */
	if (!(xwayland = wlr_xwayland_create(dpy, compositor, 1))) {
		wlr_log(WLR_ERROR, "Failed to create XWayland server");
		return;
	}
	
	/* Set up event listeners */
	LISTEN_STATIC(&xwayland->events.ready, xwaylandready);
	LISTEN_STATIC(&xwayland->events.new_surface, createnotifyx11);
	
	/* Set DISPLAY variable */
	setenv("DISPLAY", xwayland->display_name, 1);
	wlr_log(WLR_INFO, "XWayland display: %s", xwayland->display_name);
	
	/* Start health monitoring */
	if (!xwayland_health_check_source) {
		xwayland_health_check_source = 
			wl_event_loop_add_timer(event_loop, check_xwayland_health, NULL);
	}
	wl_event_source_timer_update(xwayland_health_check_source, 1000); // Start checking in 1s
#endif
}

/* Improved signal handling for XWayland */
void
handlesig_enhanced(int signo)
{
	if (signo == SIGCHLD) {
		siginfo_t in;
		while (!waitid(P_ALL, 0, &in, WEXITED|WNOHANG) && in.si_pid) {
#ifdef XWAYLAND
			/* Handle XWayland server death specially */
			if (xwayland && in.si_pid == xwayland->server->pid) {
				wlr_log(WLR_INFO, "XWayland server (PID %d) exited with status %d", 
				         in.si_pid, in.si_status);
				xwayland = NULL;
				continue;
			}
#endif
			
			/* Handle other child processes */
			pid_t *p, *lim;
			waitpid(in.si_pid, NULL, 0);
			if (in.si_pid == child_pid)
				child_pid = -1;
			if (!(p = autostart_pids))
				continue;
			lim = &p[autostart_len];

			for (; p < lim; p++) {
				if (*p == in.si_pid) {
					*p = -1;
					break;
				}
			}
		}
	} else if (signo == SIGINT || signo == SIGTERM) {
		quit(NULL);
	}
}

/* Enhanced xwaylandready with safer XCB handling */
void
xwaylandready_enhanced(struct wl_listener *listener, void *data)
{
#ifdef XWAYLAND
	struct wlr_xcursor *xcursor;
	xcb_connection_t *xc = NULL;
	int retry_count = 0;
	const int max_retries = 3;
	
	/* Retry XCB connection with backoff */
	while (retry_count < max_retries) {
		xc = xcb_connect(xwayland->display_name, NULL);
		int err = xcb_connection_has_error(xc);
		
		if (!err) {
			break; // Success
		}
		
		wlr_log(WLR_DEBUG, "XCB connection attempt %d failed with code %d, retrying...", 
		         retry_count + 1, err);
		
		if (xc) {
			xcb_disconnect(xc);
			xc = NULL;
		}
		
		/* Exponential backoff */
		usleep((1 << retry_count) * 100000); // 100ms, 200ms, 400ms
		retry_count++;
	}
	
	if (!xc || xcb_connection_has_error(xc)) {
		wlr_log(WLR_ERROR, "Failed to connect to XWayland after %d attempts", max_retries);
		if (xc) xcb_disconnect(xc);
		return;
	}
	
	wlr_log(WLR_INFO, "Successfully connected to XWayland display");

	/* Collect atoms with safe function */
	netatom[NetWMWindowTypeDialog] = getatom_safe(xc, "_NET_WM_WINDOW_TYPE_DIALOG");
	netatom[NetWMWindowTypeSplash] = getatom_safe(xc, "_NET_WM_WINDOW_TYPE_SPLASH");
	netatom[NetWMWindowTypeToolbar] = getatom_safe(xc, "_NET_WM_WINDOW_TYPE_TOOLBAR");
	netatom[NetWMWindowTypeUtility] = getatom_safe(xc, "_NET_WM_WINDOW_TYPE_UTILITY");

	/* Set up seat */
	wlr_xwayland_set_seat(xwayland, seat);

	/* Set cursor with error checking */
	if ((xcursor = wlr_xcursor_manager_get_xcursor(cursor_mgr, "default", 1))) {
		wlr_xwayland_set_cursor(xwayland,
					xcursor->images[0]->buffer, xcursor->images[0]->width * 4,
					xcursor->images[0]->width, xcursor->images[0]->height,
					xcursor->images[0]->hotspot_x, xcursor->images[0]->hotspot_y);
	} else {
		wlr_log(WLR_WARN, "Failed to get default X cursor");
	}

	xcb_disconnect(xc);
	wlr_log(WLR_INFO, "XWayland setup completed successfully");
#endif
}

/* Enhanced sethints with proper validation */
void
sethints_enhanced(struct wl_listener *listener, void *data)
{
	Client *c = wl_container_of(listener, c, set_hints);
	struct wlr_surface *surface = client_surface(c);
	
	if (!c || !c->surface.xwayland) {
		wlr_log(WLR_DEBUG, "sethints called with invalid XWayland client");
		return;
	}
	
	/* Validate that the surface is still valid */
	if (!surface || !surface->mapped) {
		wlr_log(WLR_DEBUG, "sethints called on unmapped surface");
		return;
	}
	
	if (c == focustop(selmon))
		return;

	/* Safely get urgency hint */
	if (c->surface.xwayland->hints) {
		c->isurgent = xcb_icccm_wm_hints_get_urgency(c->surface.xwayland->hints);
	} else {
		c->isurgent = 0;
	}
	
	drawbars();

	if (c->isurgent && surface && surface->mapped) {
		client_set_border_color(c, (float[])COLOR(colors[SchemeUrg][ColBorder]));
	}
}

/* Add these declarations to dwl.c */
static void restart_xwayland_idle(void *data);
static void setup_xwayland(void);
static xcb_atom_t getatom_safe(xcb_connection_t *xc, const char *name);
static int check_xwayland_health(void *data);
static void handlesig_enhanced(int signo);
static void xwaylandready_enhanced(struct wl_listener *listener, void *data);
static void sethints_enhanced(struct wl_listener *listener, void *data);