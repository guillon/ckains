#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>
#include <stdlib.h>

#include "options.h"
#include "config.h"
#include "cwd.h"

struct config config;
static struct option long_options[] = {
    {"rootfs",              required_argument, NULL, 'r'},
    {"root-id",             no_argument, NULL, '0'},
    {"bind",                required_argument, NULL, 'b'},
    {"mount",               required_argument, NULL, 'm'},
    {"bind-elsewhere",      required_argument, NULL, 'B'},
    {"mount-elsewhere",     required_argument, NULL, 'M'},
    {"pwd",                 required_argument, NULL, 'w'},
    {"cwd",                 required_argument, NULL, 'w'},
    {"working-directory",   required_argument, NULL, 'w'},
    {"32",                  no_argument, NULL, 'P'},
    {"32bit",               no_argument, NULL, 'P'},
    {"32bit-mode",          no_argument, NULL, 'P'},
};

static void setup_default_config(struct config *config)
{
    config->rootfs = "/";
    config->is_root_id = 0;
    config->mounts_nb = 0;
    config->cwd = cwd_at_startup;
    config->is_32_bit_mode = 0;
}

static void append_mount_point(struct config *config, char *source, char *target)
{
    assert(config->mounts_nb != MAX_MOUNT_INFO_NB);
    config->mounts[config->mounts_nb].source = source;
    config->mounts[config->mounts_nb].target = target;
    config->mounts_nb++;
}

int parse_options(int argc, char **argv)
{
    int opt;

    setup_default_config(&config);
    while((opt = getopt_long(argc, argv, "+r:0b:m:B:M:w:", long_options, NULL)) != -1) {
        switch(opt) {
            case 'r':
                config.rootfs = optarg;
                break;
            case '0':
                config.is_root_id = 1;
                break;
            case 'm':
            case 'b':
                append_mount_point(&config, optarg, optarg);
                break;
            case 'M':
            case 'B':
                /* As getopt has no support for multi parameter options we do
                 * some operation that are dependent of implementation and so
                 * fragile. incrementing optind to jump to next option is not
                 * sure to work everywhere. Other solution is to implement a
                 * custom parser ......
                 */
                if (argv[optind][0] == '-')
                    return -1;
                append_mount_point(&config, optarg, argv[optind]);
                optind++;
                break;
            case 'w':
                if (optarg[0] == '/')
                    config.cwd = optarg;
                else {
                    /* FIXME: replace this stuff with a resolve_path function also need in binding.c */
                    config.cwd = canonicalize_file_name(optarg);
                    if (!config.cwd)
                        config.cwd = "/";
                }
                break;
            case 'P':
                config.is_32_bit_mode = 1;
                break;
            default:
                return -1;
        }
    }

    return 0;
}

void print_usage()
{
    fprintf(stderr, "ckains : fixme\n");
}
