/* Store the passcode as a global variable in the .cl file
Write a function (not a kernel) called checkPasscode in the .cl file
o The function should accept an entire passcode attempt
- NOTE: This function is to check whether all digits in the attempt match the
stored passcode
o Check whether this attempt matches the stored passcode
o Returns true if it matches, or false otherwise */
bool checkPasscode (__global const int *passcode, // index 0
					unsigned const int n,		  // index 1
					int *attempt)			      // index 1
{
	// iterate through all digits in the passcode
	for (int i = 0; i < n; i++)
	{
		if (attempt [i] != passcode [i])
		{
			return false;
		}
	}

	return true;
}

/* Your program should search through the space of (10^8)
all possible 8-digit passcodes to find a match with the stored passcode
o Write an OpenCL kernel that will call the checkPasscode function
o Display information about the work-item that found the passcode (i.e. its global id,
work-group id and id within the work-group) */
__kernel void searchMatch (__global int *passcode, // index 0
						   __global int *n,  // index 1
						   __global int *attempt) // index 2
						   
{
	int nValue = *n;
	bool match = false;

	/* Access work-item/work-group information */
	size_t global_id_0 = get_global_id (0);
	size_t local_id_0 = get_local_id (0);
	size_t group_id_0 = get_group_id (0);
	size_t global_size_0 = get_global_size (0);

	int keyspace = 100000000; // 10^8
	int size = keyspace/global_size_0;

	int begin = global_id_0 * size;
	int end = (global_id_0+1) * size;

	/* all possible 8-digit passcodes */
	int possiblePass [8] = {0, 0, 0, 0, 0, 0, 0, 0};

	int digit;
	for (int i = begin; i < end; i++)
	{
		digit = i;
		for (int j = 7; j > -1; j--)
		{
			possiblePass[j] = digit % 10; // get values from 0 to 9
			digit /= 10; // remove the last digit
		}
		
		match = checkPasscode (passcode, nValue, possiblePass);

		if (match)
		{
			attempt[0] = global_id_0;
			attempt[1] = local_id_0;
			attempt[2] = group_id_0;
		}
	}
}