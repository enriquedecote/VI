#include <iostream>
#include <time.h>

using namespace std;



void insertionSort(int numbers[], int array_size) {
	int i, j, index;
	for (i=1; i < array_size; i++) {
		index = numbers[i];
		j = i;
		while ((j > 0) && (numbers[j-1] > index)) {
			numbers[j] = numbers[j-1];
			j = j - 1;
		}
		numbers[j] = index;
	}
}

int main() {
	srand(time(NULL));
	/*int numbers[10];
	for (int i = 0; i < 10; i++) {
		numbers[i] = (rand()%50);
		cout << "numbers[" << i << "] = " << numbers[i] << endl;
	}
	insertionSort(numbers,10);
	cout<<endl;
	for (int i = 0; i < 10; i++) {
		cout << "numbers[" << i << "] = " << numbers[i] << endl;
	}*/
	float val;
	for (unsigned i=0; i < 10; i++) {
		val = (rand()%9+1);
		cout << "val = " << val << " -- 1/val = " << 1/val << endl;
	}
	return 0;
}
