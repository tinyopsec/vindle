#define _GNU_SOURCE
#include "arg.h"
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#ifdef __FreeBSD__
#include <sys/un.h>
#include <sys/ucred.h>
#endif
#include <wayland-client.h>
#include <wayland-util.h>
#include "dwl-ipc-unstable-v2-protocol.h"

#define die(fmt, ...)	do { fprintf(stderr, fmt "\n", ##__VA_ARGS__); exit(EXIT_FAILURE); } while (0)

char *argv0;

static enum {
	NONE	= 0,
	SET	= 1<<0,
	GET	= 1<<1,
	WATCH	= 1<<2 | GET,
} mode = NONE;

static int Oflag;
static int Tflag;
static int Lflag;
static int Pflag;
static int Fflag;
static int oflag;
static int tflag;
static int lflag;
static int cflag;
static int vflag;
static int mflag;
static int fflag;

static uint32_t occ, seltags, sel, urg;

static char *output_name;
static char *focused_output;
static uint32_t tagcount;
static char *tagset;
static char *layout_name;
static size_t layoutcount, layout_idx;
static char *client_tags;

struct output {
	char *output_name;
	uint32_t name;
};
static size_t outputs_buflen = 4;
static struct output *outputs;
static size_t outputcount;

static struct wl_display *display;
static struct wl_registry *registry;
static struct zdwl_ipc_manager_v2 *dwl_ipc_manager;

static void
dwl_ipc_tags(void *data, struct zdwl_ipc_manager_v2 *dwl_ipc_manager, uint32_t count)
{
	tagcount = count;
	if (Tflag && mode & GET) printf("%d\n", tagcount);
}

static void
dwl_ipc_layout(void *data, struct zdwl_ipc_manager_v2 *dwl_ipc_manager, const char *name)
{
	if (lflag && mode & SET && strcmp(layout_name, name) == 0)
		layout_idx = layoutcount;
	if (Lflag && mode & GET) printf("%s\n", name);
	layoutcount++;
}

static const struct zdwl_ipc_manager_v2_listener dwl_ipc_listener = {
	.tags = dwl_ipc_tags,
	.layout = dwl_ipc_layout
};

static void
dwl_ipc_output_toggle_visibility(void *data, struct zdwl_ipc_output_v2 *dwl_ipc_output)
{
	if (!vflag) return;
	char *oname = data;
	if (oname) printf("%s ", oname);
	printf("toggle\n");
}

static void
dwl_ipc_output_active(void *data, struct zdwl_ipc_output_v2 *dwl_ipc_output,
	uint32_t active)
{
	if (active && (!output_name || !*output_name))
		output_name = strdup(data);
	if (active) {
		free(focused_output);
		focused_output = data ? strdup(data) : NULL;
	}
	if (!oflag) return;
	char *oname = data;
	if (oname) printf("%s ", oname);
	printf("selmon %u\n", !!active);
}

static void
dwl_ipc_output_tag(void *data, struct zdwl_ipc_output_v2 *dwl_ipc_output,
	uint32_t tag, uint32_t state, uint32_t clients, uint32_t focused)
{
	if (!tflag) return;
	if (state != ZDWL_IPC_OUTPUT_V2_TAG_STATE_NONE) seltags |= 1<<tag;
	if (state == ZDWL_IPC_OUTPUT_V2_TAG_STATE_ACTIVE) urg |= 1<<tag;
	if (clients > 0) occ |= 1<<tag;
	if (!(mode & GET)) return;
	if (data) printf("%s ", (char *)data);
	printf("tag %u %u %u %u\n", tag, state, clients, focused);
}

static void
dwl_ipc_output_layout(void *data, struct zdwl_ipc_output_v2 *dwl_ipc_output,
	uint32_t layout)
{
}

static void
dwl_ipc_output_layout_symbol(void *data, struct zdwl_ipc_output_v2 *dwl_ipc_output,
	const char *layout)
{
	if (!(lflag && mode & GET)) return;
	if (data) printf("%s ", (char *)data);
	printf("layout %s\n", layout);
}

