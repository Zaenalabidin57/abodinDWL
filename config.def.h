/* Taken from https://github.com/djpohly/dwl/issues/466 */

#define COLOR(hex)    { ((hex >> 24) & 0xFF) / 255.0f, \
                        ((hex >> 16) & 0xFF) / 255.0f, \
                        ((hex >> 8) & 0xFF) / 255.0f, \
                        (hex & 0xFF) / 255.0f }
/* appearance */
static const int sloppyfocus               = 1;  /* focus follows mouse */
static const int bypass_surface_visibility = 0;  /* 1 means idle inhibitors will disable idle tracking even if it's surface isn't visible  */
static const unsigned int borderpx         = 3;  /* border pixel of windows */
static const unsigned int systrayspacing   = 5; /* systray spacing */
static const int showsystray               = 1; /* 0 means no systray */
static const int showbar                   = 1; /* 0 means no bar */
static const int topbar                    = 0; /* 0 means bottom bar */
static const char *fonts[]                 = {"Monocraft Nerd Font:style:Light:size=12"};
static const float rootcolor[]             = COLOR(0x0009090E);
/* This conforms to the xdg-protocol. Set the alpha to zero to restore the old behavior */
static const float fullscreen_bg[]         = {0.1f, 0.1f, 0.1f, 1.0f}; /* You can also use glsl colors */
static uint32_t colors[][3]                = {
	/*               fg          bg          border    */
	/*[SchemeNorm] = { 0x6e738d96, 0xcad3f5ff, 0x444444ff },*/
	[SchemeNorm] = { 0xf8f8f2ff, 0x1e1d2dff, 0x0c161fff },
	[SchemeSel]  = { 0xf8f8f2ff, 0x202f61ff, 0x2b15d4ff },
	//[SchemeSel]  = { 0x282737ff, 0x202f61ff, 0x005577ff },
	[SchemeUrg]  = { 0,          0,          0x770000ff },
};
// aku nak sikit aksi

/* tagging - TAGCOUNT must be no greater than 31 */
#define  TAGCOUNT (5)
static char *tags[] = { "1", "2", "3", "4", "5",};

/* logging */
static int log_level = WLR_ERROR;

// setup env dsw

static const Env envs[] = {
  {"XDG_CURRENT_DESKTOP", "wlroots"},
  {"QT_QPA_PLATFORM", "wayland"},
  {"SDL_VIDEODRIVER", "wayland"},
  {"XDG_SESSION_DESKTOP", "wlroots"},
};

/* Autostart */
static const char *const autostart[] = {
        "sh", "-c" , "swaybg -i ~/Pictures/wollpeper/shiguWollupeper.png -m fill", NULL,
        "sh", "-c", "/usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1", NULL,
        //"sh", "-c", "/usr/bin/lxpolkit", NULL,
        "sh", "-c" , "mako", NULL,
        "sh", "-c" , "wl-paste --watch cliphist store", NULL,
        "sh", "-c" , "/usr/bin/kdeconnectd", NULL,
        "sh", "-c" , "/usr/bin/kdeconnect-indicator", NULL,
        "sh", "-c" , "/home/shigure/.config/scripts/abodindwl/wlranjeng.sh", NULL,
        "sh", "-c" , "/home/shigure/.config/scripts/abodindwl/turu.sh", NULL,
        NULL /* terminate */
};


/* NOTE: ALWAYS keep a rule declared even if you don't use rules (e.g leave at least one example) */
static const Rule rules[] = {
	/* app_id             title       tags mask     isfloating   monitor */
	/* examples: */
  { "Gimp",            NULL,       0,                       1,           -1 },
  { "Firefox",         NULL,       1 << 8,                  0,           -1 },
  { "Firefox",         "Picture-in-Picture",       1 << 8,                 1,           -1 },
  { "eww",             NULL,       0,                       1,           -1 },
  { "pavucontrol",             NULL,       0,                       1,           -1 },
  { "feh",             NULL,       0,                       1,           -1 },
  { "imv",             NULL,       0,                       1,           -1 },
  { "imv-dir",             NULL,       0,                       1,           -1 },
  { "solanum",             NULL,       0,                       1,           -1 },
  { "Nitrogen",             NULL,       0,                       1,           -1 },
  { "YouTube Music",             NULL,       0,                      1,           -1 },
};

