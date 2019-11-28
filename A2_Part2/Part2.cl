/* convert string to uppercase */
__kernel void toUpper (__global char *content, // index 0
					   __global char *ucontent) // index 1
{
	int id = get_global_id (0);
	// check that the letter is lowercase
	if (content [id] >= 97 && content[id] <= 122)
	{
		// convert the letter to uppercase
		ucontent [id] = content [id] - 32;
	}
	else
	{
		// remain as uppercase
		ucontent [id] = content [id];
	}
}

/* encrypt the plaintext in parallel */
__kernel void encrypt (__global char *content, // index 0
					   __global char *output,  // index 1
					   __global int *n)			   // index 2
{
	int shift = *n; // copy the n value locally
	int letter = 0; // to check that letter is not out of bounds
	int i = get_global_id (0);
	
	if (shift > 0) // if n is positive
	{
		if (content[i] >= 65 && content[i] <= 90)
		{
			letter = content[i] + shift;
			
			if (letter > 90)
			{
				// e.g. 91->65, 92->66, 93->67
				letter = letter - 26;
			}

			output[i] = letter;
		}
	}
	else // if n is negative
	{
		if (content[i] >= 65 && content[i] <= 90)
		{
			// e.g. 65 + (-1) = 65 - 1
			letter = content[i] + shift;
			
			if (letter < 65) 
			{
				// e.g. 63->89, 64->90
				letter = letter + 26;
			}

			output[i] = letter;
		}
	}
}

/* convert string to lowercase */
__kernel void toLower (__global char *content, // index 0
					   __global char *ucontent) // index 1
{
	int id = get_global_id (0);
	// check that the letter is uppercase
	if (content [id] >= 65 && content[id] <= 90)
	{
		// convert the letter to lowercase
		ucontent [id] = content [id] + 32;
	}
	else
	{
		// remain as lowercase
		ucontent [id] = content [id];
	}
}

/* decrypt the plaintext in parallel */
__kernel void decrypt (__global char *content, // index 0
					   __global char *output,  // index 1
					   __global int *n)			   // index 2
{
	int shift = *n; // copy the n value locally
	int letter; // to check that letter is not out of bounds
	int i = get_global_id (0);
	if (shift > 0) // if n is positive
	{
		if (content[i] >= 65 && content[i] <= 90)
		{
			letter = content[i] - shift;
			
			if (letter < 65)
			{
				letter = letter + 26;
			}

			output[i] = letter;
		}
	}
	else // if n is negative
	{
		if (content[i] >= 65 && content[i] <= 90)
		{
			// e.g. 65 - (-1) = 65 + 1
			letter = content[i] - shift;
			
			if (letter > 90)
			{
				letter = letter - 26;
			}

			output[i] = letter;
		}
	}
}

/* encrypt the plaintext based on lookup table in parallel */
__kernel void encryptLT (__global char *content, // index 0
					   __global char *output)    // index 1
{
	
	int i = get_global_id (0);
	
	// lookup table
	switch (content[i])
	{
		case 'a':	
		case 'A':	output[i] = 'G';
					break;
		case 'b':
		case 'B':	output[i] = 'X';
					break;
		case 'c':	
		case 'C':	output[i] = 'S';
					break;
		case 'd':	
		case 'D':	output[i] = 'Q';
					break;
		case 'e':	
		case 'E':	output[i] = 'F';
					break;
		case 'f':	
		case 'F':	output[i] = 'A';
					break;
		case 'g':	
		case 'G':	output[i] = 'R';
					break;
		case 'h':	
		case 'H':	output[i] = 'O';
					break;
		case 'i':	
		case 'I':	output[i] = 'W';
					break;
		case 'j':	
		case 'J':	output[i] = 'G';
					break;
		case 'k':
		case 'K':	output[i] = 'L';
					break;
		case 'l':
		case 'L':	output[i] = 'M';
					break;
		case 'm':
		case 'M':	output[i] = 'T';
					break;
		case 'n':
		case 'N':	output[i] = 'H';
					break;
		case 'o':
		case 'O':	output[i] = 'C';
					break;
		case 'p':
		case 'P':	output[i] = 'V';
					break;
		case 'q':
		case 'Q':	output[i] = 'P';
					break;
		case 'r':
		case 'R':	output[i] = 'N';
					break;
		case 's':
		case 'S':	output[i] = 'Z';
					break;
		case 't':
		case 'T':	output[i] = 'U';
					break;
		case 'u':
		case 'U':	output[i] = 'I';
					break;
		case 'v':
		case 'V':	output[i] = 'E';
					break;
		case 'w':
		case 'W':	output[i] = 'Y';
					break;
		case 'x':
		case 'X':	output[i] = 'D';
					break;
		case 'y':
		case 'Y':	output[i] = 'K';
					break;
		case 'z':
		case 'Z':	output[i] = 'J';
	}
}

/* decrypt the plaintext based on lookup table in parallel */
__kernel void decryptLT (__global char *content, // index 0
					   __global char *output)    // index 1
{
	
	int i = get_global_id (0);
	
	// lookup table
	switch (content[i])
	{	
		case 'G':	output[i] = 'a';
					break;
		case 'X':	output[i] = 'b';
					break;
		case 'S':	output[i] = 'c';
					break;
		case 'Q':	output[i] = 'd';
					break;
		case 'F':	output[i] = 'e';
					break;	
		case 'A':	output[i] = 'f';
					break;
		case 'R':	output[i] = 'g';
					break;
		case 'O':	output[i] = 'h';
					break;
		case 'W':	output[i] = 'i';
					break;
		case 'B':	output[i] = 'j';
					break;
		case 'L':	output[i] = 'k';
					break;
		case 'M':	output[i] = 'l';
					break;
		case 'T':	output[i] = 'm';
					break;
		case 'H':	output[i] = 'n';
					break;
		case 'C':	output[i] = 'o';
					break;
		case 'V':	output[i] = 'p';
					break;
		case 'P':	output[i] = 'q';
					break;
		case 'N':	output[i] = 'r';
					break;
		case 'Z':	output[i] = 's';
					break;
		case 'U':	output[i] = 't';
					break;
		case 'I':	output[i] = 'u';
					break;
		case 'E':	output[i] = 'v';
					break;
		case 'Y':	output[i] = 'w';
					break;
		case 'D':	output[i] = 'x';
					break;
		case 'K':	output[i] = 'y';
					break;
		case 'J':	output[i] = 'z';
	}
}