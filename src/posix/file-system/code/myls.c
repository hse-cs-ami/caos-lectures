#include <dirent.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    const char *dirname = argv[1];
    DIR *d = opendir(dirname);
    struct dirent *ent;
    while ((ent = readdir(d))) {
        char buf[4096];  // чтобы записывать длинное имя файла
        snprintf(buf, sizeof(buf), "%s/%s", dirname, ent->d_name);
        printf("%s\n", buf);
    }
}