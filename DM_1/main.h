#pragma once

// Алфавит римской системы счисления
const char alphabet[];
const int  alphabet_values[];

// Коды команд
typedef enum {
	CMD_RomanToArabian = 1,
	CMD_ArabianToRoman = 2,
	CMD_PrintSession   = 3,
	CMD_LoadSession    = 4,
	CMD_SaveSession    = 5,
	CMD_ClearSession   = 6,
	CMD_Exit           = 7
} Command;

#pragma pack(push, 1)

// Структуры данных для каждой команды
typedef struct {
	char roman[256];
	unsigned int arabian;
} Action_1, Action_2;

typedef struct {
	char path[256];
} Action_4, Action_5;

#pragma pack(pop)