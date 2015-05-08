#pragma once

#ifdef __APPLE__
#include <OpenCL/cl.h>
#include <OpenCL/cl_platform.h>
#elif defined __linux__
#include <CL/cl.h>
#include <CL/cl_platform.h>
#endif

#include <vector>
#include <string>
#include <iostream>

#include "Primitives.hpp"

Mesh load_mesh(std::string filename);
