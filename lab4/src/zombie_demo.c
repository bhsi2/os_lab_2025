#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


void demonstrate_zombies() {
    printf("\n=== Демонстрация нескольких зомби-процессов ===\n\n");
    
    int num_zombies = 3;
    pid_t pids[3];
    
    for (int i = 0; i < num_zombies; i++) {
        pids[i] = fork();
        
        if (pids[i] == 0) {
            // Дочерний процесс
            printf("Зомби %d: PID = %d завершается\n", i+1, getpid());
            exit(0);
        } else if (pids[i] > 0) {
            printf("Создан дочерний процесс %d с PID = %d\n", i+1, pids[i]);
        } else {
            perror("Ошибка fork");
            exit(1);
        }
    }
    
    printf("\nСоздано %d зомби-процессов!\n", num_zombies);
    printf("Родительский процесс ждет 20 секунд...\n\n");
    
    sleep(20);
    
    // Убираем всех зомби
    printf("Убираем всех зомби...\n");
    for (int i = 0; i < num_zombies; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        printf("Зомби с PID = %d убран\n", pids[i]);
    }
}

int main() {
    int choice;
    
    printf("Демонстрация зомби-процессов\n");
    printf("=============================\n");
    
    while (1) {
        printf("\nВыберите действие:\n");
        printf("1 - Несколько зомби-процессов\n");
        printf("2 - Выход\n");
        printf("Ваш выбор: ");
        
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                demonstrate_zombies();
                break;
            case 2:
                printf("Выход...\n");
                exit(0);
            default:
                printf("Неверный выбор!\n");
        }
    }
    
    return 0;
}