static void
dwl_ipc_output_title(void *data, struct zdwl_ipc_output_v2 *dwl_ipc_output,
	const char *title)
{
	if (!(cflag && mode & GET)) return;
	if (data) printf("%s ", (char *)data);
	printf("title %s\n", title);
}

static void
dwl_ipc_output_appid(void *data, struct zdwl_ipc_output_v2 *dwl_ipc_output,
	const char *appid)
{
	if (!(cflag && mode & GET)) return;
	if (data) printf("%s ", (char *)data);
	printf("appid %s\n", appid);
}

static void
dwl_ipc_output_fullscreen(void *data, struct zdwl_ipc_output_v2 *dwl_ipc_output,
	uint32_t is_fullscreen)
{
	if (!mflag) return;
	if (data) printf("%s ", (char *)data);
	printf("fullscreen %u\n", is_fullscreen);
}

static void
dwl_ipc_output_floating(void *data, struct zdwl_ipc_output_v2 *dwl_ipc_output,
	uint32_t is_floating)
{
	if (!fflag) return;
	if (data) printf("%s ", (char *)data);
	printf("floating %u\n", is_floating);
}

static void
dwl_ipc_output_frame(void *data, struct zdwl_ipc_output_v2 *dwl_ipc_output)
{
	if (mode & SET) {
		if (data && !(output_name && strcmp(output_name, (char *)data) == 0)) return;
		if (lflag) {
			if (layout_idx == 0) {
				for (char *c = layout_name; *c; c++) {
					if (*c < '0' || *c > '9')
						die("bad layout %s", layout_name);
					layout_idx = *c-'0' + layout_idx*10;
				}
			}
			if (layout_idx >= layoutcount)
				die("bad layout %s", layout_name);
			zdwl_ipc_output_v2_set_layout(dwl_ipc_output, layout_idx);
		}
		if (tflag) {
			uint32_t mask = seltags;
			char *t = tagset;
			uint32_t i = 0;
			int toggle = 1;

			if (*t == '!') {
				toggle = 0;
				t++;
			}

			for (; *t && *t >= '0' && *t <= '9'; t++)
				i = *t-'0' + i*10;

			if (toggle && !*t) mask = 1<<i;

			for (; *t; t++, i++) {
				switch (*t) {
				case '-':
					mask &= ~(1<<i);
					break;
				case '+':
					mask |= 1<<i;
					break;
				case '^':
					mask ^= 1<<i;
					break;
				}
			}

			if (i > tagcount) die("bad tagset %s", tagset);

			zdwl_ipc_output_v2_set_tags(dwl_ipc_output, mask, toggle);
		}
		if (cflag) {
			uint32_t and = ~0, xor = 0;
			char *t = client_tags;
			uint32_t i = 0;

			for (; *t && *t >= '0' && *t <= '9'; t++)
				i = *t-'0' + i*10;

			if (!*t) {
				and = 0;
				xor = 1<<i;
			}

			for (; *t; t++, i++) {
				switch (*t) {
				case '-':
					and &= ~(1<<i);
					break;
				case '+':
					and &= ~(1<<i);
					xor |= 1<<i;
					break;
				case '^':
					xor |= 1<<i;
					break;
				}
			}
			if (i > tagcount) die("bad client tagset %s", client_tags);

			zdwl_ipc_output_v2_set_client_tags(dwl_ipc_output, and, xor);
		}
		wl_display_flush(display);
		exit(0);
	} else {
		if (Fflag && mode == WATCH && focused_output)
			printf("focusmon %s\n", focused_output);
		if (tflag) {
			if (data) printf("%s ", (char *)data);
			printf("tags %u %u %u %u\n", occ, seltags, sel, urg);
			occ = seltags = sel = urg = 0;
		}
	}
	fflush(stdout);
}

