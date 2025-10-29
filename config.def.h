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
static const int smartgaps                 = 0;  /* 1 means no outer gap when there is only one window */
static int gaps                            = 1;  /* 1 means gaps between windows are added */
static const unsigned int gappx            = 10; /* gap pixel between windows */
static const int showsystray               = 1; /* 0 means no systray */
static const int showbar                   = 1; /* 0 means no bar */
static const int topbar                    = 0; /* 0 means bottom bar */
static const int vertpad                   = 5; /* vertical padding of bar */
static const int follow                     = 1;
static const int sidepad                   = 10; /* horizontal padding of bar */
static const int user_bh		   = 30; /* 0 means that dwl will calculate barheight, >= 1 means dwl will use user_bh as the bar height. */
static const char *fonts[]                 = {"Monocraft Nerd Font:style:Light:size=11"};
static const float rootcolor[]             = COLOR(0x0009090E);
/* This conforms to the xdg-protocol. Set the alpha to zero to restore the old behavior */
static const float fullscreen_bg[]         = {0.1f, 0.1f, 0.1f, 1.0f}; /* You can also use glsl colors */
static int enableautoswallow = 1; /* enables autoswallowing newly spawned clients */
static float swallowborder = 1.0f; /* add this multiplied by borderpx to border when a client is swallowed */
static uint32_t colors[][3]                = {
	/*               fg          bg          border    */
	/*[SchemeNorm] = { 0x6e738d96, 0xcad3f5ff, 0x444444ff },*/
	[SchemeNorm] = { 0xf8f8f2ff, 0x1e1d2dff, 0x0c161fff },
	[SchemeSel]  = { 0xf8f8f2ff, 0x202f61ff, 0x2b15d4ff },
	//[SchemeSel]  = { 0x282737ff, 0x202f61ff, 0x005577ff },
	[SchemeUrg]  = { 0,          0,          0x770000ff },
};
// aku nak sikit aksi

/* Max amount of dynamically added rules */
#define RULES_MAX 10

static const unsigned int swipe_min_threshold = 0;

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
  {"_JAVA_AWT_WM_NONREPARENTING", "1"},
};

/* Autostart */
static const char *const autostart[] = {
        "sh", "-c" , "swaybg -i ~/Pictures/wollpeper/ogata_rina.jpg -m fill", NULL,
        "sh", "-c", "/usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1", NULL,
        "sh", "-c", "gnome-keyring-daemon --start --components=gpg,pkcs11,secrets,ssh", NULL,
        //"sh", "-c", "/usr/bin/lxpolkit", NULL,
        "sh", "-c" , "dunst", NULL,
        "sh", "-c" , "wl-paste --watch cliphist store", NULL,
        "sh", "-c" , "/usr/bin/kdeconnectd", NULL,
        "sh", "-c" , "/usr/bin/kdeconnect-indicator", NULL,
        //"sh", "-c" , "/home/shigure/.config/scripts/abodindwl/wlranjeng.sh", NULL,
        //"sh", "-c" , "/home/shigure/.config/scripts/abodindwl/turu.sh", NULL,
        //"sh", "-c" , "foot -s", NULL,
        "sh", "-c" , "wayland-pipewire-idle-inhibit", NULL,
        "sh", "-c" , "/usr/lib/xdg-desktop-portal -r", NULL,
        "sh", "-c" , "/usr/lib/xdg-desktop-portal-wlr -r", NULL,
       // "sh", "-c" , "xwayland-satellite", NULL,
        NULL /* terminate */
};


static const Menu menus[] = {
	/* command                            feed function        action function */
	{ "bemenu -i -l 10 -p Windows",        menuwinfeed,         menuwinaction    },
	{ "bemenu -i -p Layouts",              menulayoutfeed,      menulayoutaction },
	{ "bemenu -i -l 10  -p Rules",  menurulefeed,        menuruleaction },
};


