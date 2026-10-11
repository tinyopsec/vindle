sloppy_focus               = true
bypass_surface_visibility  = false
log_level                  = "error"  -- "silent", "error", "info", "debug"

appearance = {
    -- Inner + outer border, both always drawn. Neutral default.
    inner_border_px   = 1,
    outer_border_px   = 1,
    gaps         = 6,      -- nwm gappx = 6. 0 = no gaps.
    smart_gaps   = false,

    root_color   = 0x222222ff,
    inner_border_color = 0x999999ff,
    outer_border_color = 0x000000ff,
    inner_focus_color  = 0xffffffff,
    outer_focus_color  = 0x999999ff,
    inner_urgent_color = 0xff0000ff,
    outer_urgent_color = 0xff0000ff,

    fullscreen_bg = 0x000000ff,
}


-- Note: the entire input section requires you to restart
-- the compositor once changed
input = {
    repeat_rate             = 50,
    repeat_delay            = 150,
    tap_to_click            = true,
    tap_and_drag            = true,
    drag_lock               = true,
    natural_scrolling       = false,
    disable_while_typing    = true,
    left_handed             = false,
    middle_button_emulation = false,
    scroll_method           = "2fg",        -- "2fg", "edge", "button"
    click_method            = "button_areas", -- "button_areas", "clickfinger"
    accel_profile           = "adaptive",   -- "adaptive", "flat"
    accel_speed             = 0.0,

    -- XKB layout. Default first at startup, Alt+Shift toggles.
    layout                  = "us,ru",
    variant                 = "",
    options                 = "grp:alt_shift_toggle",
}

rules = {
    -- nwm.h: { "Gimp", NULL, NULL, 0, 1 }, { "MPlayer", NULL, NULL, 0, 1 }
    { app_id = "Gimp",    floating = true,  monitor = -1 },
    { app_id = "MPlayer", floating = true,  monitor = -1 },
--   { app_id = "firefox", tags = 1 << 8,    floating = false, monitor = -1 },
}

monitors = {
    -- catch-all rule; name = nil means match anything
    -- nwm.h: mfact = 0.5, nmaster = 1
    { name = nil, mfact = 0.5, nmaster = 1, scale = 1.0,
      layout = "dwindle", x = -1, y = -1 },

    -- HiDPI laptop example:
    -- { name = "eDP-1", mfact = 0.5, nmaster = 1, scale = 2.0,
    --   layout = "dwindle", x = -1, y = -1 },
}

-- autostart: plain commands, run in order once the socket is live.
-- No systemd here: no `systemctl --user`, no `--systemd` flag.
autostart = {
    "dbus-update-activation-environment WAYLAND_DISPLAY XDG_SESSION_TYPE XDG_CURRENT_DESKTOP",
    "waybar",
}


-- Keymaps follow nwm (MODKEY = logo).
-- foot replaces st, fuzzel replaces dmenu_run (Wayland-native).
-- hjkl moves focus (no master-stack here); t = tile, f = float.
keybinds = {
    { mods = {"logo"},          key = "Return", action = "spawn",  args = {"foot"} },
    { mods = {"logo"},          key = "d",      action = "spawn",  args = {"fuzzel"} },
    { mods = {"logo"},          key = "q",      action = "killclient" },
    { mods = {"logo", "shift"}, key = "e",      action = "quit" },

    { mods = {"logo"},          key = "j",      action = "focusdir",  args = {"down"} },
    { mods = {"logo"},          key = "k",      action = "focusdir",  args = {"up"} },
    { mods = {"logo"},          key = "h",      action = "focusdir",  args = {"left"} },
    { mods = {"logo"},          key = "l",      action = "focusdir",  args = {"right"} },

    { mods = {"logo"},          key = "Tab",    action = "view" },
    -- layouts[] = { floating (0), dwindle (1) }
    { mods = {"logo"},          key = "t",      action = "setlayout", args = {"1"} },
    { mods = {"logo"},          key = "f",      action = "setlayout", args = {"0"} },
    { mods = {"logo"},          key = "F11",    action = "togglefullscreen" },
    { mods = {"logo", "shift"}, key = "space",  action = "togglefloating" },
    { mods = {"logo"},          key = "0",      action = "view",      args = {"all"} },
    { mods = {"logo", "shift"}, key = "0",      action = "tag",       args = {"511"} },

    -- extras (multi-monitor/dwindle, no nwm equivalent):
    { mods = {"logo"},          key = "g",      action = "togglegaps" },
    { mods = {"logo", "shift"}, key = "H",      action = "swapdir",   args = {"left"} },
    { mods = {"logo", "shift"}, key = "J",      action = "swapdir",   args = {"down"} },
    { mods = {"logo", "shift"}, key = "K",      action = "swapdir",   args = {"up"} },
    { mods = {"logo", "shift"}, key = "L",      action = "swapdir",   args = {"right"} },
    { mods = {"logo"},          key = "comma",  action = "focusmon",  args = {"left"} },
    { mods = {"logo"},          key = "period", action = "focusmon",  args = {"right"} },
    { mods = {"logo", "shift"}, key = "less",   action = "tagmon",    args = {"left"} },
    { mods = {"logo", "shift"}, key = "greater",action = "tagmon",    args = {"right"} },
}

-- TAGKEYS: 1..9 view, Ctrl+1..9 toggleview, Shift+1..9 tag, Ctrl+Shift+1..9 toggletag
for i = 1, 9 do
    local key  = tostring(i)
    local mask = 1 << (i - 1)
    table.insert(keybinds, { mods = {"logo"},                    key = key, action = "view",      args = {tostring(mask)} })
    table.insert(keybinds, { mods = {"logo", "ctrl"},            key = key, action = "toggleview", args = {tostring(mask)} })
    table.insert(keybinds, { mods = {"logo", "shift"},           key = key, action = "tag",        args = {tostring(mask)} })
    table.insert(keybinds, { mods = {"logo", "ctrl", "shift"},   key = key, action = "toggletag",  args = {tostring(mask)} })
end

-- buttons: logo+Left move, logo+Middle float toggle, logo+Right resize
buttons = {
    { mods = {"logo"}, button = "left",   action = "moveresize",     args = {"move"} },
    { mods = {"logo"}, button = "middle", action = "togglefloating" },
    { mods = {"logo"}, button = "right",  action = "moveresize",     args = {"resize"} },
}
