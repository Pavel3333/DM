﻿#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

#define NAME        "Number Converter"
#define DESCRIPTION "This program can convert numbers between roman and arabian number systems"
#define VERSION     "1.0"
#define AUTHOR      "Ushaev Pavel (group M3O-121B-19)"

// Алфавит римской системы счисления
const char alphabet[] = {
	'M',
	'D',
	'C',
	'L',
	'X',
	'V',
	'I'
};

const int alphabet_values[] = {
	1000,
	500,
	100,
	50,
	10,
	5,
	1
};

/* Глобальные переменные */

// Сессия - динамический массив, состоящий из команд (Command) и действий (Action)
unsigned int session_size = 0;
char*        session      = NULL;

/*
Структура:
	Command command1
	Action_X action1
	Command command2,
	Action_X action2
	...
*/

//Режим работы конвертера
Mode mode = MODE_RuleOf4;

// Дескриптор файла журнала
FILE* log_file = NULL;

void log(const char* data) { // Журналирование в файл всего, что мы выводим
	if (!data) return;

	if (log_file)
		fwrite(data, 1, strlen(data), log_file);

	printf("%s", data);
}

void add_command_to_session(Command command, void* data, unsigned int size) {
	char* new_session = realloc(session, session_size + sizeof(command) + size);
	if (!new_session) {
		log("Unable to reallocate memory for the command in the session\n");
		return;
	}
	session = new_session;

	unsigned int offset = session_size;

	*(Command*)(session + offset) = command;
	offset += sizeof(command);

	if (data) {
		memcpy(session + offset, data, size);
		offset += size;
	}

	session_size = offset;
}

void load_session(char* path) {
	FILE* session_file = fopen(path, "rb");
	if (!session_file) {
		log("Unable to open session file\n");
		return;
	}

	unsigned int offset = 0;
	unsigned int readed = 0;

	readed = fread(&mode, sizeof(mode), 1, session_file);
	if (readed != 1) {
		log("Unable to read converter mode from the file\n");
		return;
	}

	readed = fread(&session_size, sizeof(session_size), 1, session_file);
	if (readed != 1) {
		log("Unable to read session size from the file\n");
		return;
	}

	char* new_session = malloc(session_size);
	if (!new_session) {
		log("Unable to allocate memory for the new session\n");
		return;
	}
	free(session);
	session = new_session;

	while (offset < session_size) {
		Command command;

		readed = fread(&command, sizeof(command), 1, session_file);
		if (readed != 1) {
			log("Unable to read command from the file\n");
			break;
		}

		if (command < 0 || command > CMD_Exit) {
			log("Error while loading session: unknown command\n");
			break;
		}

		*(Command*)(session + offset) = command; // Записываем команду в память
		offset += sizeof(command);

		if (command == CMD_RomanToArabian) {
			readed = fread(session + offset, sizeof(Action_RomanToArabian), 1, session_file);
			if (readed != 1) {
				log("Unable to read action from the file\n");
				break;
			}
			offset += sizeof(Action_RomanToArabian);
		}
		else if (command == CMD_ArabianToRoman) {
			readed = fread(session + offset, sizeof(Action_ArabianToRoman), 1, session_file);
			if (readed != 1) {
				log("Unable to read action from the file\n");
				break;
			}
			offset += sizeof(Action_ArabianToRoman);
		}
		else if (command == CMD_ChangeMode) {
			readed = fread(session + offset, sizeof(Action_ChangeMode), 1, session_file);
			if (readed != 1) {
				log("Unable to read action from the file\n");
				break;
			}
			mode = ((Action_ChangeMode*)(session + offset))->mode;
			offset += sizeof(Action_ChangeMode);
		}
		else if (command == CMD_LoadSession) {
			readed = fread(session + offset, sizeof(Action_LoadSession), 1, session_file);
			if (readed != 1) {
				log("Unable to read action from the file\n");
				break;
			}
			offset += sizeof(Action_LoadSession);
		}
		else if (command == CMD_SaveSession) {
			readed = fread(session + offset, sizeof(Action_SaveSession), 1, session_file);
			if (readed != 1) {
				log("Unable to read action from the file\n");
				break;
			}
			offset += sizeof(Action_SaveSession);
		}
	}

	fclose(session_file);

	if (offset != session_size) {
		printf("Error: was readed not all session (%u of %u)\n", offset, session_size);
		log("Session corrupted and will be cleared\n");

		free(session);
		session = NULL;
		session_size = 0;
	}
	else
		log("Successfully loaded\n");
}

