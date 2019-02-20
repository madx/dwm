/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 16;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Droid Sans Mono Slashed for Powerline:size=10" };
static const char dmenufont[]       = "Droid Sans Mono Slashed for Powerline:size=10";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { "#c5c8c6", "#1d1f21", "#282a2e" },
	[SchemeSel]  = { "#ffffff", "#19b59f", "#19b59f" },
	// { "#c5c8c6", "#a54242", "#cc6666" },  // urgent (white on red)
	// { "#a54242", "#1d1f21", "#282a2e" },  // red
	// { "#8c9440", "#1d1f21", "#282a2e" },  // green
	// { "#de935f", "#1d1f21", "#282a2e" },  // orange
	// { "#5f819d", "#1d1f21", "#282a2e" },  // blue
	// { "#85678f", "#1d1f21", "#282a2e" },  // purple
	// { "#5e8d87", "#1d1f21", "#282a2e" },  // cyan
};

/* tagging */
static const char *tags[] = { "➊", "➋", "➌", "➍", "➎", "➏", "➐", "➑", "➒" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gcolor3",  NULL,       NULL,       0,            1,           0 },
	{ "Yad",      NULL,       NULL,       0,            1,           0 },
};

/* layout(s) */
static const float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "T",      tile },    /* first entry is default */
	{ "F",      NULL },    /* no layout function means floating behavior */
	{ "M",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
  { MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "idontusedmenu", NULL };
static const char *runcmd[]  = { "app-runner", NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *castcmd[]  = { "screencast", NULL };
static const char *shotcmd[]  = { "screenshot", NULL };
static const char *reloadstatuscmd[] = { "dwmstatus", "restart", NULL };
static const char *volumeupcmd[] = { "de", "volume+", NULL };
static const char *volumedowncmd[] = { "de", "volume-", NULL };
static const char *togglemute[] = { "de", "mute", NULL };
static const char *brightnessupcmd[] = { "de", "brightness+", NULL };
static const char *brightnessdowncmd[] = { "de", "brightness-", NULL };
static const char *decmd[] = { "de", NULL };

static Key keys[] = {
	/* modifier                     key            function        argument */
	{ MODKEY,                       XK_r,          spawn,          {.v = runcmd } },
	// { MODKEY|ShiftMask,             XK_r,          self_restart,   {0} },
	{ MODKEY,                       XK_e,          spawn,          {.v = decmd } },
	{ MODKEY,                       XK_s,          spawn,          {.v = shotcmd } },
	{ MODKEY|ShiftMask,             XK_s,          spawn,          {.v = castcmd } },
	{ MODKEY,                       XK_space,      spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_d,          spawn,          {.v = reloadstatuscmd } },
	{ MODKEY,                       XK_b,          togglebar,      {0} },
	{ MODKEY,                       XK_j,          focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,          focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,          incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,          incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,          setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,          setmfact,       {.f = +0.05} },
	// { MODKEY|ShiftMask,             XK_j,          movestack,      {.i = +1 } },
	// { MODKEY|ShiftMask,             XK_k,          movestack,      {.i = -1 } },
	{ MODKEY,                       XK_Return,     zoom,           {0} },
	{ MODKEY,                       XK_Tab,        view,           {0} },
	{ MODKEY,                       XK_w,          killclient,     {0} },
	{ MODKEY,                       XK_t,          setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_t,          setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_f,          setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,          setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,             XK_Return,     setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_f,          togglefloating, {0} },
	{ MODKEY,                       XK_0,          view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,          tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,      focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_semicolon,  focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,      tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_semicolon,  tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_ampersand,                  0)
	TAGKEYS(                        XK_eacute,                     1)
	TAGKEYS(                        XK_quotedbl,                   2)
	TAGKEYS(                        XK_apostrophe,                 3)
	TAGKEYS(                        XK_parenleft,                  4)
	TAGKEYS(                        XK_minus,                      5)
	TAGKEYS(                        XK_egrave,                     6)
	TAGKEYS(                        XK_underscore,                 7)
	TAGKEYS(                        XK_ccedilla,                   8)
	{ MODKEY|ShiftMask,             XK_q,          quit,           {0} },
	{ 0,                            XF86XK_AudioMute, spawn,          {.v = togglemute } },
	{ 0,                            XF86XK_AudioRaiseVolume,  spawn,  {.v = volumeupcmd } },
	{ 0,                            XF86XK_AudioLowerVolume,  spawn,  {.v = volumedowncmd } },
	{ 0,                            XF86XK_MonBrightnessUp,   spawn,  {.v = brightnessupcmd } },
	{ 0,                            XF86XK_MonBrightnessDown, spawn,  {.v = brightnessdowncmd } },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

