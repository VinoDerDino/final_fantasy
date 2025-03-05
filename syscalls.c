#include <sys/stat.h>
#include <unistd.h>

int _write(int file, char *ptr, int len) {
    // Minimal: Einfach so tun, als wäre alles erfolgreich geschrieben.
    return len;
}

int _read(int file, char *ptr, int len) {
    // Keine Eingabe verfügbar
    return 0;
}

int _close(int file) {
    return 0;
}

int _lseek(int file, int ptr, int dir) {
    return 0;
}

int _isatty(int file) {
    // Einfach annehmen, dass es sich um ein Terminal handelt
    return 1;
}

void _exit(int status) {
    // Endlosschleife, da kein Betriebssystem vorhanden ist
    while (1);
}

int _kill(int pid, int sig) {
    return -1;
}

int _getpid(void) {
    return 1;
}

int _fstat(int file, struct stat *st) {
    // Weise an, dass der Dateideskriptor ein Zeichen-Gerät ist.
    st->st_mode = S_IFCHR;
    return 0;
}