void save_session(char* path) {
	FILE* session_file = fopen(path, "wb");
	if (!session_file) {
		log("Unable to open session file\n");
		return;
	}

	unsigned int offset = 0;

	fwrite(&mode,         1, sizeof(mode),         session_file);
	fwrite(&session_size, 1, sizeof(session_size), session_file);

	while (offset < session_size) {
		Command command = *(Command*)(session + offset);
		if (command < 0 || command > CMD_Exit) {
			log("Error while saving session: unknown command\n");
			break;
		}

		fwrite(&command, 1, sizeof(command), session_file);
		offset += sizeof(command);

		if (command == CMD_RomanToArabian) {
			fwrite((Action_RomanToArabian*)(session + offset), 1, sizeof(Action_RomanToArabian), session_file);
			offset += sizeof(Action_RomanToArabian);
		}
		else if (command == CMD_ArabianToRoman) {
			fwrite((Action_ArabianToRoman*)(session + offset), 1, sizeof(Action_ArabianToRoman), session_file);
			offset += sizeof(Action_ArabianToRoman);
		}
		else if (command == CMD_ChangeMode) {
			fwrite((Action_ChangeMode*)(session + offset), 1, sizeof(Action_ChangeMode), session_file);
			offset += sizeof(Action_ChangeMode);
		}
		else if (command == CMD_LoadSession) {
			fwrite((Action_LoadSession*)(session + offset), 1, sizeof(Action_LoadSession), session_file);
			offset += sizeof(Action_LoadSession);
		}
		else if (command == CMD_SaveSession) {
			fwrite((Action_SaveSession*)(session + offset), 1, sizeof(Action_SaveSession), session_file);
			offset += sizeof(Action_SaveSession);
		}
	}

	fclose(session_file);

	if (offset != session_size)
		printf("Error: was writed not all session (%u of %u)\n", offset, session_size);
	else
		log("Successfully saved\n");
}

unsigned int find_value_by_letter(char letter) {
	for (int i = 0; i < sizeof(alphabet); i++)
		if (alphabet[i] == letter)
			return alphabet_values[i];
	return 0;
}

unsigned int roman2arabian(const char* roman) { // Перевод из римской в арабскую с/с
	unsigned int result = 0;

	unsigned int max_value = find_value_by_letter('I');

	bool flag = false;

/*
result = 0;
max_value = 1;
MMMCMXCIX
        ^ value = 10;   max_value = 10;   flag = 0;  result += value;
	   ^  value = 1;    max_value = 10;   flag = 1;  result -= value;
	  ^   value = 100;  max_value = 100;  flag = 0;  result += value;
	 ^    value = 10;   max_value = 100;  flag = 1;  result -= value;
	^     value = 1000; max_value = 1000; flag = 0;  result += value;
   ^      value = 100;  max_value = 1000; flag = 1;  result -= value;
  ^       value = 1000; max_value = 1000; flag = 0;  result += value;
 ^        value = 1000; max_value = 1000; flag = 0;  result += value;
^         value = 1000; max_value = 1000; flag = 0;  result += value;
*/

	for (int i = strlen(roman) - 1; i >= 0; i--) {
		unsigned int value = find_value_by_letter(roman[i]);
		if (!value) {
			printf("Invalid character: %c\n", roman[i]);
			return 0;
		}

		if (value < max_value) { // little letter (Rule of 3)
			if (value == (max_value / 10)) {
				if (!flag) {
					result -= value;
					flag = true;
				}
				else { // if little letter repeats 2+ times -> error
					log("Invalid number\n");
					return 0;
				}
			}
			else { // you can't do VX
				log("Invalid number\n");
				return 0;
			}
		}
		else {
			max_value = value;
			result += value;
			flag = false;
		}
	}

	return result;
}

