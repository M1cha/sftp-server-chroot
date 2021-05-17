#define _GNU_SOURCE

#include <dlfcn.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct passwd *(*real_getpwuid_t)(uid_t uid);

struct passwd *real_getpwuid(uid_t uid) {
    return ((real_getpwuid_t)dlsym(RTLD_NEXT, "getpwuid"))(uid);
}

struct passwd *getpwuid(uid_t uid_requested) {
    int err;
    static bool first = true;
    const char *sftp_chroot_dir;
    const char *sftp_uid_str;
    uid_t sftp_uid;
    struct passwd *pw;

    if (!first) {
        return real_getpwuid(uid_requested);
    }
    first = false;

    sftp_chroot_dir = getenv("SFTP_CHROOT_DIR");
    if (!sftp_chroot_dir) {
        fprintf(stderr, "SFTP_CHROOT_DIR missing\n");
        abort();
        return NULL;
    }

    sftp_uid_str = getenv("SFTP_UID");
    if (!sftp_uid_str) {
        fprintf(stderr, "SFTP_UID missing\n");
        abort();
        return NULL;
    }

    err = unsetenv("SFTP_CHROOT_DIR");
    if (err) {
        perror("unsetenv");
        abort();
        return NULL;
    }

    err = unsetenv("SFTP_UID");
    if (err) {
        perror("unsetenv");
        abort();
        return NULL;
    }

    err = unsetenv("LD_PRELOAD");
    if (err) {
        perror("unsetenv");
        abort();
        return NULL;
    }

    sftp_uid = (uid_t)strtoul(sftp_uid_str, NULL, 10);
    if (!sftp_uid) {
        fprintf(stderr, "invalid value for SFTP_UID: %s\n", sftp_uid_str);
        abort();
        return NULL;
    }

    pw = real_getpwuid(sftp_uid);
    if (!pw) {
        perror("getpwuid");
        abort();
        return NULL;
    }

    err = chroot(sftp_chroot_dir);
    if (err) {
        perror("chroot");
        abort();
        return NULL;
    }

    err = initgroups(pw->pw_name, pw->pw_gid);
    if (err) {
        perror("initgroups");
        abort();
        return NULL;
    }

    err = setgid(pw->pw_gid);
    if (err) {
        perror("setguid");
        abort();
        return NULL;
    }

    err = setuid(pw->pw_uid);
    if (err) {
        perror("setuid");
        abort();
        return NULL;
    }

    return pw;
}
