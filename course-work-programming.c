// for run use "cd "FILE_PATH" ; if ($?) { gcc -lm -march=native main.c -o main } ; if ($?) { .\main }"
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <stdbool.h>
#include <cpuid.h>
#include <math.h>
// задание игрового поля и фигуры игрока
char player = 'O';
char game_field[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
// проверка поддержки процессорной инструкции rdrand
bool rdrand_support()
{
    unsigned int eax = 1, ebx, ecx = 0, edx;
    __cpuid(1, eax, ebx, ecx, edx);
    int rdrand_support_value = (ecx & 0x40000000) ? 1 : 0;
    return rdrand_support_value;
}
// генерация псевдослучайного числа
bool rdrand()
{
    unsigned int rand_num;
    bool success;
    success = labs(_rdrand32_step(&rand_num));
    if (success)
        return rand_num;
    else
        return 1;
}
// вывод игрового поля
void print_board()
{
    printf("_______\n");
    for (int i = 0; i < 3; i++)
        printf("|%c|%c|%c|\n", game_field[i][0], game_field[i][1], game_field[i][2]);
    printf("‾‾‾‾‾‾‾\n");
}
// поиск одинаковой комбинации, необх. для победы
bool search_winner(char player)
{
    for (int i = 0; i < 3; i++)
    {
        if (game_field[i][0] == player && game_field[i][1] == player && game_field[i][2] == player)
            return 1;

        if (game_field[0][i] == player && game_field[1][i] == player && game_field[2][i] == player)
            return 1;
    }

    if ((game_field[0][0] == player && game_field[1][1] == player && game_field[2][2] == player) ||
        (game_field[0][2] == player && game_field[1][1] == player && game_field[2][0] == player))
        return 1;

    return 0;
}
// проверка полной заполненности поля
bool search_end_of_game()
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (game_field[i][j] == ' ')
                return 0;
    return 1;
}
// запись в игровое поле выбранной ячейки игроком
bool players_move(int x, int y, char player)
{
    if (x < 0 || x > 2 || y < 0 || y > 2)
    {
        printf("Ошибка! Необходимо вводить координаты в интервале от 0 до 2.\n");
        return 1;
    }

    if (game_field[x][y] != ' ')
    {
        printf("Ошибка! Ячейка уже занята. Введите другие координаты.\n");
        return 1;
    }

    game_field[x][y] = player;
    return 0;
}
// алгоритм выбора компьютером подходяцей ячейки
void computers_move(char player)
{
    char pc = (player == 'O') ? 'X' : 'O';
    int emptyCount = 0;
    int emptyX[9];
    int emptyY[9];

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (game_field[i][j] == ' ')
            {
                emptyX[emptyCount] = i;
                emptyY[emptyCount] = j;
                emptyCount++;
            }

    for (int i = 0; i < emptyCount; i++)
    {
        game_field[emptyX[i]][emptyY[i]] = player;
        if (search_winner(player))
            return;
        game_field[emptyX[i]][emptyY[i]] = ' ';
        game_field[emptyX[i]][emptyY[i]] = pc;
        if (search_winner(pc))
        {
            game_field[emptyX[i]][emptyY[i]] = player;
            return;
        }
        game_field[emptyX[i]][emptyY[i]] = ' ';
    }
    // выбор рандомного места при отсутствии мешающих игроку ячеек
    int randomIndex = abs(rdrand()) % emptyCount;
    int x = emptyX[randomIndex];
    int y = emptyY[randomIndex];
    game_field[x][y] = player;
}
int main()
{
    bool program_support = rdrand_support();
    if (!program_support)
    {
        printf("Данный процессор не поддерживается данной программой!\n");
        return 1;
    }
    int x, y;
    while (1)
    {
        // цикл действий игрока
    cycle_w:
        printf("Введите координаты хода (координаты вида: x y): ");
        scanf("%d %d", &x, &y);
        int wr = players_move(x, y, player); // вставить элемент игрока в игр. поле
        if (wr == 1)
            goto cycle_w; // возврат в случае неверно введеной координаты
        print_board();
        char pc_symb = (player == 'O') ? 'X' : 'O';
        if (search_winner(player))
        {
            printf("Игрок победил!\n");
            break;
        }
        else if (search_end_of_game())
        {
            printf("Ничья!\n");
            break;
        }
        // цикл действий ПК
        computers_move(pc_symb);
        printf("Компьютер закончил ход:\n");
        print_board();
        if (search_winner(pc_symb))
        {
            printf("Компьютер победил!\n");
            break;
        }
        else if (search_end_of_game())
        {
            printf("Ничья!\n");
            break;
        }
    }
    return 0;
}