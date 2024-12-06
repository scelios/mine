#include "nm.h"

void sort(t_sym *array, int size)
{

	for (int step = 1; step < size ; step++) 
	{
		int key = ft_strncmp(array[step].name, array[step - 1].name, 20);
		// printf("key: %d\n", key);
		int j = step - 1;

		while (j >= 0 && key < 0) 
		{
			t_sym temp = array[j];
			array[j] = array[j + 1];
			array[j + 1] = temp;
			j--;
			if (j >= 0) {
				key = ft_strncmp(array[j + 1].name, array[j].name,20);
			}
		}
		
	}
}