/* NOTE: ALWAYS keep a rule declared even if you don't use rules (e.g leave at least one example) */
static const Rule rules[] = {
	/* app_id             title       tags mask     isfloating   isterm   noswallow   monitor */
	/* examples: */
  { "foot",            NULL,       0,                       0,         1,        1,           -1 },
  { "prostat",            NULL,       0,                       1,         1,        1,           -1 },
  { "ghostty",            NULL,       0,                       0,         1,        1,           -1 },
  { "Thunar",            NULL,       0,                       0,         1,        1,           -1 },
  { "Gimp",            NULL,       0,                       1,         0,        0,           -1 },
  { "Firefox",         NULL,       1 << 8,                  0,         0,        0,           -1 },
  { "Firefox",         "Picture-in-Picture",       1 << 8,                 1,   0,        0,           -1 },
  { "eww",             NULL,       0,                       1,         0,        0,           -1 },
  { "pavucontrol",     NULL,       0,                       1,         0,        0,           -1 },
  { "feh",             NULL,       0,                       1,         0,        0,           -1 },
  { "imv",             NULL,       0,                       0,         0,        0,           -1 },
  { "imv-dir",         NULL,       0,                       0,         0,        0,           -1 },
  { "solanum",         NULL,       0,                       1,         0,        0,           -1 },
  { "Nitrogen",        NULL,       0,                       1,         0,        0,           -1 },
  { "YouTube Music",   NULL,       0,                       1,         0,        0,           -1 },
};

/* layout(s) */
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },
	{ "><>",      NULL },    /* no layout function means floating behavior */
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
static const int natural_scrolling = 1;
static const int disable_while_typing = 1;
static const int left_handed = 0;
static const int middle_button_emulation = 0;

static const char *upvol[]   = { "/usr/bin/pactl", "set-sink-volume", "0", "+5%",     NULL };
static const char *downvol[] = { "/usr/bin/pactl", "set-sink-volume", "0", "-5%",     NULL };
static const char *mutevol[] = { "/usr/bin/pactl", "set-sink-mute",   "0", "toggle",  NULL };
static const char *light_up[] = {"/usr/bin/brightnessctl", "s", "+5%", NULL};
static const char *light_down[] = {"/usr/bin/brightnessctl", "s", "5-%", NULL};
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

static const int cursor_timeout = 5;

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
//static const char *termcmd[] = { "ghostty", NULL };
static const char *termcmd[] = { "ghostty", NULL };
static const char *skrinsut[] = {"sh", "-c", "/home/shigure/.config/labwc/skinshut.sh", NULL};
static const char *dmenucmd[] = {"sh", "-c", "rofi -dmenu", NULL };

