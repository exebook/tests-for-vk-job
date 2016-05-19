/*
	Original task description:
	
	5. Вы должны написать следующую программу на языке C: Вашей программе в качестве аргумента будет передано имя файла, в котором будет находиться множество строк (ASCII символы с кодами от 32 до 127). Далее Ваша программа должна считывать строки из stdin, пока не придет строка "exit". После каждой считанной строки Ваша программа должна выводить в stdout строку "YES" или "NO" в зависимости от
	того, встречается данная строка в переданном файле или нет. Размер файла со словарем не превосходит 128мб. Напишите как можно более эффективное решение и будьте готовы объяснить, почему Вы выбрали именно это решение. В качестве ответа пришлите ссылку на репозиторий с вашей программой и makefile'ом. Программа должна
	компилироваться GCC 4.7.2. 
	
	Some notes before the code:
	
	1) File format is not described, so '\n' delimited assumed.
	2) "Most effective" is not formal, so speed-effective assumed.
	3) Whether dictionary loading time is critical is not clear.
	4) Hard to make assumptions of the nature of the dictionary entries:
		a) how random is the distribution of the keys?
		b) are they numbers or English text or binary data or something else?
		c) what is the expected length of a key?
		knowing the above could help to make the solution more effective.

*/


#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "hashdict.h"

int time1000() {
	struct timeval val;
	gettimeofday(&val, 0);
	val.tv_sec &= 0xffff;
	return val.tv_sec * 1000 + val.tv_usec / 1000;
}

/*
	Context structure is passed around to avoid global variables.
*/

struct context {
	/*
		This is the hash table based dictionary.
	*/
	struct dictionary *dict;

	/*
		detect if stdin is a terminal or a pipe.
	*/
	int terminal; 
};

/*
	Color output for the convenience of the user if terminal is detected as stdin.
*/
void color(int index) {
	if (index < 0) printf("%c(B%c[m", 27, 27);
	else printf("%c[38;5;%im", 27, index);
}

/*
	Process dictionary input buffer, tokenize and insert each key into the hash table.
*/

void tokenize(struct context *ctx, char *s, int size) {
	char *end = s + size;
	char *key;
	int n = 0;
	/*
		This loop can take few seconds on 1M - 10M keys (words).
	*/
	while (s < end) {
		key = s;
		while (s < end && *s != '\n') s++;
		*s = 0;
		if (key != s) {
			/*
				We do not even need to associate the value with the key.
			*/
			dic_add(ctx->dict, key, s-key);
			n++;
		}
		s++;
	}
}

/*
	Load bytes from the disk, check for errors.
	Return buffer with data and set size in *rd.
*/

char* load_file(struct context *ctx, char *name, int *rd) {
	char *s;
	FILE *f = fopen(name, "r");
	int size;

	if (f) {
		/*
			First, get the size of the file to allocate the buffer.
		*/
		fseek(f, 0, 2);
		size = ftell(f);
		fseek(f, 0, 0);
	}
	else {
		fprintf(stderr, "fopen() failed\n");
		exit(1);
	}
	if (ctx->terminal) color(3), printf("INPUT"), color(-1), printf(" %s\n", name);
	if (ctx->terminal) color(3), printf("LOADED"), color(-1), printf(" %i\n", size);

	s = malloc(size);
	if (s == 0) {
		fprintf(stderr, "malloc() failed\n");
		exit(1);
	}
	*rd = fread(s, 1, size, f);
	fclose(f);
	
	return s;
}

/*
	This is very fast lookup, and result output.
*/
void lookup_word(struct context *ctx, char *word, int count) {
	if (dic_find(ctx->dict, word, count)) {
		if (ctx->terminal) color(2);
		printf("YES\n");
		if (ctx->terminal) color(-1);
	}
	else {
		if (ctx->terminal) color(1);
		printf("NO\n");
		if (ctx->terminal) color(-1);
	}
	if (ctx->terminal) printf("> ");
}

/*
	School-book GNU getline() loop to read from stdin.
	If it does not work for you, you must be on Windows?
*/

void process_input(struct context *ctx) {
	char *l = 0;
	size_t rd1 = 0;
	size_t rd2 = 0;
	int control_d = 1;
	
	while ((rd2 = getline(&l, &rd1, stdin)) != -1) {
		if (l[rd2-1] == '\n') l[rd2-1] = 0, rd2--;
		/*
			We only handle single command: lowercase "exit"
		*/
		if (!memcmp("exit", l, 4)) {
			control_d = 0;
			break;
		}
		lookup_word(ctx, l, rd2);
	}
	free(l);
	
	if (ctx->terminal) {
		/*
			Exit nicely.
		*/
		if (control_d) printf("exit\n");
		color(4), printf("EXIT\n"), color(-1);
	}
}

int main(int argc, char **argv) {
	/*
		Initialization:
			1) Check for the required argument.
			2) Allocate the context structure.
			3) Set the timer checkpoint.
			4) Test if stdin is a terminal.
			5) Initialize hash table.
	*/
	if (argc < 2) {
		fprintf(stderr, "no input\n");
		exit(1);
	}
	struct context *ctx = malloc(sizeof(struct context));
	int load_time = time1000();
	ctx->terminal = isatty(fileno(stdin));
	ctx->dict = dic_new(0);
	
	/*
		Processing:
			1) Load from file.
			2) Tokenize and insert into the hash table dictionary.
	*/
	int rd;
	char *s = load_file(ctx, argv[1], &rd);
	tokenize(ctx, s, rd);

	/*
		Original buffer data is no longer needed, free.
	*/
	free(s);
	
	/*
		Print some debugging information.
	*/
	if (ctx->terminal) {
		color(3), printf("ENTRIES"), color(-1), printf(" %i\n", ctx->dict->count);
		color(3), printf("LOADTIME"), color(-1), printf(" %i\n", time1000() - load_time);
		printf("> ");
	}
	
	/*
		Start main loop.
	*/
	process_input(ctx);
	
	/*
		Dealocate resources and exit gracefully.
	*/
	dic_delete(ctx->dict);
	free(ctx);
}

