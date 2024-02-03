#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

// Функция для получения текущего времени в формате часы:минуты:секунды:миллисекунды
void get_current_time(char *buffer) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    long milliseconds = tv.tv_usec / 1000;
    sprintf(buffer, "%ld:%ld:%ld:%ld", (tv.tv_sec / 3600) % 24, (tv.tv_sec / 60) % 60, tv.tv_sec % 60, milliseconds);
}

int main() {
    pid_t pid1, pid2;
    char time_buffer[32];

    // Создание первого дочернего процесса
    pid1 = fork();

    if (pid1 < 0) {
        perror("Ошибка при создании первого дочернего процесса");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        // Этот код выполняется в первом дочернем процессе
        printf("PID первого дочернего процесса: %d, PPID: %d\n", getpid(), getppid());

        get_current_time(time_buffer);
        printf("Текущее время: %s\n", time_buffer);

        exit(EXIT_SUCCESS);
    } else {
        // Код родительского процесса
        // Создание второго дочернего процесса
        pid2 = fork();

        if (pid2 < 0) {
            perror("Ошибка при создании второго дочернего процесса");
            exit(EXIT_FAILURE);
        } else if (pid2 == 0) {
            // Этот код выполняется во втором дочернем процессе
            printf("PID второго дочернего процесса: %d, PPID: %d\n", getpid(), getppid());

            get_current_time(time_buffer);
            printf("Текущее время: %s\n", time_buffer);

            exit(EXIT_SUCCESS);
        } else {
            // Этот код выполняется в родительском процессе
            printf("PID родительского процесса: %d\n", getpid());
            printf("Запуск ps -x...\n");

            // Выполнение команды ps -x с помощью system()
            system("ps -x");

            // Ожидание завершения обоих дочерних процессов
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}
