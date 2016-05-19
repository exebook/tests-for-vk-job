#include <stdio.h>

/*
	Binary search in nondecreasing source,
	the result is the index of the first value that is
	guaranteed to be larger than X or -1 if such value was not found.
	
	Notice that task states that only two arguments are mandatory, which
	I cannot understand, because binary search requires at least three:
		1) array pointer
		2) array size
		3) the value to search for

	Task descriptionas given on VK.COM job application page:
	
	4. Написать функцию bsearch, которая принимает на вход отсортированный по неубыванию массив целых чисел и целое число x. В качестве результата функция должна возвращать индекс первого элемента массива, строго большего x. В Вашей реализации функция может принимать любое количество любых параметров, только 2
	этих параметра обязательны. При использовании Вашей функции должна быть возможность определить, существует ответ или нет. В качестве решения пришлите ссылку на репозиторий с программой на языке C с примером использования функции. 

*/

/*
	Reference brute-force search for tests.
*/

int search(int *arr, int size, int X) {
	int c = 0;
	while (c < size) {
		if (arr[c] > X) return c;
		c++;
	}
	return -1;
}

/*
	Main function. This is just a test task, not a production ready code.

	Writing such function to be production ready will require few more hours to
	systematicaly test for performance and correctness.

	This code is written empirically and tested against the bullet-proof reference brute-force
	algorithm for a few manually selected cases. The good way to fully test it for correctness
	would be probably to feed it with millions of random (think Monte-Carlo) inputs and
	compare the output to the reference function.
*/

int bsearch(int *arr, int size, int X) {
	/*
		Some border checks first. Order of checks matters.
	*/
	if (size == 0) return -1;
	
	/*
		Check the first item.
	*/
	if (arr[0] > X) return 0;
	if (size == 1) return -1;
	
	/*
		Check the last item.
	*/
	if (arr[size - 1] <= X) return -1;
	
	/*
		low - lower limit, hi - upper limit, cur - currently inspected value
	*/
	int low, hi, cur;
	low = 0;
	hi = size - 1;
	cur = (low+hi)/2;

	while (low <= hi) {
		/*
			School-book standard binary search algorithm, with a slight
			modification because we are not looking for the exact match but for a "first larger".
		*/
		if (arr[cur] < X) {
			low = cur + 1;
		}
		else if (arr[cur] == X) {
			/*
				When X was found, skip duplicate X values if present and return,
				nothing else we can do except just iterating one-by-one.
			*/
			for (;cur <= size; cur++) {
				if (arr[cur] > X) {
					return cur;
				}
			}
			return -1;
		}
		else {
			hi = cur - 1;
		}

		cur = (low + hi)/2;
	}
	if (low > hi) {
		/*
			X was not found, and now, since we do not know at this point
			whether the last step was to change hi or low, we need these two extra checks.
		*/
		if (arr[hi] > X) return hi;
		else if (arr[low] > X) return low;
		else return low + 1;
	}
	return cur;
}

int test(int *arr, int size, int X) {
	int ix, ix2;
	ix = bsearch (arr, size, X);
	ix2 = search (arr, size, X);
	if (ix != ix2) {
		printf("must be %i, but got wrong result: X=%i, arr[%i] = %i\n", ix2, X, ix, arr[ix]);
		return 1;
	}
	return 0x1000;
}

int main() {
	int item;
	int errors = 0;
	int ten[10] = {1,1,3,3,3,3,3,3,3,12};
	/*
		Try to test some known edge cases.
	*/
	errors += test(ten, 0, 0);
	errors += test(ten, 1, 0);
	errors += test(ten, 1, 1);
	errors += test(ten, 1, 4);
	errors += test(ten, 10, 0);
	errors += test(ten, 10, 3);
	errors += test(ten, 10, 6);
	errors += test(ten, 10, 12);
	errors += test(ten, 10, 20);
	printf("Tests passed: %i, tests failed: %i\n", errors >> 12, errors & 0xfff);
	return 0;
}
