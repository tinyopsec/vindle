<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=222222&height=80&section=header&text=vindle&fontSize=52&fontColor=FFFFFF&fontAlignY=72&animation=fadeIn" alt="vindle header">
  <br>
  <img src="https://readme-typing-svg.demolab.com?font=JetBrains+Mono&size=14&pause=1200&color=FFFFFF&center=true&vCenter=true&width=600&lines=Minimal+tiling+Wayland+compositor;Double+border.+Dwindle+layout.+us%2Cru+out+of+the+box;Small+source.+Lua+config.+No+animations" alt="vindle typing">
  <br>
  <img src="https://raw.githubusercontent.com/tinyopsec/assets/main/vindle/vindle.webp" style="width: 65%; height: auto;" alt="logo">
</div>

<p align="center">
  <img src="https://img.shields.io/badge/license-GPL--3.0-FFFFFF?style=flat&labelColor=222222&logo=opensourceinitiative&logoColor=FFFFFF" alt="license GPL-3.0">
  <img src="https://img.shields.io/badge/C11-CCCCCC?style=flat&label=lang&labelColor=222222&logo=c&logoColor=CCCCCC" alt="C11">
  <img src="https://img.shields.io/badge/%7E7700%20lines-FFFFFF?style=flat&label=source&labelColor=222222&logo=files&logoColor=FFFFFF" alt="about 7700 lines of C, headers and Lua">
  <img src="https://img.shields.io/badge/0.1-CCCCCC?style=flat&label=version&labelColor=222222&logo=tag&logoColor=CCCCCC" alt="version 0.1">
  &nbsp;
  <img src="https://img.shields.io/badge/Linux-FFFFFF?style=flat&labelColor=222222&logo=linux&logoColor=FFFFFF" alt="Linux">
  &nbsp;
  <img src="https://img.shields.io/badge/wlroots--0.20-CCCCCC?style=flat&label=compositor&labelColor=222222&logo=wayland&logoColor=CCCCCC" alt="wlroots 0.20">
  <img src="https://img.shields.io/badge/lua5.4-FFFFFF?style=flat&label=config&labelColor=222222&logo=lua&logoColor=FFFFFF" alt="lua5.4">
  <img src="https://img.shields.io/badge/make-CCCCCC?style=flat&label=build&labelColor=222222&logo=gnu&logoColor=CCCCCC" alt="make">
</p>

<p align="center">
  Fork of <a href="https://github.com/kantiankant/swindle">kantiankant/swindle</a> with respect, itself a fork of dwl
</p>