char* arabian2roman(unsigned int arabian) { // Перевод из арабской в римскую с/с
	if (arabian > 4000) {
		log("You can't convert numbers bigger than 4000 to Roman number system\n");
		return NULL;
	}

	char* result = NULL;

	unsigned int roman[sizeof(alphabet)] = { 0 }; // Здесь будет содержаться количество каждой цифры из алфавита римской с/с
	unsigned int roman_size = 0;                  // Размер выходной строки

	if (mode == MODE_RuleOf4) { // Работаем по правилу четырёх
		// Заполняем массив
		for (int i = 0; i < sizeof(alphabet); i++) {
			roman[i] = arabian / alphabet_values[i];
			arabian = arabian % alphabet_values[i];

			roman_size += roman[i]; // IIII
		}

		// Выделяем память для строчки (включая NULL-терминатор в конце)
		result = (char*)malloc(roman_size + 1);
		if (!result) {
			log("Unable to allocate memory for the roman number\n");
			return NULL;
		}

		// Заполняем строчку
		int counter = 0;

		for (unsigned int i = 0; i < sizeof(alphabet); i++) {
			for (unsigned int j = 0; j < roman[i]; j++) {
				result[counter] = alphabet[i];
				counter++;
			}
		}
		result[counter] = 0;
	}
	else { // Работаем по правилу трёх
		log("Sorry, Rule of 3 is not supporting\n");
		return NULL; // TODO
	}
	
	return result;
}

void print_commands() {
	log("Commands:\n");
	log("\t1. Convert roman to arabican\n");
	log("\t2. Convert arabican to roman\n");
	log("\t3. Print convert mode\n");
	log("\t4. Change convert mode\n");
	log("\t5. Print current session\n");
	log("\t6. Load session from file\n");
	log("\t7. Save current session\n");
	log("\t8. Clear current session\n");
	log("\t9. Exit\n");
}

void print_modes() {
	log("\t3. Convert by the Rule of 3\n");
	log("\t4. Convert by the Rule of 4\n");
}

void print_session() {
	unsigned int offset = 0;

	log("Session commands:\n");

	if (!session || !session_size) {
		log("Session is empty\n");
		return;
	}

	while (offset < session_size) {
		Command command = *(Command*)(session + offset);
		offset += sizeof(command);

		if (command == CMD_ArabianToRoman) {
			Action_RomanToArabian data = *(Action_RomanToArabian*)(session + offset);
			offset += sizeof(data);

			printf("Converted from arabian %u to roman %s\n", data.arabian, data.roman);
		}
		else if (command == CMD_RomanToArabian) {
			Action_ArabianToRoman data = *(Action_ArabianToRoman*)(session + offset);
			offset += sizeof(data);

			printf("Converted from roman %s to arabian %u\n", data.roman, data.arabian);
		}
		else if (command == CMD_ChangeMode) {
			Action_ChangeMode data = *(Action_ChangeMode*)(session + offset);
			offset += sizeof(data);

			printf("Converter mode changed to %u\n", data.mode);
		}
		else if (command == CMD_LoadSession) {
			Action_LoadSession data = *(Action_LoadSession*)(session + offset);
			offset += sizeof(data);

			log("Load session from ");
			log(data.path);
			log("\n");
		}
		else if (command == CMD_SaveSession) {
			Action_SaveSession data = *(Action_SaveSession*)(session + offset);
			offset += sizeof(data);

			log("Save session to ");
			log(data.path);
			log("\n");
		}
		else { // Command without data
			if (command == CMD_PrintMode)
				log("Print mode\n");
			else if (command == CMD_PrintSession)
				log("Print session\n");
			else if (command == CMD_ClearSession)
				log("Clear session\n");
			else if (command == CMD_Exit)
				log("Exit\n");
			else {
				printf("Unknown command %d\n", command);
				break;
			}
		}
	}
}

