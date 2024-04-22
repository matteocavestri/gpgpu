#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_WORK_ITEMS 1024

int main() {
  cl_int err;
  cl_platform_id platform;
  cl_device_id device;
  cl_context context;
  cl_command_queue queue;
  cl_program program;
  cl_kernel kernel;
  cl_mem buffer_count, buffer_seeds;

  int NUM_SAMPLES_PER_ITEM = 1000; // Now it's a variable

  // Initialize OpenCL
  clGetPlatformIDs(1, &platform, NULL);
  clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
  context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);

  // Using clCreateCommandQueueWithProperties instead of deprecated version
  const cl_queue_properties props[] = {CL_QUEUE_PROPERTIES,
                                       CL_QUEUE_PROFILING_ENABLE, 0};
  queue = clCreateCommandQueueWithProperties(context, device, props, &err);

  FILE *program_handle = fopen("pi_kernel.cl", "r");
  fseek(program_handle, 0, SEEK_END);
  size_t program_size = ftell(program_handle);
  rewind(program_handle);
  char *program_buffer = (char *)malloc(program_size + 1);
  program_buffer[program_size] = '\0';
  fread(program_buffer, sizeof(char), program_size, program_handle);
  fclose(program_handle);

  // Create and build the program
  program = clCreateProgramWithSource(
      context, 1, (const char **)&program_buffer, &program_size, &err);
  err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);

  // Create the kernel
  kernel = clCreateKernel(program, "compute_pi", &err);

  // Prepare buffers
  int *counts = (int *)calloc(NUM_WORK_ITEMS, sizeof(int));
  unsigned int *seeds =
      (unsigned int *)malloc(NUM_WORK_ITEMS * sizeof(unsigned int));
  for (int i = 0; i < NUM_WORK_ITEMS; i++) {
    seeds[i] = rand(); // Different seed for each work-item
  }
  buffer_count = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                NUM_WORK_ITEMS * sizeof(int), NULL, &err);
  buffer_seeds =
      clCreateBuffer(context, CL_MEM_READ_ONLY,
                     NUM_WORK_ITEMS * sizeof(unsigned int), NULL, &err);
  clEnqueueWriteBuffer(queue, buffer_seeds, CL_TRUE, 0,
                       NUM_WORK_ITEMS * sizeof(unsigned int), seeds, 0, NULL,
                       NULL);

  // Set kernel arguments
  clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_count);
  clSetKernelArg(kernel, 1, sizeof(int), &NUM_SAMPLES_PER_ITEM);
  clSetKernelArg(kernel, 2, sizeof(cl_mem), &buffer_seeds);

  // Enqueue the kernel
  size_t global_work_size = NUM_WORK_ITEMS;
  clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, NULL, 0,
                         NULL, NULL);
  clFinish(queue);

  // Read the results
  clEnqueueReadBuffer(queue, buffer_count, CL_TRUE, 0,
                      NUM_WORK_ITEMS * sizeof(int), counts, 0, NULL, NULL);

  // Calculate Pi
  int total_count = 0;
  for (int i = 0; i < NUM_WORK_ITEMS; i++) {
    total_count += counts[i];
  }
  double pi = 4.0 * total_count / (NUM_WORK_ITEMS * NUM_SAMPLES_PER_ITEM);
  printf("Approximation of Pi: %f\n", pi);

  // Cleanup
  clReleaseMemObject(buffer_count);
  clReleaseMemObject(buffer_seeds);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);
  free(counts);
  free(seeds);
  free(program_buffer);

  return 0;
}
