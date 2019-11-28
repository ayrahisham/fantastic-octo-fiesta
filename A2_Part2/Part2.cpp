// Name: Nur Suhaira Bte Badrul Hisham
// Student ID: 5841549
// Assignment 2 Part 2

// Without this, the compiler will use the default OpenCL 2.x, 
// where some OpenCL 1.2 functions are deprecated.
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

// By default, OpenCL disables exceptions.This line enables it.
#define __CL_ENABLE_EXCEPTIONS

// The headers from the C++ standard library and STL that are used
#include <iostream>
#include <fstream> // For file reading
#include <ctime>    // For time()
#include <cctype>
#include <cstdlib>  // For srand() and rand()

// Including the OpenCL header. 
// Depending on which Operating System (OS) 
// we are using, this checks whether we are running the program on Mac OS 
// or other OS :
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

// Include a user defined header file :
#include "common.h"

// Having to type std:: and cl:: prefixes in front of the C++
// standard library or OpenCL functions
// using namespace std;
// using namespace cl;

bool checkFileOK (std::fstream&, std::string); // open file checking ok
void displayFileContents (std::fstream&, std::string, std::vector <cl_char>&); // store contents of file in string display
int getShiftValue (int); // get user input for shifting n position away
void encrypt (int, std::vector <cl_char>, std::vector <cl_char>&); // shift contents in n positions away (left/right)
// store the results in a vector
void displayCipher(std::vector <cl_char>); // display ciphertext to user
int countNoOfLetters(std::vector <cl_char>); // to get no. of work items needed
void displayPlaintext(std::vector <cl_char>, std::vector <cl_char>&); // only contains letters
void displayCipherK(std::vector <cl_char>); // encrypted text processed in kernel
void displayDCipherK(std::vector <cl_char>); // decrypted text processed in kernel

