#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
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
		log("Unable to open session file");
		return;
	}

	unsigned int offset = 0;
	unsigned int readed = 0;

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
	session = new_session;

	while (offset < session_size) {
		Command command;

		readed = fread(&command, sizeof(command), 1, session_file);
		if (readed != 1) {
			log("Unable to read command from the file\n");
			break;
		}

		if (command < 0 || command > CMD_Exit) {
			log("Error while saving session: unknown command\n");
			break;
		}

		*(Command*)(session + offset) = command; // Записываем команду в память
		offset += sizeof(command);

		if (command == CMD_RomanToArabian) {
			readed = fread(session + offset, sizeof(Action_1), 1, session_file);
			if (readed != 1) {
				log("Unable to read action (1) from the file\n");
				break;
			}
			offset += sizeof(Action_1);
		}
		else if (command == CMD_ArabianToRoman) {
			readed = fread(session + offset, sizeof(Action_2), 1, session_file);
			if (readed != 1) {
				log("Unable to read action (2) from the file\n");
				break;
			}
			offset += sizeof(Action_2);
		}
		else if (command == CMD_LoadSession) {
			readed = fread(session + offset, sizeof(Action_4), 1, session_file);
			if (readed != 1) {
				log("Unable to read action (4) from the file\n");
				break;
			}
			offset += sizeof(Action_4);
		}
		else if (command == CMD_SaveSession) {
			readed = fread(session + offset, sizeof(Action_5), 1, session_file);
			if (readed != 1) {
				log("Unable to read action (5) from the file\n");
				break;
			}
			offset += sizeof(Action_5);
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
}

void save_session(char* path) {
	FILE* session_file = fopen(path, "wb");
	if (!session_file) {
		log("Unable to open session file");
		return;
	}

	unsigned int offset = 0;

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
			fwrite((Action_1*)(session + offset), 1, sizeof(Action_1), session_file);
			offset += sizeof(Action_1);
		}
		else if (command == CMD_ArabianToRoman) {
			fwrite((Action_2*)(session + offset), 1, sizeof(Action_2), session_file);
			offset += sizeof(Action_2);
		}
		else if (command == CMD_LoadSession) {
			fwrite((Action_4*)(session + offset), 1, sizeof(Action_4), session_file);
			offset += sizeof(Action_4);
		}
		else if (command == CMD_SaveSession) {
			fwrite((Action_5*)(session + offset), 1, sizeof(Action_5), session_file);
			offset += sizeof(Action_5);
		}
	}

	fclose(session_file);

	if (offset != session_size)
		printf("Error: was writed not all session (%u of %u)\n", offset, session_size);
}

unsigned int roman2arabian(const char* roman) { // Перевод из римской в арабскую с/с
	// TODO: need to implement dat shit in the future
	log("This don't work, sorry\n");
	return 0;
}

char* arabian2roman(unsigned int arabian) { // Перевод из арабской в римскую с/с
	if (arabian > 4000) {
		log("You can't convert numbers bigger than 4000 to Roman number system\n");
		return NULL;
	}

	unsigned int roman[sizeof(alphabet)] = { 0 }; // Здесь будет содержаться количество каждой цифры из алфавита римской с/с
	unsigned int roman_size = 0;                  // Размер выходной строки

	// Заполняем массив
	for (int i = 0; i < sizeof(alphabet); i++) {
		roman[i] = arabian / alphabet_values[i];
		arabian   = arabian % alphabet_values[i];

		roman_size += roman[i];
	}

	// Выделяем память для строчки (включая NULL-терминатор в конце)
	char* result = (char*)malloc(roman_size + 1);
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
	
	return result;
}

void print_commands() {
	log("Please type the command:\n");
	log("\t1. Convert roman to arabican\n");
	log("\t2. Convert arabican to roman\n");
	log("\t3. Print current session\n");
	log("\t4. Load session from file\n");
	log("\t5. Save current session\n");
	log("\t6. Clear current session\n");
	log("\t7. Exit\n");
}

void print_session() {
	unsigned int offset = 0;

	log("Session commands:\n");

	while (offset < session_size) {
		Command command = *(Command*)(session + offset);
		offset += sizeof(command);

		if (command == CMD_ArabianToRoman) {
			Action_1 data = *(Action_1*)(session + offset);
			offset += sizeof(data);

			printf("Converted from arabian %u to roman %s\n", data.arabian, data.roman);
		}
		else if (command == CMD_RomanToArabian) {
			Action_2 data = *(Action_2*)(session + offset);
			offset += sizeof(data);

			printf("Converted from roman %s to arabian %u\n", data.roman, data.arabian);
		}
		else if (command == CMD_LoadSession) {
			Action_4 data = *(Action_4*)(session + offset);
			offset += sizeof(data);

			log("Load session from ");
			log(data.path);
			log("\n");
		}
		else if (command == CMD_SaveSession) {
			Action_5 data = *(Action_5*)(session + offset);
			offset += sizeof(data);

			log("Save session from ");
			log(data.path);
			log("\n");
		}
		else { // Command without data
			if (command == CMD_PrintSession)
				log("Print session\n");
			else if (command == CMD_ClearSession)
				log("Clear session\n");
			else if (command == CMD_Exit)
				log("Exit\n");
			else
				printf("Unknown command %d\n", command);
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
			Action_1 data;

			log("Please enter the roman number: ");

			scanned = scanf("%256s", data.roman); // Ввод команды
			if (scanned != 1) {
				log("Error: invalid number\n"); // Вывести сообщение об ошибке
				continue;
			}

			unsigned int result = roman2arabian(data.roman);
			if (!result) {
				log("Unable to convert this number\n");
				continue;
			}

			data.arabian = result;

			add_command_to_session(command, &data, sizeof(data));
		}
		else if (command == CMD_ArabianToRoman) {
			Action_2 data;

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
		else if (command == CMD_PrintSession) {
			print_session();

			add_command_to_session(command, NULL, 0);
		}
		else if (command == CMD_LoadSession) {
			Action_4 data;

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
			Action_5 data;

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