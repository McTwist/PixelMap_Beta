

#pragma once
#ifndef COMPUTER_HPP
#define COMPUTER_HPP

#include "Globals.hpp"

namespace Computer
{
	unsigned long GetAvailableMemory();
	unsigned long GetTotalMemory();
	unsigned long GetLogicalCores();
}

#endif // COMPUTER_HPP