int main()
{
	cl::Platform platform;			// device's platform
	cl::Device device;				// device used
	cl::Context context;			// context for the device
	cl::Program program;			// OpenCL program object
	cl::Kernel kernel;				// a single kernel object
	cl::CommandQueue queue;			// commandqueue for a context and device

	// declare data and memory objects
	cl::Buffer outputBuffer;
	cl::Buffer contentsBuffer;
	cl::Buffer shiftBuffer;
	cl::Buffer pwdBuffer;

	std::fstream afile;
	std::string filename = "plaintext";
	std::vector <cl_char> content;
	std::vector <cl_char> ciphertext; // ciphertext processed in host
	cl_int n = 0; // n position to shift
	cl_int ciphersize = 0; // size of letters to be encrypted 

	try
	{
		std::cout << "\n============PART 2: Shift Cipher============\n" << std::endl;
		std::cout << "PART 2A:\n" << std::endl;
		
		// if file open OK
		if (checkFileOK(afile, filename))
		{
			// display file contents to user to aid in value n (only letters)
			displayFileContents(afile, filename, content);

			// make sure n value is non-zero
			n = getShiftValue(n);
			std::cin.ignore(20, '\n');

			// in this function, to compare n value whether it is positive or negative
			encrypt(n, content, ciphertext);

			// once encrypted in above function, this function enables to display the results
			displayCipher(ciphertext);
		}
		else
		{
			std::cout << filename << " opened for reading failed" << std::endl;
		}

		std::cout << "--------------------" << std::endl;
		std::cout << "\nPART 2B:\n" << std::endl;
		std::cout << "Using a kernel to encrypt and decrypt in parallel...\n" << std::endl;

		// select an OpenCL device
		if (!select_one_device(&platform, &device))
		{
			// if no device selected
			quit_program("Device not selected.");
		}

		// create a context from device
		context = cl::Context(device);

		// build the program
		if (!build_program(&program, &context, "Part2.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}

		// create command queue
		queue = cl::CommandQueue(context, device);
		std::cout << "Creating a command queue..." << std::endl;

		// create buffers
		// a buffer object where the data is intended to be read-only (CL_MEM_READ_ONLY) on the device and
		// CL_MEM_COPY_HOST_PTR indicates that the application wants the OpenCL implementation to allocate
		// memory for the memory object and copy the data from memory referenced by content and n
		std::cout << "Reading the contents from contents into local memory..." << std::endl;

		ciphersize = countNoOfLetters(content);
		std::cout << "No. of letters to encrypt: " << ciphersize << std::endl;

		std::vector <cl_char> plaintext(ciphersize); // plaintext that consists only letters
		displayPlaintext(content, plaintext);

		// Create a buffer for plaintext
		contentsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * ciphersize, &plaintext[0]);
		shiftBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &n);
		// This memory object is intended to be for read and write access on the device.
		// Since no host pointer was passed, the following only reserves memory :
		outputBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_char) * ciphersize);

		// create a kernel
		std::cout << "Creating a kernel to convert to uppercase..." << std::endl;
		kernel = cl::Kernel(program, "toUpper");

		// setting kernel arguments
		std::cout << "Setting kernel arguments for conversion..." << std::endl;
		kernel.setArg(0, contentsBuffer);
		kernel.setArg(1, outputBuffer);

		// enqueue kernel for execution
		// the kernel will execute on "ciphersize" work-items
		cl::NDRange offset(0);
		cl::NDRange globalSize(ciphersize); 	// "ciphersize" work-units per kernel
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);
		std::cout << "Kernel enqueued for execution..." << std::endl;
		std::cout << "--------------------" << std::endl;

		std::vector <cl_char> uppertextK(ciphersize);
		std::cout << "Reading the contents from local memory to output array after conversion...\n" << std::endl;
		queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(cl_char) * ciphersize, &uppertextK[0]);

		std::cout << "Plaintext (in uppercase):" << std::endl;
		for (int i = 0; i < uppertextK.size(); i++)
		{

			std::cout << uppertextK[i];
		}

		std::cout << std::endl << std::endl;

		// Update the buffer with the uppercase plaintext
		contentsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * ciphersize, &uppertextK[0]);

		// create a kernel
		std::cout << "Creating a kernel to encrypt..." << std::endl;
		kernel = cl::Kernel(program, "encrypt");

		// setting kernel arguments
		std::cout << "Setting kernel arguments for encryption..." << std::endl;
		kernel.setArg(0, contentsBuffer); // plaintext in uppercase
		kernel.setArg(1, outputBuffer); // ciphertext to be stored after processing
		kernel.setArg(2, shiftBuffer); // shift value from user

		// enqueue kernel for execution
		// the kernel will execute on 4 work-items
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);
		std::cout << "Kernel enqueued for execution..." << std::endl;
		std::cout << "--------------------" << std::endl;

		std::vector <cl_char> ciphertextK(ciphersize);
		std::cout << "Reading the contents from local memory to output array after encryption...\n" << std::endl;
		queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(cl_char) * ciphersize, &ciphertextK[0]);
		displayCipherK(ciphertextK);

		// create a kernel
		std::cout << "Creating a kernel to decrypt..." << std::endl;
		kernel = cl::Kernel(program, "decrypt");

		// Update the buffer with the ciphertext
		contentsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * ciphersize, &ciphertextK[0]);

		// setting kernel arguments
		std::cout << "Setting kernel arguments for decryption..." << std::endl;
		kernel.setArg(0, contentsBuffer); // plaintext in uppercase
		kernel.setArg(1, outputBuffer); // ciphertext to be stored after processing
		kernel.setArg(2, shiftBuffer); // shift value from user

		// enqueue kernel for execution
		// the kernel will execute on 4 work-items
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);
		std::cout << "Kernel enqueued for execution..." << std::endl;
		std::cout << "--------------------" << std::endl;

		std::cout << "Reading the contents from local memory to output array after decryption...\n" << std::endl;
		queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(cl_char) * ciphersize, &ciphertextK[0]);
		
		// create a kernel
		std::cout << "Creating a kernel to convert to lowercase..." << std::endl;
		kernel = cl::Kernel(program, "toLower");

		// Update the buffer with the plaintext
		contentsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * ciphersize, &ciphertextK[0]);

		// setting kernel arguments
		std::cout << "Setting kernel arguments for conversion..." << std::endl;
		kernel.setArg(0, contentsBuffer);
		kernel.setArg(1, outputBuffer);

		// enqueue kernel for execution
		// the kernel will execute on "ciphersize" work-items
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);
		std::cout << "Kernel enqueued for execution..." << std::endl;
		std::cout << "--------------------" << std::endl;

		std::vector <cl_char> lowertextK(ciphersize);
		std::cout << "Reading the contents from local memory to output array after conversion...\n" << std::endl;
		queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(cl_char) * ciphersize, &lowertextK[0]);

		displayDCipherK(lowertextK);

		std::cout << "--------------------" << std::endl;
		std::cout << "\nPART 2C:\n" << std::endl;
		std::cout << "Perform parallel encryption and decryption based on the lookup table...\n" << std::endl;

		// Create a buffer for plaintext
		contentsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * ciphersize, &plaintext[0]);

		// create a kernel
		std::cout << "Creating a kernel to encrypt based on lookup table..." << std::endl;
		kernel = cl::Kernel(program, "encryptLT");

		// setting kernel arguments
		std::cout << "Setting kernel arguments for encryption based on lookup table..." << std::endl;
		kernel.setArg(0, contentsBuffer); // plaintext (lowercase and uppercase)
		kernel.setArg(1, outputBuffer); // ciphertext to be stored after processing

		// enqueue kernel for execution
		// the kernel will execute on 4 work-items
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);
		std::cout << "Kernel enqueued for execution..." << std::endl;
		std::cout << "--------------------" << std::endl;

		std::cout << "Reading the contents from local memory to output array after encryption based on lookup table...\n" << std::endl;
		queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(cl_char) * ciphersize, &ciphertextK[0]);
		displayCipherK(ciphertextK);

		// Create a buffer for ciphertext
		contentsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * ciphersize, &ciphertextK[0]);

		// create a kernel
		std::cout << "Creating a kernel to decrypt based on lookup table..." << std::endl;
		kernel = cl::Kernel(program, "decryptLT");

		// setting kernel arguments
		std::cout << "Setting kernel arguments for decryption based on lookup table..." << std::endl;
		kernel.setArg(0, contentsBuffer); // plaintext (lowercase and uppercase)
		kernel.setArg(1, outputBuffer); // ciphertext to be stored after processing

		// enqueue kernel for execution
		// the kernel will execute on 4 work-items
		queue.enqueueNDRangeKernel(kernel, offset, globalSize);
		std::cout << "Kernel enqueued for execution..." << std::endl;
		std::cout << "--------------------" << std::endl;

		std::cout << "Reading the contents from local memory to output array after decryption based on lookup table...\n" << std::endl;
		queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(cl_char) * ciphersize, &ciphertextK[0]);

		displayDCipherK(ciphertextK);
	}
	catch (cl::Error e)
	{
		handle_error (e);
	}
	system("pause");
	
	return 0;
}

