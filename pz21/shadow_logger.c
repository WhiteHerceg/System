#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

static volatile int shadow_alive = 1;

static void shadow_signal(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        shadow_alive = 0;
    }
}

static void shadow_time(char* buf, size_t size) {
    time_t now;
    struct tm* tm_info;
    time(&now);
    tm_info = localtime(&now);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

static void become_shadow() {
#ifndef _WIN32
    pid_t pid;

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    chdir("/");

    int fd;
    for (fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--) {
        close(fd);
    }
#endif
}

int main() {
#ifndef _WIN32
    become_shadow();
#endif

    signal(SIGTERM, shadow_signal);
    signal(SIGINT, shadow_signal);

    const char* shadow_log = "shadow_trail.log";
    FILE* shadow_file = fopen(shadow_log, "a");
    
    if (!shadow_file) {
        exit(EXIT_FAILURE);
    }

    char time_mark[64];
    shadow_time(time_mark, sizeof(time_mark));
    fprintf(shadow_file, "[%s] Shadow awakens...\n", time_mark);
    fflush(shadow_file);

    int step = 0;
    while (shadow_alive) {
        sleep(2);
        step++;
        
        shadow_time(time_mark, sizeof(time_mark));
        fprintf(shadow_file, "[%s] Shadow follows... (Step %d)\n", time_mark, step);
        fflush(shadow_file);
    }

    shadow_time(time_mark, sizeof(time_mark));
    fprintf(shadow_file, "[%s] Shadow fades away...\n", time_mark);
    fclose(shadow_file);

    return EXIT_SUCCESS;
}
