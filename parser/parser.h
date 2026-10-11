#ifndef VINDLE_PARSER_H
#define VINDLE_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <wayland-server-core.h>

/* Maximum lengths. Good for the soul, they say */
#define CFG_MAX_RULES      64
#define CFG_MAX_MONITORS   16
#define CFG_MAX_KEYBINDS   256
#define CFG_MAX_ARGS       16
#define CFG_MAX_STRLEN     256
#define CFG_MAX_AUTOSTART  32
#define CFG_MAX_AUTOSTART_CMD 512

/* Appearance */

typedef struct {
	unsigned int inner_border_px;
	unsigned int outer_border_px;
	unsigned int gaps;      
	bool         smart_gaps;
	float        inner_border_color[4];
	float        outer_border_color[4];
	float        inner_focus_color[4];
	float        outer_focus_color[4];
	float        inner_urgent_color[4];
	float        outer_urgent_color[4];
	float        root_color[4];
	float        fullscreen_bg[4];
} CfgAppearance;

/* Input */

typedef struct {
	int    repeat_rate;
	int    repeat_delay;
	bool   tap_to_click;
	bool   tap_and_drag;
	bool   drag_lock;
	bool   natural_scrolling;
	bool   disable_while_typing;
	bool   left_handed;
	bool   middle_button_emulation;
	int    scroll_method;   /* libinput enum value */
	int    click_method;    /* libinput enum value */
	int    send_events_mode;
	int    accel_profile;   /* libinput enum value */
	double accel_speed;
	int    tap_button_map;  /* libinput enum value */
	/* XKB keyboard layout, e.g. "us,ru". Empty = default. */
	char   layout[CFG_MAX_STRLEN];
	char   variant[CFG_MAX_STRLEN];
	char   options[CFG_MAX_STRLEN];
} CfgInput;

/* Window Rules  */

typedef struct {
	char     app_id[CFG_MAX_STRLEN];
	char     title[CFG_MAX_STRLEN];  /* empty string = wildcard */
	uint32_t tags;
	bool     floating;
	int      monitor;                /* -1 = current */
} CfgRule;

/* Monitor Rules */

typedef struct {
	char  name[CFG_MAX_STRLEN];   /* empty = catch-all */
	float mfact;
	int   nmaster;
	float scale;
	char  layout[CFG_MAX_STRLEN]; /* "dwindle", "floating", etc. */
	int   transform;              /* WL_OUTPUT_TRANSFORM_* */
	int   x, y;                   /* -1,-1 = autoconfigure */
} CfgMonitorRule;

/* keybinds */

typedef struct {
	uint32_t mods;                         /* WLR_MODIFIER_* bitmask */
	uint32_t key;                          /* XKB keysym */
	char     action[CFG_MAX_STRLEN];       /* "spawn", "killclient", etc. */
	char     args[CFG_MAX_ARGS][CFG_MAX_STRLEN];
	int      nargs;
} CfgKeybind;

/* mouse buttons */

typedef struct {
	uint32_t mods;
	uint32_t button;                 
	char     action[CFG_MAX_STRLEN];
	char     args[CFG_MAX_ARGS][CFG_MAX_STRLEN];
	int      nargs;
} CfgButton;

/* Autostart */

typedef struct {
	char cmd[CFG_MAX_AUTOSTART_CMD];
} CfgAutostart;

/* the whole shebang */

typedef struct {
	CfgAppearance appearance;

	CfgInput      input;

	CfgRule       rules[CFG_MAX_RULES];
	int           nrules;

	CfgMonitorRule monitors[CFG_MAX_MONITORS];
	int            nmonitors;

	CfgKeybind    keybinds[CFG_MAX_KEYBINDS];
	int           nkeybinds;

	CfgButton     buttons[CFG_MAX_KEYBINDS];
	int           nbuttons;

	bool          sloppyfocus;
	bool          bypass_surface_visibility;
	int           log_level; /* WLR_* log level */

	CfgAutostart  autostart[CFG_MAX_AUTOSTART];
	int           nautostart;
} Config;

/* Public API */

int config_load(const char *path, Config *cfg);

char *config_get_path(char *buf, size_t bufsz);

void config_apply_appearance(const Config *cfg);

void config_apply_input(const Config *cfg);

void config_apply_keybinds(const Config *cfg);

void config_autostart_run(const Config *cfg);

typedef void (*config_reload_cb)(const Config *cfg, void *userdata);

struct wl_event_source *config_watch_start(
	struct wl_event_loop *loop,
	const char           *path,
	config_reload_cb      cb,
	void                 *userdata
);

void config_watch_stop(struct wl_event_source *src);

#endif