bool checkFileOK(std::fstream& afile, std::string filename) // open file checking ok
{
	afile.open(filename, std::ios::in);

	if (!afile)
	{
		return false;
	}

	return true;

}

void displayFileContents(std::fstream& afile, std::string filename, std::vector <cl_char>& content)
{
	char letter;

	std::cout << "Reading contents in \"" << filename << "\"..." << std::endl;
	//std::cout << afile.rdbuf();
	
	std::cout << "\nPlaintext:" << std::endl;
	while (afile >> letter)
	{
		if (tolower (letter) >= 'a' && tolower (letter) <= 'z')
		{
			content.push_back (letter);
		}
	}

	for (int i = 0; i < content.size(); i++)
	{

		std::cout << content [i];
	}

	std::cout << std::endl;

	afile.close();
}

int getShiftValue (int n)
{
	do
	{
		std::cout << "\nEnter a valid value n: ";
		std::cin >> n;
	} while (n == 0);

	if (n < 0)
	{
		std::cout << "You have chosen to shift by " << n << " positions to the left." << std::endl;
	}
	else
	{
		std::cout << "You have chosen to shift by " << n << " positions to the right." << std::endl;
	}

	return n;
}

void encrypt (int n, std::vector <cl_char> content, std::vector <cl_char>& ciphertext)
{
	char letter;
	std::string cipher = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	std::cout << "\nEncrypting contents..." << std::endl;

	std::cout << "\nCiphertext: " << std::endl;
	for (int i = 0; i < content.size(); i++)
	{
		letter = content[i];
		// make sure the character is a letter
		if (toupper(letter) >= 'A' && toupper(letter) <= 'Z')
		{
			// convert to uppercase
			letter = toupper(letter);
			
			// if n is negative
			if (n < 0)
			{
				// iterate thru the cipher letters
				for (int i = 0; i < cipher.size(); i++)
				{
					// // match the letter with the cipher string to get the index
					if (cipher[i] == letter)
					{
						// if i - n value is within the index of the cipher
						// remove the negative sign from n
						if (i-abs(n) > -1)
						{
							// shift the letter in n position(s) from its left
							letter = cipher[i-abs(n)];
							ciphertext.push_back(letter); // add the new cipher char to vector
							break;
						}
						// if i - n value is not within the index of the cipher
						else
						{
							// shift the letter in n position(s) from its left
							// wrap around to the end
							letter = cipher[i-abs(n) + cipher.size()];
							ciphertext.push_back(letter); // add the new cipher char to vector
							break;
						}
					}
				}
			}
			// if n is positive (n > 0)
			else 
			{
				// iterate thru the cipher letters
				for (int i = 0; i < cipher.size(); i++)
				{
					// // match the letter with the cipher string to get the index
					if (cipher[i] == letter)
					{
						// if i + n value is not within the index of the cipher or the size of the cipher
						if (i+n >= cipher.size())
						{
							// shift the letter in n position(s) from its right
							// wrap around to the end
							letter = cipher[(i+n) % cipher.size()];
							ciphertext.push_back(letter); // add the new cipher char to vector
							break; // need to break else it will keep iterating in the loop
						}
						// if i + n value is within the index of the cipher
						else
						{
							// shift the letter in n position(s) from its right
							letter = cipher[i+n];
							ciphertext.push_back(letter); // add the new cipher char to vector
							break; // // need to break else it will keep iterating in the loop
						}
					}
				}
			}
		}
		
	}
}