static const struct zdwl_ipc_output_v2_listener dwl_ipc_output_listener = {
	.toggle_visibility = dwl_ipc_output_toggle_visibility,
	.active = dwl_ipc_output_active,
	.tag = dwl_ipc_output_tag,
	.layout = dwl_ipc_output_layout,
	.title = dwl_ipc_output_title,
	.appid = dwl_ipc_output_appid,
	.layout_symbol = dwl_ipc_output_layout_symbol,
	.fullscreen = dwl_ipc_output_fullscreen,
	.floating = dwl_ipc_output_floating,
	.frame = dwl_ipc_output_frame,
};

static void
wl_output_name(void *data, struct wl_output *output, const char *name)
{
	if (outputs) {
		struct output *o = (struct output *)data;
		o->output_name = strdup(name);
		printf("+ ");
	}
	if (Oflag) printf("%s\n", name);
	if (output_name && *output_name && strcmp(output_name, name) != 0) {
		wl_output_release(output);
		return;
	}
	if (dwl_ipc_manager) {
		struct zdwl_ipc_output_v2 *dwl_ipc_output =
			zdwl_ipc_manager_v2_get_output(dwl_ipc_manager, output);
		zdwl_ipc_output_v2_add_listener(dwl_ipc_output, &dwl_ipc_output_listener,
				output_name && *output_name ? NULL : strdup(name));
	}
}

static void
wl_output_geometry(void *data, struct wl_output *wl_output, int32_t x, int32_t y, int32_t w, int32_t h, int32_t subpixel, const char *make, const char *model, int32_t transform) {
}

static void
wl_output_mode(void *data, struct wl_output *wl_output, uint32_t flags, int32_t width, int32_t height, int32_t refresh)
{
}

static void
wl_output_done(void *data, struct wl_output *wl_output)
{
}

static void
wl_output_scale(void *data, struct wl_output *wl_output, int32_t factor)
{
}

static void
wl_output_description(void *data, struct wl_output *output, const char *description)
{
}

static const struct wl_output_listener output_listener = {
	.geometry = wl_output_geometry,
	.mode = wl_output_mode,
	.done = wl_output_done,
	.scale = wl_output_scale,
	.name = wl_output_name,
	.description = wl_output_description,
};

static void
global_add(void *data, struct wl_registry *wl_registry,
		uint32_t name, const char *interface, uint32_t version)
{
	if (strcmp(interface, wl_output_interface.name) == 0) {
		struct wl_output *o = wl_registry_bind(
				wl_registry, name, &wl_output_interface,
				WL_OUTPUT_NAME_SINCE_VERSION);
		wl_output_add_listener(o, &output_listener, outputs ? &outputs[outputcount] : NULL);
		if (!outputs) return;
		if (outputcount >= outputs_buflen) {
			outputs_buflen *= 2;
			outputs = reallocarray(outputs, outputs_buflen, sizeof(struct output));
		}
		outputs[outputcount].name = name;
		outputcount++;
	} else if (strcmp(interface, zdwl_ipc_manager_v2_interface.name) == 0) {
		dwl_ipc_manager = wl_registry_bind(wl_registry, name,
			&zdwl_ipc_manager_v2_interface, version < 2 ? version : 2);
		zdwl_ipc_manager_v2_add_listener(dwl_ipc_manager, &dwl_ipc_listener, NULL);
	}
}

static void
global_remove(void *data, struct wl_registry *wl_registry, uint32_t name)
{
	if (!outputs) return;
	for (size_t i = 0; i < outputcount; i++) {
		if (outputs[i].name == name) {
			printf("- %s\n", outputs[i].output_name);
			free(outputs[i].output_name);
			outputs[i] = outputs[--outputcount];
		}
	}
}

static const struct wl_registry_listener registry_listener = {
	.global = global_add,
	.global_remove = global_remove,
};

static void
usage(int code)
{
	fprintf(code ? stderr : stdout,
			"usage:"
			"\t%1$s [-OTLPF]\n"
			"\t%1$s [-o <output>] -s [-t <tags>] [-l <layout>] [-c <tags>]\n"
			"\t%1$s [-o <output>] (-g | -w) [-FOotlcvmf]\n",
			argv0);
	exit(code);
}

