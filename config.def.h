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
	/*                  fg         bg         border   */
	[SchemeNorm]    = { "#c5c8c6", "#1d1f21", "#282a2e" },
	[SchemeSel]     = { "#ffffff", "#19b59f", "#19b59f" },
	[SchemeUrgent]  = { "#c5c8c6", "#a54242", "#cc6666" },
	[SchemeRed]     = { "#a54242", "#1d1f21", "#282a2e" },
	[SchemeGreen]   = { "#8c9440", "#1d1f21", "#282a2e" },
	[SchemeOrange]  = { "#de935f", "#1d1f21", "#282a2e" },
	[SchemeBlue]    = { "#5f819d", "#1d1f21", "#282a2e" },
	[SchemePurple]  = { "#85678f", "#1d1f21", "#282a2e" },
	[SchemeCyan]    = { "#5e8d87", "#1d1f21", "#282a2e" },
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
	{ "B",      bstack },
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
	/* modifier                     key            function        argument                   documentation */
	{ MODKEY,                       XK_space,      spawn,             {.v = termcmd } },           //? M-space: Launch terminal
	{ MODKEY,                       XK_r,          spawn,             {.v = runcmd } },            //? M-r: Launch app-runner
	{ MODKEY,                       XK_e,          spawn,             {.v = decmd } },             //? M-e: Launch de
	{ MODKEY,                       XK_s,          spawn,             {.v = shotcmd } },           //? M-s: Take screenshot
	{ MODKEY|ShiftMask,             XK_s,          spawn,             {.v = castcmd } },           //? M-S-s: Record screencast
	{ MODKEY,                       XK_b,          togglebar,         {0} },                       //? M-b: Toggle bar
	{ MODKEY|ShiftMask,             XK_b,          spawn,             {.v = reloadstatuscmd } },   //? M-S-b: Restart bar updater script
	{ MODKEY,                       XK_j,          focusstack,        {.i = +1 } },                //? M-j: Focus next window in stack
	{ MODKEY,                       XK_k,          focusstack,        {.i = -1 } },                //? M-k: Focus previous window in stack
	{ MODKEY|ShiftMask,             XK_j,          movestack,         {.i = +1 } },                //? M-S-j: Swap current window with next in stack
	{ MODKEY|ShiftMask,             XK_k,          movestack,         {.i = -1 } },                //? M-S-k: Swap current window with previous in stack
	{ MODKEY,                       XK_i,          incnmaster,        {.i = +1 } },                //? M-i: Increase number of windows in master area
	{ MODKEY,                       XK_d,          incnmaster,        {.i = -1 } },                //? M-d: Decrease number of windows in master area
	{ MODKEY,                       XK_h,          setmfact,          {.f = -0.05} },              //? M-h: Increase size of master area by 5%
	{ MODKEY,                       XK_l,          setmfact,          {.f = +0.05} },              //? M-l: Decrease size of master area by 5%
	{ MODKEY,                       XK_Return,     zoom,              {0} },                       //? M-Return: Set current window as master window
	{ MODKEY,                       XK_Tab,        view,              {0} },                       //? M-Tab: Toggle between current and first tag
	{ MODKEY,                       XK_w,          killclient,        {0} },                       //? M-w: Close window
	{ MODKEY,                       XK_t,          setlayout,         {.v = &layouts[0]} },        //? M-t: Set current tag layout to Tiled
	{ MODKEY|ShiftMask,             XK_t,          setlayout,         {.v = &layouts[3]} },        //? M-S-t: Set current tag layout to BottomStack
	{ MODKEY,                       XK_f,          setlayout,         {.v = &layouts[1]} },        //? M-f: Set current tag layout to Floating
	{ MODKEY,                       XK_m,          setlayout,         {.v = &layouts[2]} },        //? M-m: Set current tag layout to Monocle
	{ MODKEY|ShiftMask,             XK_f,          togglefloating,    {0} },                       //? M-S-f: Set the current window as floating
	{ MODKEY,                       XK_a,          view,              {.ui = ~0 } },               //? M-a: View all tags at once
	{ MODKEY|ShiftMask,             XK_a,          tag,               {.ui = ~0 } },               //? M-S-a: Make the current window visible on all tags
	{ MODKEY,                       XK_comma,      focusmon,          {.i = -1 } },                //? M-comma: Focus previous monitor
	{ MODKEY,                       XK_semicolon,  focusmon,          {.i = +1 } },                //? M-semicolon: Focus next monitor
	{ MODKEY|ShiftMask,             XK_comma,      tagmon,            {.i = -1 } },                //? M-S-comma: Move window to previous monitor
	{ MODKEY|ShiftMask,             XK_semicolon,  tagmon,            {.i = +1 } },                //? M-S-semicolon: Move window to next monitor
	TAGKEYS(                        XK_ampersand,                     0)                           //? M-<1-9>: View only windows from nth tag
	TAGKEYS(                        XK_eacute,                        1)                           //? M-C-<1-9>: Toggle windows from nth tag visibility
	TAGKEYS(                        XK_quotedbl,                      2)                           //? M-S-<1-9>: Change window tag to n
	TAGKEYS(                        XK_apostrophe,                    3)                           //? M-C-S-<1-9>: Add nth tag to window tags
	TAGKEYS(                        XK_parenleft,                     4)
	TAGKEYS(                        XK_minus,                         5)
	TAGKEYS(                        XK_egrave,                        6)
	TAGKEYS(                        XK_underscore,                    7)
	TAGKEYS(                        XK_ccedilla,                      8)
	{ MODKEY|ShiftMask,             XK_q,          quit,              {0} },                       //? M-S-q: Quit dwm
	{ MODKEY|ShiftMask,             XK_r,          selfrestart,       {0} },                       //? M-S-r: Restart dwm
	{ 0,                            XF86XK_AudioMute, spawn,          {.v = togglemute } },        //? XF86AudioMute: Toggle mute
	{ 0,                            XF86XK_AudioRaiseVolume,  spawn,  {.v = volumeupcmd } },       //? XF86AudioRaiseVolume: Volume up
	{ 0,                            XF86XK_AudioLowerVolume,  spawn,  {.v = volumedowncmd } },     //? XF86AudioLowerVolume: Volume down
	{ 0,                            XF86XK_MonBrightnessUp,   spawn,  {.v = brightnessupcmd } },   //? XF86MonBrightnessUp: Increase brightness
	{ 0,                            XF86XK_MonBrightnessDown, spawn,  {.v = brightnessdowncmd } }, //? XF86MonBrightnessDown: Decrease brightness
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

