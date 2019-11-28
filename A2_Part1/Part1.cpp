// Name: Nur Suhaira Bte Badrul Hisham
// Student ID: 5841549
// Assignment 2 Part 1

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

const int int8 = 8; // fixed array size for array1
const int int16 = 16; // fixed array size for array2
const int FILESIZE = 300; // contents size

void createArrays (cl_int*, cl_int*); // creating values for array1 and array2
void displayOutputArr (std::vector <cl_int>); // display the results after kernel execution

int main()
{
	cl::Platform platform;			// device's platform
	cl::Device device;				// device used
	cl::Context context;			// context for the device
	cl::Program program;			// OpenCL program object
	cl::Kernel kernel;				// a single kernel object
	cl::CommandQueue queue;			// commandqueue for a context and device
	
	cl_int array1 [int8]; // array1 with random values from 0 to 9
	cl_int array2 [int16]; // array2 with values from -8 to 8

	// declare data and memory objects
	std::vector <cl_int> output (int8);
	cl::Buffer array1Buffer;
	cl::Buffer array2Buffer;
	cl::Buffer outputBuffer;

	// Initialize random seed
	srand (time(NULL));  

	try
	{
		std::cout << "\n============TASK 1: Basic Kernel Programming============\n" << std::endl;

		// select an OpenCL device
		if (!select_one_device(&platform, &device))
		{
			// if no device selected
			quit_program("Device not selected.");
		}

		// create a context from device
		context = cl::Context(device);

		// build the program
		if (!build_program(&program, &context, "Part1.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}

		createArrays(array1, array2);

		// create command queue
		queue = cl::CommandQueue(context, device);
		std::cout << "Creating a command queue..." << std::endl;

		// create buffers
		// a buffer object where the data is intended to be read-only (CL_MEM_READ_ONLY) on the device and
		// CL_MEM_COPY_HOST_PTR indicates that the application wants the OpenCL implementation to allocate
		// memory for the memory object and copy the data from memory referenced by array1 & 2
		std::cout << "Reading the contents from array 1 into local memory..." << std::endl;
		array1Buffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * int8, &array1[0]);
		std::cout << "Reading the contents from array 2 into local memory..." << std::endl;
		array2Buffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * int16, &array2[0]);
		std::cout << "--------------------" << std::endl;

		// This memory object is intended to be for read and write access on the device.
		// Since no host pointer was passed, the following only reserves memory :
		outputBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_int) * int8);

		// create a kernel
		std::cout << "Creating a kernel..." << std::endl;
		kernel = cl::Kernel(program, "copy");

		// setting kernel arguments
		std::cout << "Setting kernel arguments..." << std::endl;
		kernel.setArg(0, array1Buffer);
		kernel.setArg(1, array2Buffer);
		kernel.setArg(2, outputBuffer);

		// enqueue kernel for execution
		queue.enqueueTask(kernel);
		std::cout << "Kernel enqueued for execution..." << std::endl;
		std::cout << "--------------------" << std::endl;

		// In the host program, check that the results are correct and 
		// display the contents of the output array
		std::cout << "Reading the contents from local memory to output array..." << std::endl;
		queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(cl_int) * output.size(), &output[0]);
		
		displayOutputArr(output);
	}
	catch (cl::Error e)
	{
		handle_error (e);
	}
	system("pause");
	
	return 0;
}

void createArrays(cl_int* array1, cl_int* array2)
{
	unsigned int i, j; // counters (unsigned)
	signed int k; // counters (signed)

	std::cout << "Generating random values [0 to 9] for Array 1: " << std::endl;
	std::cout << "\n\tIndex\tContent" << std::endl;
	for (i = 0; i < int8; i++)
	{
		// Generate random values  between 0 and 9
		array1[i] = (rand() % 10);

		// Displaying the content of the array 1 with its index
		std::cout << "\t" << i << "\t" << array1[i] << std::endl;
	}

	// i will be the same value as SIZE8 which will cause to exit the loop
	// this means array 1 is filled fully based on the declared size
	std::cout << "Array 1 created: ";
	if (i == int8)
	{
		std::cout << "Successful" << std::endl;
	}
	else
	{
		std::cout << "Failed" << std::endl;
	}
	std::cout << "--------------------" << std::endl;
	std::cout << "Generating values for Array 2: " << std::endl;
	std::cout << "\tSetting values [1 to 8] for first half of Array 2" << std::endl;
	std::cout << "\tSetting values [-8 to -1] for second half of Array 2" << std::endl;
	std::cout << "\n\tIndex\tContent" << std::endl;
	// Setting values for first half of array2
	j = 0;
	for (i = 0; i < int16 / 2; i++)
	{
		// Increment values from 1 to 8
		++j;
		array2[i] = j;

		// Displaying the content of the array 2 with its index
		std::cout << "\t" << i << "\t" << array2[i] << std::endl;
	}
	// Setting values for second half of array2
	k = -8;
	for (i; i < int16; i++)
	{
		array2[i] = k;

		// Displaying the content of the array 2 with its index
		std::cout << "\t" << i << "\t" << array2[i] << std::endl;

		// Increment values from -8 to -1
		++k;
	}

	// i will be the same value as SIZE8 which will cause to exit the loop
	// this means array 1 is filled fully based on the declared size
	std::cout << "Array 2 created: ";
	if (i == int16)
	{
		std::cout << "Successful" << std::endl;
	}
	else
	{
		std::cout << "Failed" << std::endl;
	}
	std::cout << "--------------------" << std::endl;

}

void displayOutputArr (std::vector <cl_int> output)
{
	std::cout << "\n\tIndex\tContent" << std::endl;
	for (int i = 0; i < int8; i++)
	{
		// Displaying the content of the v1 with its index
		std::cout << "\t" << i << "\t" << output[i] << std::endl;
	}
	std::cout << "--------------------" << std::endl;
}