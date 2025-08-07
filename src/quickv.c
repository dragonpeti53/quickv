#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    FILE* file;
} quickv;

quickv* opendb(const char* filename) {
    FILE* file = fopen(filename, "r+b");

    if (!file) {

        file = fopen(filename, "wb");
        if (!file) return NULL;

        unsigned char byte = 0xFF;
        fwrite(&byte, 1, 1, file);
        fclose(file);


        file = fopen(filename, "r+b");
        if (!file) return NULL;
    }

    quickv* db = malloc(sizeof(quickv));


    db->file = fopen(filename, "r+b");
    if (!db->file) {
        free(db);
        return NULL;
    }

    return db;
}

int quickv_set(quickv* db, const char* key, const char *value) {
    if (!db || !key || !value) return -1;

    FILE* file = db->file;

    fseek(file, 0, SEEK_SET);

    while (fgetc(file) != 0xFF) {}

    fseek(file, -1, SEEK_CUR);
    long start = ftell(file);
    fseek(file, 0, SEEK_END);
    long end = ftell(file);

    size_t size = (size_t)(end - start);

    unsigned char* buffer = malloc(size);

    fseek(file, start, SEEK_SET);

    fread(buffer, 1, size, file);

    fseek(file, start, SEEK_SET);

    fwrite(key, 1, strlen(key), file);
    char null_byte = '\0';
    fwrite(&null_byte, 1, 1, file);
    uint64_t offset = ftell(file) + size + sizeof(uint64_t);
    fwrite(&offset, 1, sizeof(uint64_t), file);

    fwrite(buffer, 1, size, file);
    free(buffer);

    fwrite(value, 1, strlen(value), file);
    fwrite(&null_byte, 1, 1, file);

    return 0;

}

const char* quickv_get(quickv* db, const char* key) {
    if (!db || !key) return NULL;
    FILE* file = db->file;
    fseek(file, 0, SEEK_SET);

    size_t target_len = strlen(key);

    int byte;
    while((byte = fgetc(file)) != EOF) {
        unsigned char c = (unsigned char)byte;
        if (c == 0xFF) return NULL;
        else if (c == 0x00) fseek(file, 8, SEEK_CUR);

        if (c == key[0]) {
            int matched = 1;
            for (size_t i = 1; i < target_len; i++) {
                int next_byte = fgetc(file);
                if ((unsigned char)next_byte != (unsigned char)key[i]) {
                    matched = 0;
                    break;
                }
            }
            if (matched) {
                fseek(file, 1, SEEK_CUR);
                uint64_t offset;
                fread(&offset, sizeof(uint64_t), 1, file);
                fseek(file, (long)offset, SEEK_SET);

                static char buffer[1024];
                size_t i = 0;
                int ch;
                while (i < sizeof(buffer) - 1 && (ch = fgetc(file)) != EOF && ch != '\0') {
                    buffer[i++] = (char)ch;
                }
                buffer[i] = '\0';

                return buffer;
            }
        }


    }
return NULL;
}

int main() {
    quickv* db = opendb("test.db");
    //quickv_set(db, "key", "value");
    printf("%s\n", quickv_get(db, "key"));
}