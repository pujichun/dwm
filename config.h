#include <X11/XF86keysym.h>

/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx          = 3; /* 窗口边框大小 */
static const unsigned int snap              = 32; /* 边缘依附宽度 */
static const unsigned int systraypinning    = 0; /* 托盘跟随的显示器 0代表不指定 */
static const unsigned int systrayspacing    = 2; /* 托盘间距 */
static const int systraypinningfailfirst    = 1; /* 1表示如果ping失败将在第一个显示器上显示托盘 否则在最后一个显示器上显示托盘*/
static const int showsystray                = 1; /* 是否显示系统托盘 0不显示 1显示 */
static const unsigned int gappih            = 10; /* 两个窗口水平方向的间距 */
static const unsigned int gappiv            = 10; /* 窗口间垂直方向的间距 */
static const unsigned int gappoh            = 10; /* 窗口与屏幕左右边缘的间距 */
static const unsigned int gappov            = 10; /* 窗口与屏幕上下边缘的间距 */
static const int smartgaps                  = 1; /* 1表示只有一个窗口时与屏幕边缘没有间距 */
static const int showbar                    = 1; /* 0不显示状态栏 1显示状态栏 */
static const int topbar                     = 0; /* 0状态栏显示在底部 1状态栏显示在顶部 */
static const Bool viewontag                 = True; /* 切换标签开关 */
static const char* fonts[]                  = { "Fira Code Nerd Font:size=13" }; /* 字体与字号 */
static const char dmenufont[]               = "Fira Code Nerd Font:size=13"; /*dmenu的字体与字号*/
static const char col_gray1[]               = "#222222";
static const char col_gray2[]               = "#444444";
static const char col_gray3[]               = "#bbbbbb";
static const char col_gray4[]               = "#ffffff";
static const char col_cyan[]                = "#37474F";
static const char col_border[]              = "#42A5F5";
static const unsigned int baralpha          = 0xd0; /*状态栏透明度*/
static const unsigned int borderalpha       = OPAQUE; /*边框透明度*/
static const char* colors[][3] = {
    /* workspace diff status color*/
    /*               fg         bg         border   */
    [SchemeNorm] = { col_gray3, col_gray1, col_gray2  },
    [SchemeSel]  = { col_gray4, col_cyan,  col_border },
    [SchemeHid]  = { col_cyan,  col_gray1, col_border },
};
static const unsigned int alphas[][3] = {
    /* workspace diff status alpha*/
    /*               fg      bg        border     */
    [SchemeNorm] = { OPAQUE, baralpha, borderalpha },
    [SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
static const char *tags[] = { "\ue795¹", "\ue743²", "\ue712³", "\uf303⁴", "\uf188⁵", "\ue725⁶", "\uf001⁷", "\ue5ff⁸", "\uf51c⁹" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",				NULL,       NULL,       0,            1,           -1 },
	{ "Android Emulator",	NULL,       NULL,       0,            1,           -1 },
	{ "Emulator",			NULL,       NULL,       0,            1,           -1 },
	{ "quemu-system-i386",	NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",			NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "tile",      tile },    /* first entry is default */
	{ "NULL",      NULL },    /* no layout function means floating behavior */
	{ "monocle",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2]            = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]      = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]       = { "st", NULL };
static const char *browsercmd[]    = { "google-chrome-stable", NULL };

static const char *upvol[]         = { "/home/pujic/scripts/vol-up.sh",  NULL };
static const char *downvol[]       = { "/home/pujic/scripts/vol-down.sh",  NULL };
static const char *mutevol[]       = { "/home/pujic/scripts/vol-toggle.sh",  NULL };

static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "80x24", NULL };

static const char* screenshotcmd[] = { "flameshot", "gui", NULL };

static const char* rofilanchcmd[] = { "rofi", "-show", "drun", NULL };
static const char* rofilanchruncmd[] = { "rofi", "-show", "run", NULL };

static Key keys[] = {
	/* modifier            key                      function        argument */
	{ MODKEY,              XK_p,                    spawn,			{.v = rofilanchcmd} }, /*open rofi*/
	{ MODKEY,              XK_d,                    spawn,          {.v = dmenucmd } }, /*open dmenu*/
	{ MODKEY,              XK_Return,               spawn,          {.v = termcmd } },	/*open terminal*/
	{ MODKEY,              XK_c,                    spawn,          {.v = browsercmd } }, /*open browser*/
	{ 0,                   XF86XK_AudioLowerVolume, spawn,          {.v = downvol } },  /*volume down*/
	{ 0,                   XF86XK_AudioMute,        spawn,          {.v = mutevol } },/*volume mute*/
	{ 0,                   XF86XK_AudioRaiseVolume, spawn,          {.v = upvol   } },/*volume up*/
	{ MODKEY,              XK_bracketleft,          spawn,          {.v = downvol } },
	{ MODKEY,              XK_backslash,            spawn,          {.v = mutevol } },
	{ MODKEY,              XK_bracketright,         spawn,          {.v = upvol   } },
	{ 0,                   XK_Print,                spawn,          {.v = screenshotcmd } }, /*screen shot*/
	{ MODKEY|ShiftMask,    XK_z,					spawn,          {.v = screenshotcmd } },
	{ MODKEY|ShiftMask,    XK_j,                    rotatestack,    {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_k,                    rotatestack,    {.i = -1 } },
	{ MODKEY,              XK_j,                    focusstack,     {.i = +1 } },
	{ MODKEY,              XK_k,                    focusstack,     {.i = -1 } },
	{ MODKEY,              XK_h,                    viewtoleft,     {0} },
	{ MODKEY,              XK_l,                    viewtoright,    {0} },
	{ MODKEY|ShiftMask,    XK_h,                    tagtoleft,      {0} },
	{ MODKEY|ShiftMask,    XK_l,                    tagtoright,     {0} },
	{ MODKEY|ShiftMask,    XK_u,                    incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_i,                    incnmaster,     {.i = -1 } },
	{ MODKEY,              XK_u,                    setmfact,       {.f = -0.05} },
	{ MODKEY,              XK_i,                    setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,    XK_Return,               zoom,           {0} },
	{ MODKEY,              XK_Tab,                  view,           {0} },
	{ MODKEY|ShiftMask,    XK_q,                    killclient,     {0} },
	{ MODKEY,              XK_t,                    setlayout,      {.v = &layouts[0]} },
	{ MODKEY,              XK_m,                    setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,    XK_f,                    fullscreen,     {0} },
	{ MODKEY,              XK_space,                setlayout,      {0} },
	{ MODKEY|ShiftMask,    XK_space,                togglefloating, {0} },  /*float window*/
	{ MODKEY,              XK_apostrophe,           togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,              XK_0,                    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,    XK_0,                    tag,            {.ui = ~0 } },
	{ MODKEY,              XK_comma,                focusmon,       {.i = -1 } },
	{ MODKEY,              XK_period,               focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_comma,                tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,    XK_period,               tagmon,         {.i = +1 } },
	{ MODKEY|ControlMask,  XK_q,					quit,           {0} },
	TAGKEYS(               XK_1,                      0)
	TAGKEYS(               XK_2,                      1)
	TAGKEYS(               XK_3,                      2)
	TAGKEYS(               XK_4,                      3)
	TAGKEYS(               XK_5,                      4)
	TAGKEYS(               XK_6,                      5)
	TAGKEYS(               XK_7,                      6)
	TAGKEYS(               XK_8,                      7)
	TAGKEYS(               XK_9,                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button1,        togglewin,      {0} },
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