#include "shiftview.c"

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
	{ MODKEY,                    XKB_KEY_d,          spawn,          SHCMD("rofi -show drun")},
	{ MODKEY,                    XKB_KEY_t,          spawn,          SHCMD("todo")},
	{ MODKEY, XKB_KEY_Return,     spawn,          {.v = termcmd} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_Return,     spawn,          SHCMD("foot --app-id=prostat") },
	{ MODKEY, XKB_KEY_p,     togglebar,          {0} },
	{ MODKEY, XKB_KEY_semicolon,     togglebar,          {0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_P,     spawn,          SHCMD("swaylock --screenshots --clock --indicator --indicator-radius 100 --indicator-thickness 7 --effect-blur 7x5 --effect-vignette 0.5:0.5 --ring-color bb00cc --key-hl-color 880033 --line-color 00000000 --inside-color 00000088 --separator-color 00000000 --grace 2 --fade-in 0.2")},
	{ MODKEY,                    XKB_KEY_j,          focusstack,     {.i = +1} },
	{ MODKEY,                    XKB_KEY_k,          focusstack,     {.i = -1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,                    XKB_KEY_J,          movestack,     {.i = +1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,                    XKB_KEY_K,          movestack,     {.i = -1} },
  { MODKEY,                           XKB_KEY_o,  spawn,            SHCMD("qutebrowser")},
  { MODKEY,                           XKB_KEY_n,  spawn,            SHCMD("ghostty -e yazi")},
  { MODKEY|WLR_MODIFIER_SHIFT,                           XKB_KEY_N,  spawn,            SHCMD("thunar")},
  { 0,                          XKB_KEY_Print,  spawn,            SHCMD("skinsut")},
  { MODKEY|WLR_MODIFIER_SHIFT,                           XKB_KEY_E,  spawn,            SHCMD("ghostty -e /home/shigure/exit.sh")},
  { MODKEY,                           XKB_KEY_y,  spawn,            SHCMD("cliphist list | rofi -dmenu | cliphist decode | wl-copy")},
  { MODKEY,                           XKB_KEY_w,  spawn,            SHCMD("rofi -modi emoji -show emoji")},
  { MODKEY|WLR_MODIFIER_SHIFT,                           XKB_KEY_W,  menu,            {.v = &menus[2]}},
	{ MODKEY,                    XKB_KEY_i,          incnmaster,     {.i = +1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,                    XKB_KEY_I,          incnmaster,     {.i = -1} },
	{ MODKEY,                    XKB_KEY_h,          setmfact,       {.f = -0.05f} },
	{ MODKEY,                    XKB_KEY_l,          setmfact,       {.f = +0.05f} },
	{ MODKEY,                    XKB_KEY_f,     zoom,           {0} },
	{ MODKEY,                    XKB_KEY_Tab,        view,           {0} },
	{ WLR_MODIFIER_ALT,                    XKB_KEY_Tab,        menu,           {.v = &menus[0]} },
	//{ WLR_MODIFIER_CTRL,                    XKB_KEY_Tab,        focusstack,           {.i = 1 }},
	//{ WLR_MODIFIER_CTRL|WLR_MODIFIER_SHIFT,                    XKB_KEY_Tab,        focusstack,           {.i = -1 }},
	{ MODKEY,                    XKB_KEY_g,          togglegaps,     {0} },
	{ MODKEY, XKB_KEY_q,          killclient,     {0} },
	//{ MODKEY|WLR_MODIFIER_SHIFT,                    XKB_KEY_T,          setlayout,      {.v = &layouts[0]} },
	//{ MODKEY|WLR_MODIFIER_SHIFT,                    XKB_KEY_F,          setlayout,      {.v = &layouts[1]} },
	//{ MODKEY,                    XKB_KEY_m,          setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                    XKB_KEY_space,      setlayout,      {0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_S,          addscratchpad,    {0} },
	{ MODKEY|WLR_MODIFIER_CTRL,  XKB_KEY_s,         removescratchpad, {0} },
  { MODKEY,                    XKB_KEY_s,        togglescratchpad, {0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_space,      togglefloating, {0} },
	{ MODKEY|WLR_MODIFIER_SHIFT,                    XKB_KEY_F,         togglefullscreen, {0} },
	{ MODKEY,                    XKB_KEY_a,          toggleswallow,  {0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_A,          toggleautoswallow, {0} },
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

static const Gesture gestures[] = {
	//{ 0, SWIPE_LEFT, 4, shiftview, { .i = 1 } },
	//{ 0, SWIPE_RIGHT, 4, shiftview, { .i = -1 } },
	{ 0, SWIPE_UP, 4, shiftview, { .i = -1 } },
	{ 0, SWIPE_DOWN, 4, shiftview, { .i = +1 } },
	{ 0, SWIPE_LEFT, 4, movestack, { .i = -1 } },
	{ 0, SWIPE_RIGHT, 4, movestack, { .i = +1 } },
	{ 0, SWIPE_UP, 3, focusstack, {.i = -1} },
	{ 0, SWIPE_DOWN, 3, focusstack, {.i = +1} },
	{ 0, SWIPE_LEFT, 3, zoom, {0} },
	{ 0, SWIPE_RIGHT, 3, zoom, {0} },
};
