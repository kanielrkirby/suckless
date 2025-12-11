/* See LICENSE file for copyright and license details. */
// #include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 120;  /* refresh rate (per second) for client move/resize */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define ALT Mod1Mask
#define SUPER Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ SUPER,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ SUPER|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ SUPER|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ SUPER|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "/home/mx/.local/bin/menu_custom", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };

/* brightness control function */
void
brightness(const Arg *arg) {
	char step[8];
	int direction = arg->i >> 8;
	int value = arg->i & 0xFF;
	
	snprintf(step, sizeof(step), "%d%%%c", value, direction ? '+' : '-');
	
	const char *brightcmd[] = { "brightnessctl", "set", step, NULL };
	Arg a = {.v = brightcmd};
	spawn(&a);
}

/* volume control function */
void
volume(const Arg *arg) {
	char step[8];
	int action = arg->i >> 8;
	int value = arg->i & 0xFF;
	
	if (action == 2) {
		const char *mutecmd[] = { "pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL };
		Arg a = {.v = mutecmd};
		spawn(&a);
	} else {
		snprintf(step, sizeof(step), "%s%d%%", action ? "+" : "-", value);
		const char *volcmd[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", step, NULL };
		Arg a = {.v = volcmd};
		spawn(&a);
	}
}

static const Key keys[] = {
	/* modifier                    key             function        argument */
	{ SUPER,                       XK_p,           spawn,          {.v = dmenucmd } },
	{ SUPER|ShiftMask,             XK_Return,      spawn,          {.v = termcmd } },
	{ SUPER,                       XK_b,           togglebar,      {0} },
	{ SUPER,                       XK_j,           focusstack,     {.i = +1 } },
	{ SUPER,                       XK_k,           focusstack,     {.i = -1 } },
	{ SUPER,                       XK_i,           incnmaster,     {.i = +1 } },
	{ SUPER,                       XK_d,           incnmaster,     {.i = -1 } },
	{ SUPER,                       XK_h,           setmfact,       {.f = -0.05} },
	{ SUPER,                       XK_l,           setmfact,       {.f = +0.05} },
	{ SUPER,                       XK_Return,      zoom,           {0} },
	{ SUPER,                       XK_Tab,         view,           {0} },
	{ SUPER|ShiftMask,             XK_c,           killclient,     {0} },
	{ SUPER,                       XK_t,           setlayout,      {.v = &layouts[0]} },
	{ SUPER,                       XK_f,           setlayout,      {.v = &layouts[1]} },
	{ SUPER,                       XK_m,           setlayout,      {.v = &layouts[2]} },
	{ SUPER,                       XK_apostrophe,  setlayout,      {0} },
	{ SUPER|ShiftMask,             XK_apostrophe,  togglefloating, {0} },
	{ SUPER,                       XK_0,           view,           {.ui = ~0 } },
	{ SUPER|ShiftMask,             XK_0,           tag,            {.ui = ~0 } },
	{ SUPER,                       XK_comma,       focusmon,       {.i = -1 } },
	{ SUPER,                       XK_period,      focusmon,       {.i = +1 } },
	{ SUPER|ShiftMask,             XK_comma,       tagmon,         {.i = -1 } },
	{ SUPER|ShiftMask,             XK_period,      tagmon,         {.i = +1 } },
	TAGKEYS(                       XK_1,                           0)
	TAGKEYS(                       XK_2,                           1)
	TAGKEYS(                       XK_3,                           2)
	TAGKEYS(                       XK_4,                           3)
	TAGKEYS(                       XK_5,                           4)
	TAGKEYS(                       XK_6,                           5)
	TAGKEYS(                       XK_7,                           6)
	TAGKEYS(                       XK_8,                           7)
	TAGKEYS(                       XK_9,                           8)
	{ SUPER|ShiftMask,             XK_q,           quit,           {0} },
	// { 0,                           XF86XK_MonBrightnessUp,         brightness,              {.i = (1 << 8) | 2 } },
	// { 0,                           XF86XK_MonBrightnessDown,       brightness,              {.i = (0 << 8) | 2 } },
	// { SUPER,                       XF86XK_MonBrightnessUp,         brightness,              {.i = (1 << 8) | 10 } },
	// { SUPER,                       XF86XK_MonBrightnessDown,       brightness,              {.i = (0 << 8) | 10 } },
	// { 0,                           XF86XK_AudioRaiseVolume,        volume,                  {.i = (1 << 8) | 5 } },
	// { 0,                           XF86XK_AudioLowerVolume,        volume,                  {.i = (0 << 8) | 5 } },
	// { 0,                           XF86XK_AudioMute,               volume,                  {.i = (2 << 8) | 0 } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         SUPER,          Button1,        movemouse,      {0} },
	{ ClkClientWin,         SUPER,          Button2,        togglefloating, {0} },
	{ ClkClientWin,         SUPER,          Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            SUPER,          Button1,        tag,            {0} },
	{ ClkTagBar,            SUPER,          Button3,        toggletag,      {0} },
};

