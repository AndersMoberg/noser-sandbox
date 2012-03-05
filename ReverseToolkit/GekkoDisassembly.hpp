// GekkoDisassembly.hpp
// Nolan Check
// Created 6/4/2011

#ifndef _GEKKODISASSEMBLY_HPP
#define _GEKKODISASSEMBLY_HPP

#include <cstdint>
#include <string>

class LabelProvider
{
public:
	virtual std::string GetLabel(uint32_t addr) const = 0;
};

std::string DisassembleGekko(uint32_t instr, uint32_t cia, uint32_t& addrRef, const LabelProvider* labelProv);

#endif