/* layout(s) */
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[]=",      tile },
	{ "[M]",      monocle },
};

/* monitors */
/* (x=-1, y=-1) is reserved as an "autoconfigure" monitor position indicator
 * WARNING: negative values other than (-1, -1) cause problems with Xwayland clients
 * https://gitlab.freedesktop.org/xorg/xserver/-/issues/899
*/
/* NOTE: ALWAYS add a fallback rule, even if you are completely sure it won't be used */
static const MonitorRule monrules[] = {
	/* name       mfact  nmaster scale layout       rotate/reflect                x    y */
	{ "eDP-1",    0.5f,  1,      1,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   -1,  -1 },
	{ "HDMI-A-1",    0.5f,  1,      0.6f,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   -1, -1  },
};

/* keyboard */
static const struct xkb_rule_names xkb_rules = {
	/* can specify fields: rules, model, layout, variant, options */
	/* example:
	.options = "ctrl:nocaps",
	*/
	//.options = NULL,
	.options = "caps:escape",
};

static const int repeat_rate = 25;
static const int repeat_delay = 200;

/* Trackpad */
static const int tap_to_click = 1;
static const int tap_and_drag = 1;
static const int drag_lock = 1;
static const int natural_scrolling = 0;
static const int disable_while_typing = 1;
static const int left_handed = 0;
static const int middle_button_emulation = 0;

static const char *upvol[]   = { "/usr/bin/pactl", "set-sink-volume", "0", "+5%",     NULL };
static const char *downvol[] = { "/usr/bin/pactl", "set-sink-volume", "0", "-5%",     NULL };
static const char *mutevol[] = { "/usr/bin/pactl", "set-sink-mute",   "0", "toggle",  NULL };
static const char *light_up[] = {"/usr/bin/brightnessctl", "s", "20+", NULL};
static const char *light_down[] = {"/usr/bin/brightnessctl", "s", "20-", NULL};
static const char *Ppause[] = {"/usr/bin/playerctl", "play-pause", NULL};
static const char *Pplay[] = {"/usr/bin/playerctl", "play-pause", NULL};
static const char *audionext[] = {"/usr/bin/playerctl", "next", NULL};
static const char *audioprev[] = {"/usr/bin/playerctl", "previous", NULL};

/* You can choose between:
LIBINPUT_CONFIG_SCROLL_NO_SCROLL
LIBINPUT_CONFIG_SCROLL_2FG
LIBINPUT_CONFIG_SCROLL_EDGE
LIBINPUT_CONFIG_SCROLL_ON_BUTTON_DOWN
*/
static const enum libinput_config_scroll_method scroll_method = LIBINPUT_CONFIG_SCROLL_2FG;

/* You can choose between:
LIBINPUT_CONFIG_CLICK_METHOD_NONE
LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS
LIBINPUT_CONFIG_CLICK_METHOD_CLICKFINGER
*/
static const enum libinput_config_click_method click_method = LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS;

/* You can choose between:
LIBINPUT_CONFIG_SEND_EVENTS_ENABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED_ON_EXTERNAL_MOUSE
*/
static const uint32_t send_events_mode = LIBINPUT_CONFIG_SEND_EVENTS_ENABLED;

/* You can choose between:
LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT
LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE
*/
static const enum libinput_config_accel_profile accel_profile = LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE;
static const double accel_speed = 0.0;

/* You can choose between:
LIBINPUT_CONFIG_TAP_MAP_LRM -- 1/2/3 finger tap maps to left/right/middle
LIBINPUT_CONFIG_TAP_MAP_LMR -- 1/2/3 finger tap maps to left/middle/right
*/
static const enum libinput_config_tap_button_map button_map = LIBINPUT_CONFIG_TAP_MAP_LRM;

