// Name: Nur Suhaira Bte Badrul Hisham
// Student ID: 5841549
// Assignment 2 Part 3

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

#define LENGTH 8
#define WORKITEMINFO 3

int main()
{
	cl::Platform platform;			// device's platform
	cl::Device device;				// device used
	cl::Context context;			// context for the device
	cl::Program program;			// OpenCL program object
	cl::Kernel kernel;				// a single kernel object
	cl::CommandQueue queue;			// commandqueue for a context and device
	
	std::vector <cl_int> passcode (LENGTH);
	unsigned int workGrpSz = 0;

	cl_int size = LENGTH;
	cl_int passcodeArr [] = {9, 5, 8, 4, 1, 5, 4, 9}; // passcode (9 + UOW student id)
	passcode.insert (passcode.begin(), passcodeArr, passcodeArr + LENGTH); // insert passcode into vector
	

	// declare data and memory objects
	cl::Buffer outputBuffer;
	cl::Buffer pwdBuffer;
	cl::Buffer sizeBuffer;

	try
	{
		std::cout << "\n===PART 3: Brute-force Passcode Cracking===\n" << std::endl;

		// select an OpenCL device
		if (!select_one_device(&platform, &device))
		{
			// if no device selected
			quit_program("Device not selected.");
		}

		// create a context from device
		context = cl::Context(device);

		// build the program
		if (!build_program(&program, &context, "Part3.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}

		// create command queue
		queue = cl::CommandQueue(context, device);
		std::cout << "Creating a command queue..." << std::endl;

		// Create a buffer for passcode
		pwdBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * LENGTH, &passcode[0]);
		sizeBuffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * LENGTH, &size);
		// This memory object is intended to be for read and write access on the device.
		// Since no host pointer was passed, the following only reserves memory :
		outputBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_int) * WORKITEMINFO); // 3 info items

		// create a kernel to check whether all digits in the attempt match the stored passcode
		std::cout << "Creating a kernel to check passcode matching..." << std::endl;
		kernel = cl::Kernel(program, "searchMatch");

		// setting kernel arguments
		std::cout << "Setting kernel arguments for passcode checking..." << std::endl;
		kernel.setArg(0, pwdBuffer);
		kernel.setArg(1, sizeBuffer);
		kernel.setArg(2, outputBuffer);

		// Get devices in the context
		std::vector <cl::Device> contextDevice = context.getInfo <CL_CONTEXT_DEVICES>();

		for (int i = 0; i < contextDevice.size(); i++)
		{
			// Get number of maximum work group size
			workGrpSz = contextDevice[i].getInfo <CL_DEVICE_MAX_WORK_GROUP_SIZE>();
		}

		std::cout << "--------------------" << std::endl;
		std::cout << "Number of maximum work group size: " << workGrpSz << std::endl;

		// Set total number of work items
		size_t globalSize = workGrpSz; // 2^8 = 256

		// Divide the total number of work items into work item groups of 64
		size_t localSize = 64;

		// Enqueue kernel for execution
		queue.enqueueNDRangeKernel (kernel, NULL, globalSize, localSize, 0, NULL);
		std::cout << "Kernel enqueued for execution..." << std::endl;
		std::cout << "--------------------" << std::endl;

		// Display information about the work-item that found the passcode 
		// (i.e. its global id, work - group id and id within the work - group)
		// Create a local variable to store info items
		int *workItem = (int*) malloc (sizeof (cl_int) * WORKITEMINFO);

		std::cout << "Reading the contents from local memory to local variable...\n" << std::endl;
		queue.enqueueReadBuffer (outputBuffer, CL_TRUE, 0, sizeof(cl_int) * WORKITEMINFO, workItem);

		// Display the output
		std::cout << "Work-item Information" << std::endl;
		std::cout << "=====================" << std::endl;
		std::cout << "CL_DEVICE_NAME: " << contextDevice[0].getInfo<CL_DEVICE_NAME>() << std::endl;
		std::cout << "Global ID                    :\t";
		std::cout << workItem[0] << std::endl;
		std::cout << "ID Within Work Group (Local) :\t";
		std::cout << workItem[1] << std::endl;
		std::cout << "Work-Group ID                :\t";
		std::cout << workItem[2] << std::endl;

	}
	catch (cl::Error e)
	{
		handle_error (e);
	}
	system("pause");
	
	return 0;
}