int
main(int argc, char *argv[])
{
	ARGBEGIN {
	case '?': case 'h':
		usage(0); break;
	case 's':
		if (mode != NONE) usage(1);
		mode = SET;
		break;
	case 'g':
		if (mode != NONE) usage(1);
		mode = GET;
		break;
	case 'w':
		if (mode != NONE) usage(1);
		mode = WATCH;
		break;
	case 'o':
		if (mode & SET)
			output_name = EARGF(usage(1));
		else if (mode & GET || !(output_name = ARGF()))
			oflag = 1;
		break;
	case 't':
		tflag = 1;
		if (mode & GET) break;
		mode = SET;
		tagset = EARGF(usage(1));
		break;
	case 'l':
		lflag = 1;
		if (mode & GET) break;
		mode = SET;
		layout_name = EARGF(usage(1));
		break;
	case 'c':
		cflag = 1;
		if (mode & GET) break;
		mode = SET;
		client_tags = EARGF(usage(1));
		break;
	case 'O':
		Oflag = 1;
		if (mode & SET) usage(1);
		if (mode & WATCH)
			outputs = malloc(outputs_buflen * sizeof(struct output));
		else
			mode = GET;
		break;
	case 'T':
		Tflag = 1;
		if (mode && mode != GET) usage(1);
		mode = GET;
		break;
	case 'L':
		Lflag = 1;
		if (mode && mode != GET) usage(1);
		mode = GET;
		break;
	case 'P':
		Pflag = 1;
		if (mode && mode != GET) usage(1);
		mode = GET;
		break;
	case 'F':
		Fflag = 1;
		if (mode && mode != GET) usage(1);
		mode = GET;
		break;
	case 'v':
		vflag = 1;
		if (mode & SET) usage(1);
		mode |= GET;
		break;
	case 'm':
		mflag = 1;
		if (mode & SET) usage(1);
		mode |= GET;
		break;
	case 'f':
		fflag = 1;
		if (mode & SET) usage(1);
		mode |= GET;
		break;
	default:
		fprintf(stderr, "%s: bad option: -%c\n", argv0, ARGC());
		usage(1);
	} ARGEND
	if (argc > 0) {
		fprintf(stderr, "%s: bad args:", argv0);
		for (; *argv; argv++)
			fprintf(stderr, " %s", argv[0]);
		fprintf(stderr, "\n");
		usage(1);
	}
	if (mode == NONE) usage(1);
	if (mode & GET && !(oflag || tflag || lflag || Oflag || Tflag || Lflag || Pflag || Fflag || cflag || vflag || mflag || fflag))
		oflag = tflag = lflag = cflag = vflag = mflag = fflag = Fflag = 1;

	display = wl_display_connect(NULL);
	if (!display) die("bad display");

	if (Pflag) {
		int fd = wl_display_get_fd(display);
#ifdef __FreeBSD__
		struct xucred xucred;
		socklen_t len = sizeof(xucred);
		if (getsockopt(fd, SOL_LOCAL, LOCAL_PEERCRED, &xucred, &len) == -1)
			die("bad getsockopt");
		printf("%d\n", xucred.cr_pid);
#else
		struct ucred ucred;
		socklen_t len = sizeof(struct ucred);
		if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &ucred, &len) == -1)
			die("bad getsockopt");
		printf("%d\n", ucred.pid);
#endif
	}

	registry = wl_display_get_registry(display);
	if (!registry) die("bad registry");

	wl_registry_add_listener(registry, &registry_listener, NULL);

	wl_display_dispatch(display);
	wl_display_roundtrip(display);

	if (!dwl_ipc_manager && (mode & SET || tflag || lflag || Tflag || Lflag || cflag || vflag || mflag || fflag))
		die("bad dwl-ipc protocol");

	wl_display_roundtrip(display);

	if (Fflag && !(mode == WATCH) && focused_output)
		printf("focusmon %s\n", focused_output);

	if (mode == WATCH)
		while (wl_display_dispatch(display) != -1);

	return 0;
}
