#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define MAX_PATH_LENGTH 1024

ssize_t copy_file(const char *source, const char *destination) {
    char buffer[4096];
    ssize_t bytes_read, bytes_written, total_bytes = 0;

    int source_fd = open(source, O_RDONLY);
    if (source_fd == -1) {
        perror("Ошибка открытия исходного файла");
        exit(EXIT_FAILURE);
    }

    int destination_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (destination_fd == -1) {
        perror("Ошибка открытия файла назначения");
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(source_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(destination_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Ошибка записи в файл назначения");
            exit(EXIT_FAILURE);
        }
        total_bytes += bytes_written;
    }

    if (bytes_read == -1) {
        perror("Ошибка чтения исходного файла");
        exit(EXIT_FAILURE);
    }

    close(source_fd);
    close(destination_fd);

    return total_bytes;
}

void synchronize_directories(const char *dir1, const char *dir2, int max_processes) {
    DIR *dir = opendir(dir1);
    if (!dir) {
        perror("Ошибка открытия каталога Dir1");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  // Регулярный файл
            char source_path[MAX_PATH_LENGTH];
            char destination_path[MAX_PATH_LENGTH];

            snprintf(source_path, sizeof(source_path), "%s/%s", dir1, entry->d_name);
            snprintf(destination_path, sizeof(destination_path), "%s/%s", dir2, entry->d_name);

            if (access(destination_path, F_OK) == -1) {  // Проверка, существует ли файл в Dir2
                pid_t pid = fork();
                if (pid == -1) {
                    perror("Ошибка при создании процесса");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {  // Дочерний процесс
                    printf("PID процесса: %d, Копируется файл: %s\n", getpid(), entry->d_name);
                    ssize_t bytes_copied = copy_file(source_path, destination_path);
                    printf("PID процесса: %d, Файл скопирован: %s, Скопировано байт: %ld\n", getpid(), entry->d_name, bytes_copied);
                    exit(EXIT_SUCCESS);
                } else {
                    // Родительский процесс
                    while (waitpid(-1, NULL, WNOHANG) > 0);
                    if (max_processes > 0) {
                        max_processes--;
                    } else {
                        wait(NULL);
                    }
                }
            }
        }
    }

    closedir(dir);
}

int main() {
    char dir1[MAX_PATH_LENGTH], dir2[MAX_PATH_LENGTH];
    int max_processes;

    printf("Введите имя первого каталога: ");
    scanf("%s", dir1);
    printf("Введите имя второго каталога: ");
    scanf("%s", dir2);
    printf("Введите максимальное количество одновременно работающих процессов: ");
    scanf("%d", &max_processes);

    synchronize_directories(dir1, dir2, max_processes);

    return 0;
}