int main() {
	int command; // Команда, которую нужно выполнить
	int scanned; // Сколько полей обработал scanf
	int err = 0; // Переменная для хранения кода ошибки

	log(NAME ", v" VERSION "\n");
	log(DESCRIPTION        "\n");
	log("Author: " AUTHOR "\n");
	log("\n");

	log_file = fopen("Converter.log", "a");
	if (!log_file)
		log("Unable to open the log file\n");

	while (!err) {  // Цикл обработки команд
		print_commands();

		log("Please type the command:\n");

		scanned = scanf("%d", &command); // Ввод команды
		if (scanned != 1) {              // Проверить, вдруг введено неверно
			log("Error: invalid command\n"); // Вывести сообщение об ошибке
			/*
				Следующая строчка нужна для очистки потока ввода от неправильных данных
				Например, если вы ввели "jgvuyf", то scanf не считает число из потока ввода (stdin)
				Однако эта строчка "jgvuyf" останется в потоке ввода и будет считываться stdin при каждой итерации цикла
				Именно поэтому нужно применить rewind, дабы забыть неверные данные как страшный сон
			*/
			rewind(stdin);
			continue;
		}

		if (command == CMD_RomanToArabian) {
			Action_RomanToArabian data;

			log("Please enter the roman number: ");

			scanned = scanf("%256s", data.roman); // Ввод команды
			if (scanned != 1) {
				log("Error: invalid number\n"); // Вывести сообщение об ошибке
				continue;
			}

			data.arabian = roman2arabian(data.roman);
			if (!data.arabian) {
				log("Unable to convert this number\n");
				continue;
			}

			printf("Number in arabian numeric system is: %u\n", data.arabian);

			add_command_to_session(command, &data, sizeof(data));
		}
		else if (command == CMD_ArabianToRoman) {
			Action_ArabianToRoman data;

			log("Please enter the arabian number: ");

			scanned = scanf("%u", &(data.arabian));
			if (scanned != 1) {
				log("Error: invalid number\n"); // Вывести сообщение об ошибке
				continue;
			}

			log("Converting...\n");

			char* result = arabian2roman(data.arabian);
			if (!result) {
				log("Unable to convert this number\n");
				continue;
			}

			log("Number in roman numeric system is: ");
			log(result);
			log("\n");

			memcpy(data.roman, result, strlen(result) + 1);
			free(result);

			add_command_to_session(command, &data, sizeof(data));
		}
		else if (command == CMD_ChangeMode) {
			Action_ChangeMode data;

			print_modes();

			log("Please type the mode: ");

			scanned = scanf("%u", &mode);
			if (scanned != 1) {
				log("Error: invalid number\n"); // Вывести сообщение об ошибке
				continue;
			}

			if (mode != MODE_RuleOf3 && mode != MODE_RuleOf4) {
				mode = MODE_RuleOf4;
				log("Invalid mode. Setted 4 (default mode)\n");
				continue;
			}
			data.mode = mode;

			log("Mode setted successfully\n");

			add_command_to_session(command, &data, sizeof(data));
		}
		else if (command == CMD_PrintMode) {
			printf("Mode is %u\n", mode);

			add_command_to_session(command, NULL, 0);
		}
		else if (command == CMD_PrintSession) {
			print_session();

			add_command_to_session(command, NULL, 0);
		}
		else if (command == CMD_LoadSession) {
			Action_LoadSession data;

			log("Please enter the session file name: ");

			scanned = scanf("%256s", data.path); // Ввод команды
			if (scanned != 1) {
				log("Error: invalid number\n"); // Вывести сообщение об ошибке
				continue;
			}

			load_session(data.path);

			add_command_to_session(command, (char*)(&data), sizeof(data));
		}
		else if (command == CMD_SaveSession) {
			Action_SaveSession data;

			log("Please enter the session file name: ");

			scanned = scanf("%256s", data.path); // Ввод команды
			if (scanned != 1) {
				log("Error: invalid number\n"); // Вывести сообщение об ошибке
				continue;
			}

			save_session(data.path);

			add_command_to_session(command, (char*)(&data), sizeof(data));
		}
		else if (command == CMD_ClearSession) {
			if (session) {
				free(session);
				session = NULL;
			}

			session_size = 0;

			add_command_to_session(command, NULL, 0);
		}
		else if (command == CMD_Exit) {      // Выход
			add_command_to_session(command, NULL, 0);

			break;
		}
		else {
			log("Error: unknown command\n"); // Неизвестная команда
			continue;
		}
	}

	log("Good bye!\n");

	if (log_file)
		fclose(log_file);
}