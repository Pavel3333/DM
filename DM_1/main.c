#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME        "Number Converter"
#define DESCRIPTION "This program can convert numbers between romean and arabian number systems"
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

// Коды команд
typedef enum {
	CMD_RomeanToArabian = 1,
	CMD_ArabianToRomean = 2,
	CMD_SaveSession     = 3,
	CMD_Exit            = 4
} Command;

FILE* log_file = NULL;

void log(const char* data) { // Журналирование в файл всего, что мы выводим
	if (!data) return;

	if (log_file)
		fwrite(data, 1, strlen(data), log_file);

	printf("%s", data);
}

char* romean2arabian(const char* romean) { // Перевод из римской в арабскую с/с
	// TODO: need to implement dat shit in the future
	log("This don't work, sorry\n");
	return NULL;
}

char* arabian2romean(unsigned int arabian) { // Перевод из арабской в римскую с/с
	if (arabian > 4000) {
		log("You can't convert numbers bigger than 4000 to Romean number system\n");
		return NULL;
	}

	unsigned int romean[sizeof(alphabet)] = { 0 }; // Здесь будет содержаться количество каждой цыфры из алфавита римской с/с
	unsigned int romean_size = 0; // Размер выходной строки

	// Заполняем массив
	for (int i = 0; i < sizeof(alphabet); i++) {
		romean[i] = arabian / alphabet_values[i];
		arabian   = arabian % alphabet_values[i];

		romean_size += romean[i];
	}

	// Выделяем память для строчки (включая NULL-термир=натор в конце)
	char* result = (char*)malloc(romean_size + 1);
	if (!result) {
		log("Unable to allocate memory for the romean number\n");
		return NULL;
	}

	// Заполняем строчку
	int counter = 0;

	for (int i = 0; i < sizeof(alphabet); i++) {
		for (int j = 0; j < romean[i]; j++) {
			result[counter] = alphabet[i];
			counter++;
		}
	}
	result[counter] = 0;
	
	return result;
}

void print_commands() {
	log("Please type the command:\n");
	log("\t1. Convert romean to arabican\n");
	log("\t2. Convert arabican to romean\n");
	log("\t3. Save current session\n");
	log("\t4. Exit\n");
}

int main() {
	int command; // Команда, которую нужно выполнить
	int scanned; // Сколько полей обработал scanf
	int err = 0; // Переменная для хранения кода ошибки

	log(NAME ", v" VERSION "\n");
	log(DESCRIPTION "\n");
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

		if (command == CMD_RomeanToArabian) {
			// TODO: need to implement dat shit in the future
		}
		else if (command == CMD_ArabianToRomean) {
			unsigned int number;

			log("Please enter the arabian number: ");

			scanned = scanf("%u", &number);
			if (scanned != 1) {
				log("Error: invalid number\n"); // Вывести сообщение об ошибке
				continue;
			}

			log("Converting...\n");

			char* result = arabian2romean(number);
			if (!result) {
				log("Unable to convert this number\n");
				continue;
			}

			log("Number in romean numeric system is: ");
			log(result);
			log("\n");
		}
		else if (command == CMD_SaveSession) {
			// TODO: implement dat shit
		}
		else if (command == CMD_Exit)   // Выход
			break;
		else {
			log("Error: unknown command\n"); // Неизвестная команда
			continue;
		}
	}

	log("Good bye!\n");
}