> [!TIP]
> **Want a status bar?** vindle draws none, but any layer shell bar works and the tiling area avoids it. See [External Bars](#external-bars)

## Contents

- [Quick Start](#quick-start)
- [What vindle is](#what-vindle-is)
- [What changed on top of swindle](#what-changed-on-top-of-swindle)
- [Features](#features)
- [Screenshots](#screenshots)
- [Installation](#installation)
- [Usage](#usage)
- [Key Bindings](#key-bindings)
- [Configuration](#configuration)
- [Window Rules](#window-rules)
- [Monitors](#monitors)
- [External Bars](#external-bars)
- [How It Works](#how-it-works)
- [Contributing](#contributing)
- [Related](#related)

---

## Quick Start

```sh
git clone https://github.com/tinyopsec/vindle
cd vindle
make && sudo make install
cp /etc/vindle/config.lua ~/.config/vindle/config.lua
vindle
```

> [!NOTE]
> The default terminal is `foot` and the default launcher is `fuzzel`. Change them in your config before starting if you use something else (see [Configuration](#configuration))
>
> vindle needs `XDG_RUNTIME_DIR` for the Wayland socket. On a seat it is set already; with a bare flow, export it first

---

## What vindle is

vindle manages windows and input on Wayland and nothing else. One tiling layout (dwindle) plus floating, 9 tags as a bitmask, predictable focus, a double border inherited from the swindle lineage and kept. No bar, no widgets, no wallpaper, no animations, no themes. External programs cover the rest: foot, fuzzel, waybar, swaybg.

It is written to be read. About 7700 lines of C, headers and Lua in total, the compositor itself about 3900. If you spare an evening you will know where everything lives.

---

## What changed on top of swindle

Everything below is user visible, no refactors for beauty:

- Comfortable standard keymap (hjkl focus, t tile, f float, g gaps)
- `us` first and `ru` second out of the box with Alt+Shift toggle (I am Kazakh, Russian is near native, Latin must work on first boot); one line in `input.layout` changes the order, binds work in every layout
- Dwindle honors the monitor `mfact` instead of always splitting evenly
- Mouse drag floats a tiled window in place and carries it across tags
- Unknown config keys print a warning instead of failing silently
- Monitor fields reapply on config save, no restart dance for them
- `smsg` client tool on the frozen dwl-ipc protocol
- Wayland native defaults: foot, fuzzel, waybar, swaybg
- A real man page (`man vindle`)

---

## Features

| Category | Details |
|---|---|
| Layouts | Dwindle (binary split, honors `mfact`) and floating |
| Workspaces | 9 tags via bitmasks; windows may carry multiple tags |
| Mouse support | Move, resize, toggle floating via modifier plus button; a tiled window floats in place at its current size; carry a dragged window to another tag with the tag keys |
| Gaps | Configurable `gaps` with `smart_gaps`; toggle at run time with `Mod+g` |
| Borders | Double ring: inner plus outer widths and colors for normal, focus and urgent states. The two rings plus the gap read as one calm frame, my favorite part of the look |
| Fullscreen | Toggle via keybind or client request |
| Urgent hints | Activation requests mark windows urgent instead of stealing focus |
| Dialogs | Dialog windows and fixed size windows float automatically |
| Layer shell | Bars and panels reserve space; tiling avoids it |
| XWayland | Optional at build time (`XWAYLAND` in `config.mk`) |
| Keyboard | `us` first, `ru` second with Alt+Shift toggle (my native pair, changeable in one line); layout independent binds with key repeat |
| Hot reload | Config watcher reapplies values on save; input options still need a restart |
| IPC | Frozen dwl-ipc surface plus `smsg` client tool |
| Compilation | Zero warnings under `-std=c11 -Wall -Wextra` |

---

## Screenshots

<img src="https://github.com/tinyopsec/distrohop/raw/main/assets/void3.webp" width="100%" alt="my rice: vindle on Void Linux">

This is my screenshot, vindle on Void Linux

---

## Installation

### Requirements

| Dependency | Purpose | Note |
|---|---|---|
| wlroots 0.20 | Compositor backend | Pinned major, compiled with libinput |
| wayland | Protocols | Server side |
| libinput | Pointer and touchpad config | |
| xkbcommon | Keyboard layouts | |
| lua5.4 | Config file | Frozen, no upgrades |
| wayland-protocols, wayland-scanner, pkg-config | Build time only | |
| xcb, xcb-icccm | XWayland build flag only | |

Install these with their development packages, then build. Tested on Void Linux.

### From Source

```sh
git clone --depth 1 https://github.com/tinyopsec/vindle
cd vindle
make
sudo make install   # installs to /usr/local/bin/vindle, config to /etc/vindle, man page to share/man/man1
```

Change `PREFIX` in `config.mk` to install elsewhere. To stage the install (packaging), use `make DESTDIR=/tmp/stage install`. Downstream packaging is out of scope for this repo on purpose: the portable `Makefile` covers all targets.

### BSD notes (best effort, community tested)

Linux is the first class target. BSD builds on a best effort basis with no promises.

```sh
gmake LUA_PKG=lua-5.4
sudo gmake ETCDIR=/usr/local/etc/vindle install
```

Start from a TTY, not from inside X11 (unset `DISPLAY` or set `WLR_BACKENDS` to pick the DRM backend). Seat access is required: enable `seatd`, join the `video` group. If no user config exists, the installed default is used directly.

### Uninstall

```sh
sudo make uninstall
```

---

## Usage

### Starting vindle

From a TTY:

```sh
vindle
```

With an explicit config or a startup command:

```sh
vindle -c ~/.config/vindle/config.lua
vindle -s "waybar"
```

`vindle -v` prints the version. `vindle -d` enables debug logging.

### Display Managers

For display managers:

```ini
# /usr/share/wayland-sessions/vindle.desktop
[Desktop Entry]
Name=vindle
Comment=Minimal tiling Wayland compositor
Exec=vindle
Type=Application
```

### Terminal and Launcher

Defaults in `example/config.lua`:

```lua
{ mods = {"logo"}, key = "Return", action = "spawn", args = {"foot"} },
{ mods = {"logo"}, key = "d",      action = "spawn", args = {"fuzzel"} },
```

Use any Wayland native pair instead, for example `foot` plus `bemenu` or `alacritty` plus `fuzzel`.

> [!IMPORTANT]
> Copy `/etc/vindle/config.lua` to `~/.config/vindle/config.lua` and edit the copy. Reinstalls overwrite `/etc/vindle`

---

## Key Bindings

The default modifier is Super (Win).

### Windows and Layouts

| Key | Action |
|---|---|
| Mod + Return | Spawn terminal (foot) |
| Mod + d | Spawn launcher (fuzzel) |
| Mod + q | Kill focused window |
| Mod + Shift + e | Quit vindle |
| Mod + h | Focus left |
| Mod + j | Focus down |
| Mod + k | Focus up |
| Mod + l | Focus right |
| Mod + Tab | Toggle to the previously viewed tag set |
| Mod + t | Dwindle layout |
| Mod + f | Floating layout |
| Mod + F11 | Toggle fullscreen |
| Mod + Shift + Space | Toggle floating for focused window |
| Mod + 0 | View all tags |
| Mod + Shift + 0 | Assign focused window to all tags |
| Mod + g | Toggle gaps |
| Mod + Shift + H/J/K/L | Swap with the neighbor in that direction |
| Mod + comma/period | Focus monitor left/right |
| Mod + Shift + less/greater | Send focused window to monitor left/right |

### Tags

| Key | Action |
|---|---|
| Mod + 1-9 | Switch to tag |
| Mod + Ctrl + 1-9 | Toggle tag view (show alongside current) |
| Mod + Shift + 1-9 | Move focused window to tag |
| Mod + Ctrl + Shift + 1-9 | Toggle tag assignment on focused window |

### Mouse (modifier held over a client window)

| Button | Action |
|---|---|
| Mod + Left | Move window (tiled windows float in place) |
| Mod + Middle | Toggle floating |
| Mod + Right | Resize window |

While dragging, tag and view keys apply to the dragged window, so it travels to the new tag with you.

All bindings live in `keybinds` and `buttons` in the Lua config.

---

## Configuration

The chain is `example/config.lua` (repo) to `/etc/vindle/config.lua` (installed default) to `~/.config/vindle/config.lua` (yours). The compositor watches your file and reloads on save.

| Section | Holds |
|---|---|
| Top level | `sloppy_focus`, `bypass_surface_visibility`, `log_level` |
| `appearance` | Inner and outer border widths and colors (normal, focus, urgent), `gaps`, `smart_gaps`, root and fullscreen colors |
| `input` | Repeat rate and delay, libinput options, `layout`/`variant`/`options` for XKB |
| `rules` | `app_id`, `title`, `tags`, `floating`, `monitor` per window |
| `monitors` | `name`, `mfact`, `nmaster`, `scale`, `layout`, `transform`, `x`, `y` per output |
| `keybinds`, `buttons` | Mods plus key or button plus action plus args |
| `autostart` | Plain shell commands, run in order once the socket is live |

Reload notes:

- Appearance, rules, binds, autostart list and monitor `mfact` apply on save without a restart
- Input options (repeat, libinput, layout) apply at startup only
- Misspelled keys print `unknown key` warnings to standard error. The session keeps running

---

## Window Rules

```lua
rules = {
    { app_id = "Gimp",    floating = true,  monitor = -1 },
    { app_id = "MPlayer", floating = true,  monitor = -1 },
}
```

Empty `app_id` or `title` matches anything. `monitor` -1 means current. Dialogs and fixed size windows float even without a rule.

---

## Monitors

```lua
monitors = {
    { name = nil, mfact = 0.5, nmaster = 1, scale = 1.0,
      layout = "dwindle", x = -1, y = -1 },
}
```

First match wins; empty `name` matches any output. `mfact` is the first child share of every dwindle split (0.5 splits evenly, 0.7 was tried live and looked ugly so 0.5 stays). `nmaster` is accepted but reserved by the dwindle layout. `x` and `y` of -1 mean automatic placement

---

## External Bars

vindle has no bar and never will: drawing widgets is outside its scope. It cooperates with layer shell bars, so the bar reserves its edge and tiling uses what is left.

Minimal [waybar](https://github.com/Alexays/Waybar) autostart (`~/.config/vindle/config.lua`):

```lua
autostart = {
    "dbus-update-activation-environment WAYLAND_DISPLAY XDG_SESSION_TYPE XDG_CURRENT_DESKTOP",
    "waybar",
}
```

Start wallpaper the same way (`swaybg -i ~/wall.png -m fill`), since the compositor manages no wallpaper itself.

---

## How It Works

vindle renders through the wlroots scene graph. Each window owns a scene tree with its surface plus eight border rectangles (four inner, four outer). Tiling writes geometry into a per tag binary tree and the tree is walked top down with integer arithmetic, so there is no pixel drift. Every split divides the parent in the `mfact` share; wider parents split left and right, taller ones top and bottom.

Tags are bitmasks. Each client carries a tag bitmask and the active view is a bitmask. A client is visible when the bitwise AND of its tags and the current view is nonzero.

Focus is explicit and predictable: the focused client leads the focus stack, tag switches refocus the top visible client, urgency never steals focus, fullscreen is tracked per monitor. The focused window is the dwindle insertion anchor, so new windows split what you look at.

State leaves the compositor two ways: human readable stdout lines (tags, titles, layouts per output, same shape as dwl) and the frozen dwl-ipc protocol consumed by `smsg`. The ext-workspace protocol publishes one workspace object per tag per monitor for panels that speak it.

It is hard to explain in words, but you can grasp the entire code structure if you spend an evening reading the code. I use this compositor daily, so rough edges get fixed fast.

---

## Contributing

Found a bug or a rough edge. Open an issue, I read everything myself.

<details>
<summary>What a good patch looks like</summary>

- Small change in current style, no rewrites for beauty
- No new dependencies, no new config mechanisms, no new IPC or protocols
- It builds with zero warnings (`cc -std=c11 -Wall -Wextra`), warnings itch
- Prove it: `luac5.4 -p example/config.lua` plus a headless run with socket up and zero config errors
- New behavior needs a live check or an honest `NOT VERIFIED` line

</details>

---

## Related

### Lineage

| Project | Link |
|---|---|
| swindle | [github.com/kantiankant/swindle](https://github.com/kantiankant/swindle) |
| dwl | [codeberg.org/dwl/dwl](https://codeberg.org/dwl/dwl) |

### Specifications

| Spec | Link |
|---|---|
| wlroots | [gitlab.freedesktop.org/wlroots/wlroots](https://gitlab.freedesktop.org/wlroots/wlroots) |
| Wayland | [wayland.freedesktop.org](https://wayland.freedesktop.org) |
| xkbcommon | [xkbcommon.org](https://xkbcommon.org/doc/current/) |
| Lua 5.4 | [lua.org/manual/5.4](https://www.lua.org/manual/5.4/) |

---

## License

GPL-3.0, see `LICENSE` plus attributions in `licenses/`