/* If you want to use the windows key for MODKEY, use WLR_MODIFIER_LOGO */
#define MODKEY WLR_MODIFIER_LOGO

#define TAGKEYS(KEY,SKEY,TAG) \
	{ MODKEY,                    KEY,            view,            {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL,  KEY,            toggleview,      {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_SHIFT, SKEY,           tag,             {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL|WLR_MODIFIER_SHIFT,SKEY,toggletag, {.ui = 1 << TAG} }

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
//static const char *termcmd[] = { "foot", NULL };
static const char *termcmd[] = { "ghostty", NULL };
static const char *skrinsut[] = {"sh", "-c", "hyprshot -m region -z -o ~/Pictures/screenshot | wl-copy -p -t image/png", NULL};
static const char *dmenucmd[] = { "fuzzel", "--dmenu", NULL };

static const Key keys[] = {
	/* Note that Shift changes certain key codes: c -> C, 2 -> at, etc. */
	/* modifier                  key                 function        argument */

    {0,                       XKB_KEY_XF86AudioLowerVolume, spawn, {.v = downvol}},
	{0,                       XKB_KEY_XF86AudioMute, spawn, {.v = mutevol }},
	{0,                       XKB_KEY_XF86AudioRaiseVolume, spawn, {.v = upvol}},
  {0,                       XKB_KEY_XF86AudioPlay, spawn, {.v = Pplay}},
  {0,                       XKB_KEY_XF86AudioPause, spawn, {.v = Ppause}},
  {0,                       XKB_KEY_XF86AudioNext, spawn, {.v = audionext}},
  {0,                       XKB_KEY_XF86AudioPrev, spawn, {.v = audioprev}},
	{0,				XKB_KEY_XF86MonBrightnessUp,		spawn,	{.v = light_up}},
	{0,				XKB_KEY_XF86MonBrightnessDown,	spawn,	{.v = light_down}},

	//{ MODKEY,                    XKB_KEY_d,          spawn,          {.v = menucmd} },
	{ MODKEY,                    XKB_KEY_d,          spawn,          SHCMD("fuzzel")},
	{ MODKEY, XKB_KEY_Return,     spawn,          {.v = termcmd} },
	{ MODKEY, XKB_KEY_p,     togglebar,          {0} },
	{ MODKEY, XKB_KEY_semicolon,     togglebar,          {0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_P,     spawn,          SHCMD("swaylock -f -c 000000")},
	{ MODKEY,                    XKB_KEY_j,          focusstack,     {.i = +1} },
	{ MODKEY,                    XKB_KEY_k,          focusstack,     {.i = -1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,                    XKB_KEY_J,          movestack,     {.i = +1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,                    XKB_KEY_K,          movestack,     {.i = -1} },
      { MODKEY,                           XKB_KEY_o,  spawn,            SHCMD("zen-browser")},
    { MODKEY,                           XKB_KEY_n,  spawn,            SHCMD("thunar")},
    { 0,                          XKB_KEY_Print ,  spawn,            {.v = skrinsut}},
    { MODKEY|WLR_MODIFIER_SHIFT,                           XKB_KEY_E,  spawn,            SHCMD("foot /home/shigure/exit.sh")},
    { MODKEY,                           XKB_KEY_y,  spawn,            SHCMD("cliphist list | rofi -dmenu | cliphist decode | wl-copy")},
    { MODKEY|WLR_MODIFIER_SHIFT,                           XKB_KEY_W,  spawn,            SHCMD("rofi -modi emoji -show emoji")},
	{ MODKEY,                    XKB_KEY_i,          incnmaster,     {.i = +1} },
	{ MODKEY,                    XKB_KEY_d,          incnmaster,     {.i = -1} },
	{ MODKEY,                    XKB_KEY_h,          setmfact,       {.f = -0.05f} },
	{ MODKEY,                    XKB_KEY_l,          setmfact,       {.f = +0.05f} },
	{ MODKEY,                    XKB_KEY_Return,     zoom,           {0} },
	{ MODKEY,                    XKB_KEY_Tab,        view,           {0} },
	{ MODKEY, XKB_KEY_q,          killclient,     {0} },
	{ MODKEY|WLR_MODIFIER_SHIFT,                    XKB_KEY_T,          setlayout,      {.v = &layouts[0]} },
	{ MODKEY|WLR_MODIFIER_SHIFT,                    XKB_KEY_F,          setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                    XKB_KEY_m,          setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                    XKB_KEY_space,      setlayout,      {0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_space,      togglefloating, {0} },
	{ MODKEY,                    XKB_KEY_f,         togglefullscreen, {0} },
	{ MODKEY,                    XKB_KEY_0,          view,           {.ui = ~0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_parenright, tag,            {.ui = ~0} },
	{ MODKEY,                    XKB_KEY_comma,      focusmon,       {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY,                    XKB_KEY_period,     focusmon,       {.i = WLR_DIRECTION_RIGHT} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_less,       tagmon,         {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_greater,    tagmon,         {.i = WLR_DIRECTION_RIGHT} },
	TAGKEYS(          XKB_KEY_z, XKB_KEY_Z,                     0),
	TAGKEYS(          XKB_KEY_x, XKB_KEY_X,                         1),
	TAGKEYS(          XKB_KEY_c, XKB_KEY_C,                 2),
	TAGKEYS(          XKB_KEY_v, XKB_KEY_V,                     3),
	TAGKEYS(          XKB_KEY_b, XKB_KEY_B,                    4),
	TAGKEYS(          XKB_KEY_6, XKB_KEY_asciicircum,                5),
	TAGKEYS(          XKB_KEY_7, XKB_KEY_ampersand,                  6),
	TAGKEYS(          XKB_KEY_8, XKB_KEY_asterisk,                   7),
	TAGKEYS(          XKB_KEY_9, XKB_KEY_parenleft,                  8),
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_Q,          quit,           {0} },

	/* Ctrl-Alt-Backspace and Ctrl-Alt-Fx used to be handled by X server */
	{ WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_Terminate_Server, quit, {0} },
	/* Ctrl-Alt-Fx is used to switch to another VT, if you don't know what a VT is
	 * do not remove them.
	 */
#define CHVT(n) { WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_XF86Switch_VT_##n, chvt, {.ui = (n)} }
	CHVT(1), CHVT(2), CHVT(3), CHVT(4), CHVT(5), CHVT(6),
	CHVT(7), CHVT(8), CHVT(9), CHVT(10), CHVT(11), CHVT(12),
};

static const Button buttons[] = {
	{ ClkLtSymbol, 0,      BTN_LEFT,   setlayout,      {.v = &layouts[0]} },
	{ ClkLtSymbol, 0,      BTN_RIGHT,  setlayout,      {.v = &layouts[1]} },
	{ ClkTitle,    0,      BTN_MIDDLE, zoom,           {0} },
	{ ClkStatus,   0,      BTN_MIDDLE, spawn,          {.v = termcmd} },
	{ ClkClient,   MODKEY, BTN_LEFT,   moveresize,     {.ui = CurMove} },
	{ ClkClient,   MODKEY, BTN_MIDDLE, togglefloating, {0} },
	{ ClkClient,   MODKEY, BTN_RIGHT,  moveresize,     {.ui = CurResize} },
	{ ClkTagBar,   0,      BTN_LEFT,   view,           {0} },
	{ ClkTagBar,   0,      BTN_RIGHT,  toggleview,     {0} },
	{ ClkTagBar,   MODKEY, BTN_LEFT,   tag,            {0} },
	{ ClkTagBar,   MODKEY, BTN_RIGHT,  toggletag,      {0} },
	{ ClkTray,     0,      BTN_LEFT,   trayactivate,   {0} },
	{ ClkTray,     0,      BTN_RIGHT,  traymenu,       {0} },
};
