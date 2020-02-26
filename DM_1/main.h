#pragma once

// Алфавит римской системы счисления
const char alphabet[];
const int  alphabet_values[];

// Коды команд
typedef enum {
	CMD_RomanToArabian = 1,
	CMD_ArabianToRoman = 2,
	CMD_PrintMode      = 3,
	CMD_ChangeMode     = 4,
	CMD_PrintSession   = 5,
	CMD_LoadSession    = 6,
	CMD_SaveSession    = 7,
	CMD_ClearSession   = 8,
	CMD_Exit           = 9
} Command;

#pragma pack(push, 1)

//Режимы работы конвертера
typedef enum {
	MODE_RuleOf3 = 3,
	MODE_RuleOf4 = 4
} Mode;

// Структуры данных для каждой команды
typedef struct {
	char roman[256];
	unsigned int arabian;
} Action_RomanToArabian, Action_ArabianToRoman;

typedef struct {
	Mode mode;
} Action_ChangeMode;

typedef struct {
	char path[256];
} Action_LoadSession, Action_SaveSession;

#pragma pack(pop)