void displayCipher(std::vector <cl_char> cipher)
{
	int b = 0;
	for (int i = 0; i < cipher.size(); i++)
	{
		++b; // increment for every cipher char that is going to be displayed
		std::cout << cipher[i];
		if (b % 5 == 0) // in blocks of 5 so check if b is divisible by 5
		{
			std::cout << ' '; // print a space to divide next block
		}
	}

	std::cout << std::endl;
}

int countNoOfLetters(std::vector <cl_char> content)
{
	int total = 0;
	for (int i = 0; i < content.size(); i++)
	{
		// make sure the character is a letter
		if (toupper(content[i]) >= 'A' && toupper(content[i]) <= 'Z')
		{
			++total; // to count no. of letters
		}
	}
	return total;
}

void displayPlaintext(std::vector <cl_char> content, std::vector <cl_char>& plaintext)
{
	std::cout << "\nPlaintext:" << std::endl;
	int j = 0;
	for (int i = 0; i < content.size (); i++)
	{
		// make sure the character is a letter
		if (toupper(content[j]) >= 'A' && toupper(content[j]) <= 'Z')
		{
			// store only letters
			plaintext[i] = content[j];
			std::cout << plaintext[i];
			++j;
		}
	}
	std::cout << std::endl << std::endl;
}

void displayCipherK(std::vector <cl_char> cipher)
{
	int b = 0;
	std::cout << "Ciphertext:" << std::endl;
	for (int i = 0; i < cipher.size(); i++)
	{
		++b; // increment for every cipher char that is going to be displayed
		std::cout << cipher[i];
		if (b % 5 == 0) // in blocks of 5 so check if b is divisible by 5
		{
			std::cout << ' '; // print a space to divide next block
		}
	}
	std::cout << std::endl << std::endl;
}

void displayDCipherK(std::vector <cl_char> cipher)
{
	std::cout << "Decrypted ciphertext:" << std::endl;
	for (int i = 0; i < cipher.size(); i++)
	{
		std::cout << cipher[i];
	}
	std::cout << std::endl << std::endl;
}