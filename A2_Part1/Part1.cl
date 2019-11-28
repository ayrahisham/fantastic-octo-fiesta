/* Accepts array 1 as an array of int4s, array 2 and an output array */
__kernel void copy (__global int4 *array1, // index 0
					__global int *array2, // index 1
					__global int *output) // index 2
{
	const int size8 = 8; // const array size
	int index = get_global_id (0);

	int8 v = (int8) (array1[0], array1[1]);

	// Copy the contents of array 2 into two int8 vectors called v1 and v2 (using vloadn)

	int8 v1 = vload8 (index, array2);
	
	int8 v2 = vload8 (index, array2+8);

	// Creates an int8 vector in private memory called results
	int8 results;
	
	// Check whether any of the elements in v are greater than 5
	int8 mask;
	bool ok = false;
	for (int j = 0; j < size8; j++)
	{
		// If there are, then for elements that are greater than 5
		if (v[j] > 5)
		{
			ok = true;
			break;
		}
	}
	if (ok == true)
	{
		for (int j = 0; j < size8; j++)
		{
			// for elements that are greater than 5
			if (v1[j] > 5)
			{
				// copy the corresponding elements from v1
				mask [j] = 0;
			}
			// for elements less than 5 in v1
			else if (v1[j] < 5)
			{
				// copy the elements from v2
				mask [j] = -1;
			}
		}

		results = select (v1, v2, mask);
	}
	// If not, 
	else
	{
		// fill the first 4 elements with the contents from the first 4 elements of v1 
		for (int k = 0; k < size8/2; k++)
		{
			results [k] = v1 [k];
		}
		// and the next 4 elements with contents from the first 4 elements of v2
		int i = 0;
		for (int k = 4; k < size8; k++)
		{
			results [k] = v2 [i];
			++i;
		}
	}
	// Stores the contents of v, v1, v2 and results in the output array (using vstoren)
	vstore8 (results, 0, output);
} 
