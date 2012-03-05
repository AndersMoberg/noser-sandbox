// GekkoDisassembly.cpp
// Nolan Check
// Created 1/4/2011

#include "GekkoDisassembly.hpp"

#include <iomanip>
#include <sstream>

static std::string bin(uint32_t val, unsigned int count)
{
	std::string result;
	result.resize(count+1);
	for (unsigned int i = 0; i < count; ++i)
	{
		result[i] = (val & (1 << (count-1-i))) ? '1' : '0';
	}
	result[count] = 'b';
	return result;
}

//static std::string hex(uint32_t val, unsigned int count = 0)
//{
//	std::stringstream ss;
//	if (count != 0)
//		ss << std::hex << std::uppercase << std::setfill('0') << std::setw(count) << val << 'h';
//	else
//		ss << std::hex << std::uppercase << std::setfill('0') << val << 'h';
//	return ss.str();
//}

//static std::string hex(uint16_t val, unsigned int count = 0)
//{
//	return hex(uint32_t(val), count);
//}
//
//static std::string hex(uint8_t val, unsigned int count = 0)
//{
//	return hex(uint32_t(val), count);
//}
//
//static std::string hex(int16_t val, unsigned int count = 0)
//{
//	std::stringstream ss;
//	if (count != 0)
//		ss << std::hex << std::uppercase << std::setfill('0') << std::setw(count) << val << 'h';
//	else
//	{
//		if (val < 0)
//			ss << std::hex << std::uppercase << std::setfill('0') << '-' << (-val) << 'h';
//		else
//			ss << std::hex << std::uppercase << std::setfill('0') << val << 'h';
//	}
//	return ss.str();
//}

static std::string reg(uint32_t val)
{
	std::stringstream ss;
	ss << 'r' << val;
	return ss.str();
}

static std::string freg(uint32_t val)
{
	std::stringstream ss;
	ss << 'f' << val;
	return ss.str();
}

static std::string crf(uint32_t val)
{
	std::stringstream ss;
	ss << "cr" << val;
	return ss.str();
}

static std::string crb(uint32_t val)
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}

static std::string address(uint32_t val, const LabelProvider* labelProv)
{
	return labelProv->GetLabel(val);
}

static std::string simm(int16_t val)
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}

static std::string uimm(uint16_t val)
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}

static std::string displ(int16_t val)
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}

static std::string crm(uint8_t val)
{
	std::stringstream ss;
	ss << int(val);
	return ss.str();
}

static std::string fm(uint8_t val)
{
	std::stringstream ss;
	ss << int(val);
	return ss.str();
}

inline uint32_t instr_OPCD(uint32_t instr) { return instr >> 26; }
inline int32_t instr_LI(uint32_t instr) { return (int32_t)((instr & ~0x3) << 6) >> 6; }
inline int32_t instr_BD(uint32_t instr) { return (int32_t)((instr & ~0x3) << 16) >> 16; }
inline uint32_t instr_AA(uint32_t instr) { return (instr >> 1) & 0x1; }
inline uint32_t instr_LK(uint32_t instr) { return instr & 0x1; }
inline uint32_t instr_spr(uint32_t instr) { return ((instr >> 16) & 0x1F) | (((instr >> 11) & 0x1F) << 5); }
inline uint32_t instr_tbr(uint32_t instr) { return ((instr >> 16) & 0x1F) | (((instr >> 11) & 0x1F) << 5); }
inline uint32_t instr_crbA(uint32_t instr) { return (instr >> 16) & 0x1F; }
inline uint32_t instr_crbB(uint32_t instr) { return (instr >> 11) & 0x1F; }
inline uint32_t instr_crbD(uint32_t instr) { return (instr >> 21) & 0x1F; }
inline uint32_t instr_frA(uint32_t instr) { return (instr >> 16) & 0x1F; }
inline uint32_t instr_frB(uint32_t instr) { return (instr >> 11) & 0x1F; }
inline uint32_t instr_frC(uint32_t instr) { return (instr >> 6) & 0x1F; }
inline uint32_t instr_frD(uint32_t instr) { return (instr >> 21) & 0x1F; }
inline uint32_t instr_frS(uint32_t instr) { return (instr >> 21) & 0x1F; }
inline uint32_t instr_rA(uint32_t instr) { return (instr >> 16) & 0x1F; }
inline uint32_t instr_rB(uint32_t instr) { return (instr >> 11) & 0x1F; }
inline uint32_t instr_rD(uint32_t instr) { return (instr >> 21) & 0x1F; }
inline uint32_t instr_rS(uint32_t instr) { return (instr >> 21) & 0x1F; }
inline uint32_t instr_BO(uint32_t instr) { return (instr >> 21) & 0x1F; }
inline uint32_t instr_BI(uint32_t instr) { return (instr >> 16) & 0x1F; }
inline uint32_t instr_crfD(uint32_t instr) { return (instr >> 23) & 0x7; }
inline int16_t instr_d1631(uint32_t instr) { return (int16_t)(instr & 0xFFFF); }
inline int16_t instr_d2031(uint32_t instr) { return (int16_t)(instr << 4) >> 4; }
inline uint32_t instr_SH(uint32_t instr) { return (instr >> 11) & 0x1F; }
inline uint32_t instr_MB(uint32_t instr) { return (instr >> 6) & 0x1F; }
inline uint32_t instr_ME(uint32_t instr) { return (instr >> 1) & 0x1F; }
inline uint8_t instr_CRM(uint32_t instr) { return (instr >> 12) & 0xFF; }
inline uint8_t instr_FM(uint32_t instr) { return (instr >> 17) & 0xFF; }
inline uint32_t instr_TO(uint32_t instr) { return (instr >> 21) & 0x1F; }
inline uint32_t instr_SR(uint32_t instr) { return (instr >> 16) & 0xF; }
inline int16_t instr_SIMM(uint32_t instr) { return (int16_t)(instr & 0xFFFF); }
inline uint16_t instr_UIMM(uint32_t instr) { return instr & 0xFFFF; }
inline uint32_t instr_XO2130(uint32_t instr) { return (instr >> 1) & 0x3FF; }
inline uint32_t instr_XO2230(uint32_t instr) { return (instr >> 1) & 0x1FF; }
inline uint32_t instr_XO2530(uint32_t instr) { return (instr >> 1) & 0x3F; }
inline uint32_t instr_XO2630(uint32_t instr) { return (instr >> 1) & 0x1F; }
inline uint32_t instr_OE(uint32_t instr) { return (instr >> 10) & 0x1; }
inline uint32_t instr_Rc(uint32_t instr) { return instr & 0x1; }
inline uint32_t instr_L(uint32_t instr) { return (instr >> 21) & 0x1; }
inline uint32_t instr_W16(uint32_t instr) { return (instr >> 15) & 0x1; }
inline uint32_t instr_W21(uint32_t instr) { return (instr >> 10) & 0x1; }
inline uint32_t instr_I1719(uint32_t instr) { return (instr >> 12) & 0x7; }
inline uint32_t instr_I2224(uint32_t instr) { return (instr >> 7) & 0x7; }

std::string DisassembleGekko(uint32_t instr, uint32_t cia, uint32_t& addrRef, const LabelProvider* labelProv)
{
	addrRef = 0;
	std::stringstream ss;

	uint32_t opcd = instr_OPCD(instr);
	switch (opcd)
	{
	case 3: // twi: Trap Word Immediate
		ss << "twi " << instr_TO(instr) << ", " << reg(instr_rA(instr)) << ", " << simm(instr_SIMM(instr));
		break;
	case 4:
		switch (instr_XO2630(instr))
		{
		case 10: // ps_sum0[x]: Paired Single vector SUM high
			if (instr_Rc(instr) == 0)
				ss << "ps_sum0 " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown ps_sum0[x] Rc = " << instr_Rc(instr);
			break;
		case 11: // ps_sum1[x]: Paired Single vector SUM low
			if (instr_Rc(instr) == 0)
				ss << "ps_sum1 " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown ps_sum1[x] Rc = " << instr_Rc(instr);
			break;
		case 12: // ps_muls0[x]: Paired Single Multiply Scalar high
			if (instr_Rc(instr) == 0)
				ss << "ps_muls0 " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr));
			else
				ss << "ERROR: Unknown ps_muls0[x] Rc = " << instr_Rc(instr);
			break;
		case 13: // ps_muls1[x]: Paired Single Multiply Scalar low
			if (instr_Rc(instr) == 0)
				ss << "ps_muls1 " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr));
			else
				ss << "ERROR: Unknown ps_muls1[x] Rc = " << instr_Rc(instr);
			break;
		case 14: // ps_madds0[x]: Paired Single Multiply-Add Scalar high
			if (instr_Rc(instr) == 0)
				ss << "ps_madds0 " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown ps_madds0[x] Rc = " << instr_Rc(instr);
			break;
		case 15: // ps_madds1[x]: Paired Single Multiply-Add Scalar low
			if (instr_Rc(instr) == 0)
				ss << "ps_madds1 " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown ps_madds1[x] Rc = " << instr_Rc(instr);
			break;
		case 18: // ps_div[x]: Paired Single Divide
			if (instr_Rc(instr) == 0)
				ss << "ps_div " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown ps_div[x] Rc = " << instr_Rc(instr);
			break;
		case 20: // ps_sub[x]: Paired Single Subtract
			if (instr_Rc(instr) == 0)
				ss << "ps_sub " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown ps_sub[x] Rc = " << instr_Rc(instr);
			break;
		case 21: // ps_add[x]: Paired Single Add
			if (instr_Rc(instr) == 0)
				ss << "ps_add " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown ps_add[x] Rc = " << instr_Rc(instr);
			break;
		case 23: // ps_sel[x]: Paired Single Select
			ss << "ERROR: Unknown ps_sel[x] Rc = " << instr_Rc(instr);
			break;
		case 24: // ps_res[x]: Paired Single Reciprocal Estimate
			if (instr_Rc(instr) == 0)
				ss << "ps_res " << freg(instr_frD(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown ps_res[x] Rc = " << instr_Rc(instr);
			break;
		case 25: // ps_mul[x]: Paired Single Multiply
			if (instr_Rc(instr) == 0)
				ss << "ps_mul " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr));
			else
				ss << "ERROR: Unknown ps_mul[x] Rc = " << instr_Rc(instr);
			break;
		case 28: // ps_msub[x]: Paired Single Multiply-Subtract
			if (instr_Rc(instr) == 0)
				ss << "ps_msub " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown ps_msub[x] Rc = " << instr_Rc(instr);
			break;
		case 29: // ps_madd[x]: Paired Single Multiply-Add
			if (instr_Rc(instr) == 0)
				ss << "ps_madd " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown ps_madd[x] Rc = " << instr_Rc(instr);
			break;
		case 30: // ps_nmsub[x]: Paired Single Negative Multiply-Subtract
			if (instr_Rc(instr) == 0)
				ss << "ps_nmsub " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown ps_nmsub[x] Rc = " << instr_Rc(instr);
			break;
		case 31: // ps_nmadd[x]: Paired Single Negative Multiply-Add
			if (instr_Rc(instr) == 0)
				ss << "ps_nmadd " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown ps_nmadd[x] Rc = " << instr_Rc(instr);
			break;
		default:
			switch (instr_XO2530(instr))
			{
			case 6: // psq_lx: Paired Single Quantized Load Indexed
				ss << "psq_lx " << freg(instr_frD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr)) << ", " << instr_W21(instr) << ", " << instr_I2224(instr);
				break;
			case 7: // psq_stx: Paired Single Quantized Store Indexed
				ss << "psq_stx " << freg(instr_frS(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr)) << ", " << instr_W21(instr) << ", " << instr_I2224(instr);
				break;
			default:
				switch (instr_XO2130(instr))
				{
				case 0: // ps_cmpu0: Paried Singles Compare Unordered High
					ss << "ps_cmpu0 " << crf(instr_crfD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
					break;
				case 32: // ps_cmpo0: Paired Singles Compare Ordered High
					ss << "ps_cmpo0 " << crf(instr_crfD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
					break;
				case 40: // ps_neg[x]: Paired Single Negate
					if (instr_Rc(instr) == 0)
						ss << "ps_neg " << freg(instr_frD(instr)) << ", " << freg(instr_frB(instr));
					else
						ss << "ERROR: Unknown ps_neg[x] Rc = " << instr_Rc(instr);
					break;
				case 64: // ps_cmpu1: Paired Singles Compare Unordered Low
					ss << "ps_cmpu1 " << crf(instr_crfD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
					break;
				case 72: // ps_mr[x]: Paired Single Move Register
					if (instr_Rc(instr) == 0)
						ss << "ps_mr " << freg(instr_frD(instr)) << ", " << freg(instr_frB(instr));
					else
						ss << "ERROR: Unknown ps_mr[x] Rc = " << instr_Rc(instr);
					break;
				case 264: // ps_abs[x]: Paired Single Absolute Value
					ss << "ERROR: Unknown ps_abs[x] Rc = " << instr_Rc(instr);
					break;
				case 528: // ps_merge00[x]: Paired Single MERGE high
					if (instr_Rc(instr) == 0)
						ss << "ps_merge00 " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
					else
						ss << "ERROR: Unknown ps_merge00[x] Rc = " << instr_Rc(instr);
					break;
				case 560: // ps_merge01[x]: Paired Single MERGE direct
					if (instr_Rc(instr) == 0)
						ss << "ps_merge01 " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
					else
						ss << "ERROR: Unknown ps_merge01[x] Rc = " << instr_Rc(instr);
					break;
				case 592: // ps_merge10[x]: Paired Single MERGE swapped
					if (instr_Rc(instr) == 0)
						ss << "ps_merge10 " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
					else
						ss << "ERROR: Unknown ps_merge10[x] Rc = " << instr_Rc(instr);
					break;
				case 624: // ps_merge11[x]: Paired Single MERGE low
					if (instr_Rc(instr) == 0)
						ss << "ps_merge11 " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
					else
						ss << "ERROR: Unknown ps_merge11[x] Rc = " << instr_Rc(instr);
					break;
				case 1014: // dcbz_l: Data Cache Block Set to Zero Locked
					ss << "dcbz_l " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
					break;
				default:
					ss << "ERROR: Unknown OPCD:4 XO[21:30] " << instr_XO2130(instr) << " (" << bin(instr_XO2130(instr),10) << ')';
					break;
				}
				break;
			}
			break;
		}
		break;
	case 7: // mulli: Multiply Low Immediate
		ss << "mulli " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << simm(instr_SIMM(instr));
		break;
	case 8: // subfic: Subtract from Immediate Carrying
		ss << "subfic " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << simm(instr_SIMM(instr));
		break;
	case 10: // cmpli: Compare Logical Immediate
		if (instr_crfD(instr) == 0 && instr_L(instr) == 0)
			ss << "cmplwi " << reg(instr_rA(instr)) << ", " << uimm(instr_UIMM(instr));
		else if (instr_L(instr) == 0)
			ss << "cmplwi " << crf(instr_crfD(instr)) << ", " << reg(instr_rA(instr)) << ", " << uimm(instr_UIMM(instr));
		else
			ss << "cmpli " << crf(instr_crfD(instr)) << ", " << instr_L(instr) << ", " << reg(instr_rA(instr)) << ", " << uimm(instr_UIMM(instr));
		break;
	case 11: // cmpi: Compare Immediate
		if (instr_crfD(instr) == 0 && instr_L(instr) == 0)
			ss << "cmpwi " << reg(instr_rA(instr)) << ", " << simm(instr_SIMM(instr));
		else if (instr_L(instr) == 0)
			ss << "cmpwi " << crf(instr_crfD(instr)) << ", " << reg(instr_rA(instr)) << ", " << simm(instr_SIMM(instr));
		else
			ss << "cmpi " << crf(instr_crfD(instr)) << ", " << instr_L(instr) << ", " << reg(instr_rA(instr)) << ", " << simm(instr_SIMM(instr));
		break;
	case 12: // addic: Add Immediate Carrying
		//if (instr_SIMM(instr) < 0)
		//	ss << "subic " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << hex((int16_t)-instr_SIMM(instr));
		//else
		ss << "addic " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << simm(instr_SIMM(instr));
		break;
	case 13: // addic.: Add Immediate Carrying and Record
		//if (instr_SIMM(instr) < 0)
		//	ss << "subic. " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << hex((int16_t)-instr_SIMM(instr));
		//else
		ss << "addic. " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << simm(instr_SIMM(instr));
		break;
	case 14: // addi: Add Immediate
		if (instr_rA(instr) == 0)
			ss << "li " << reg(instr_rD(instr)) << ", " << simm(instr_SIMM(instr));
		//else if (instr_SIMM(instr) < 0)
		//	ss << "subi " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << hex((int16_t)-instr_SIMM(instr));
		else
			ss << "addi " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << simm(instr_SIMM(instr));
		break;
	case 15: // addis: Add Immediate Shifted
		if (instr_rA(instr) == 0)
			ss << "lis " << reg(instr_rD(instr)) << ", " << simm(instr_SIMM(instr));
		else
			ss << "addis " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << simm(instr_SIMM(instr));
		break;
	case 16: // bc[x]: Branch Conditional
		if (instr_AA(instr) == 0 && instr_LK(instr) == 0)
		{
			if (instr_BO(instr) == 4 && instr_BI(instr) == 0)
				ss << "bge " << address(cia + instr_BD(instr), labelProv);
			else if (instr_BO(instr) == 4 && instr_BI(instr) == 1)
				ss << "ble " << address(cia + instr_BD(instr), labelProv);
			else if (instr_BO(instr) == 4 && instr_BI(instr) == 2)
				ss << "bne " << address(cia + instr_BD(instr), labelProv);
			else if (instr_BO(instr) == 4 && (instr_BI(instr) % 4) == 0)
				ss << "bge " << crf(instr_BI(instr) / 4) << ", " << address(cia + instr_BD(instr), labelProv);
			else if (instr_BO(instr) == 4 && (instr_BI(instr) % 4) == 1)
				ss << "ble " << crf(instr_BI(instr) / 4) << ", " << address(cia + instr_BD(instr), labelProv);
			else if (instr_BO(instr) == 4 && (instr_BI(instr) % 4) == 2)
				ss << "bne " << crf(instr_BI(instr) / 4) << ", " << address(cia + instr_BD(instr), labelProv);
			else if (instr_BO(instr) == 12 && instr_BI(instr) == 0)
				ss << "blt " << address(cia + instr_BD(instr), labelProv);
			else if (instr_BO(instr) == 12 && instr_BI(instr) == 1)
				ss << "bgt " << address(cia + instr_BD(instr), labelProv);
			else if (instr_BO(instr) == 12 && instr_BI(instr) == 2)
				ss << "beq " << address(cia + instr_BD(instr), labelProv);
			else if (instr_BO(instr) == 12 && (instr_BI(instr) % 4) == 0)
				ss << "blt " << crf(instr_BI(instr) / 4) << ", " << address(cia + instr_BD(instr), labelProv);
			else if (instr_BO(instr) == 12 && (instr_BI(instr) % 4) == 2)
				ss << "beq " << crf(instr_BI(instr) / 4) << ", " << address(cia + instr_BD(instr), labelProv);
			else if (instr_BO(instr) == 13 && instr_BI(instr) == 2)
				ss << "beq+ " << address(cia + instr_BD(instr), labelProv);
			else if (instr_BO(instr) == 16 && instr_BI(instr) == 0)
				ss << "bdnz " << address(cia + instr_BD(instr), labelProv);
			else
				ss << "bc " << instr_BO(instr) << ", " << instr_BI(instr) << ", " << address(cia + instr_BD(instr), labelProv);

			addrRef = cia + instr_BD(instr);
		}
		else if (instr_AA(instr) == 0 && instr_LK(instr) == 1)
			ss << "bcl " << instr_BO(instr) << ", " << instr_BI(instr) << ", " << address(cia + instr_BD(instr), labelProv);
		else
			ss << "ERROR: Unknown bc[x] AA = " << instr_AA(instr) << ", LK = " << instr_LK(instr);
		break;
	case 17: // sc: System Call
		ss << "sc";
		break;
	case 18: // b[x]: Branch
		if (instr_AA(instr) == 0 && instr_LK(instr) == 0)
		{
			ss << "b " << address(cia + instr_LI(instr), labelProv);
			addrRef = cia + instr_LI(instr);
		}
		else if (instr_AA(instr) == 0 && instr_LK(instr) == 1)
		{
			ss << "bl " << address(cia + instr_LI(instr), labelProv);
			addrRef = cia + instr_LI(instr);
		}
		else if (instr_AA(instr) == 1 && instr_LK(instr) == 1)
		{
			ss << "bla " << address(instr_LI(instr), labelProv);
			addrRef = instr_LI(instr);
		}
		else
			ss << "ERROR: Unknown b[x] AA = " << instr_AA(instr) << ", LK = " << instr_LK(instr);
		break;
	case 19:
		switch (instr_XO2130(instr))
		{
		case 16: // bclr[x]: Branch Conditional to Link Register
			if (instr_LK(instr) == 0)
			{
				if (instr_BO(instr) == 4 && instr_BI(instr) == 0)
					ss << "bgelr";
				else if (instr_BO(instr) == 4 && instr_BI(instr) == 1)
					ss << "blelr";
				else if (instr_BO(instr) == 4 && instr_BI(instr) == 2)
					ss << "bnelr";
				else if (instr_BO(instr) == 12 && instr_BI(instr) == 2)
					ss << "beqlr";
				else if (instr_BO(instr) == 12 && (instr_BI(instr) % 4) == 2)
					ss << "beqlr " << crf(instr_BI(instr) / 4);
				else if (instr_BO(instr) == 20 && instr_BI(instr) == 0)
					ss << "blr";
				else
					ss << "bclr " << instr_BO(instr) << ", " << instr_BI(instr);
			}
			else if (instr_LK(instr) == 1)
			{
				if (instr_BO(instr) == 20 && instr_BI(instr) == 0)
					ss << "blrl";
				else
					ss << "bclrl " << instr_BO(instr) << ", " << instr_BI(instr);
			}
			else
				ss << "ERROR: Unknown bclr[x] LK = " << instr_LK(instr);
			break;
		case 50: // rfi: Return from Interrupt
			ss << "rfi";
			break;
		case 150: // isync: Instruction Synchronize
			ss << "isync";
			break;
		case 193: // crxor: Condition Register XOR
			if (instr_crbD(instr) == instr_crbA(instr) && instr_crbA(instr) == instr_crbB(instr))
				ss << "crclr " << crb(instr_crbD(instr));
			else
				ss << "crxor " << crb(instr_crbD(instr)) << ", " << crb(instr_crbA(instr)) << ", " << crb(instr_crbB(instr));
			break;
		case 289: // creqv: Condition Register Equivalent
			ss << "creqv " << crb(instr_crbD(instr)) << ", " << crb(instr_crbA(instr)) << ", " << crb(instr_crbB(instr));
			break;
		case 449: // cror: Condition Register OR
			ss << "cror " << crb(instr_crbD(instr)) << ", " << crb(instr_crbA(instr)) << ", " << crb(instr_crbB(instr));
			break;
		case 528: // bcctr[x]: Branch Conditional to Count Register
			if (instr_LK(instr) == 0)
			{
				if (instr_BO(instr) == 20 && instr_BI(instr) == 0)
					ss << "bctr";
				else
					ss << "bcctr " << instr_BO(instr) << ", " << instr_BI(instr);
			}
			else if (instr_LK(instr) == 1)
			{
				if (instr_BO(instr) == 20 && instr_BI(instr) == 0)
					ss << "bctrl";
				else
					ss << "bcctrl " << instr_BO(instr) << ", " << instr_BI(instr);
			}
			else
				ss << "ERROR: Unknown bcctr[x] LK = " << instr_LK(instr);
			break;
		default:
			ss << "ERROR: Unknown OPCD:19 XO[21:30] " << instr_XO2130(instr) << " (" << bin(instr_XO2130(instr),10) << ')';
			break;
		}
		break;
	case 20: // rlwimi[x]: Rotate Left Word Immediate then Mask Insert
		if (instr_Rc(instr) == 0)
			ss << "rlwimi " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_SH(instr) << ", " << instr_MB(instr) << ", " << instr_ME(instr);
		else if (instr_Rc(instr) == 1)
			ss << "rlwimi. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_SH(instr) << ", " << instr_MB(instr) << ", " << instr_ME(instr);
		else
			ss << "ERROR: Unknown rlwimi[x] Rc = " << instr_Rc(instr);
		break;
	case 21: // rlwinm[x]: Rotate Left Word Immediate then AND with Mask
		if (instr_Rc(instr) == 0)
		{
			if (instr_SH(instr) == 0 && instr_MB(instr) == 0)
				ss << "clrrwi " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << (31-instr_ME(instr));
			else if (instr_SH(instr) == 0 && instr_ME(instr) == 31)
				ss << "clrlwi " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_MB(instr);
			else if (instr_MB(instr) == 0 && instr_ME(instr) == 31)
				ss << "rotlwi " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_SH(instr);
			else if (instr_SH(instr) == 31-instr_ME(instr) && instr_MB(instr) == 0)
				ss << "slwi " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_SH(instr);
			else if (32-instr_SH(instr) == instr_MB(instr) && instr_ME(instr) == 31)
				ss << "srwi " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_MB(instr);
			else if (31-instr_SH(instr) == instr_ME(instr))
				ss << "clrlslwi " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << (instr_MB(instr)+instr_SH(instr)) << ", " << instr_SH(instr);
			else if (instr_ME(instr) == 31)
				ss << "extrwi " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << (32-instr_MB(instr)) << ", " << (instr_SH(instr) - (32-instr_MB(instr)));
			else
				ss << "rlwinm " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_SH(instr) << ", " << instr_MB(instr) << ", " << instr_ME(instr);
		}
		else
		{
			if (instr_SH(instr) == 0 && instr_MB(instr) == 0)
				ss << "clrrwi. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << (31-instr_ME(instr));
			else if (instr_SH(instr) == 0 && instr_ME(instr) == 31)
				ss << "clrlwi. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_MB(instr);
			else if (instr_MB(instr) == 0 && instr_ME(instr) == 31)
				ss << "rotlwi. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_SH(instr);
			else if (instr_SH(instr) == 31-instr_ME(instr) && instr_MB(instr) == 0)
				ss << "slwi. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_SH(instr);
			else if (32-instr_SH(instr) == instr_MB(instr) && instr_ME(instr) == 31)
				ss << "srwi. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_MB(instr);
			else if (31-instr_SH(instr) == instr_ME(instr))
				ss << "clrlslwi. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << (instr_MB(instr)+instr_SH(instr)) << ", " << instr_SH(instr);
			else if (instr_ME(instr) == 31)
				ss << "extrwi. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << (32-instr_MB(instr)) << ", " << (instr_SH(instr) - (32-instr_MB(instr)));
			else
				ss << "rlwinm. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_SH(instr) << ", " << instr_MB(instr) << ", " << instr_ME(instr);
		}
		break;
	case 23: // rlwnm[x]: Rotate Left Word then AND with Mask
		if (instr_Rc(instr) == 0)
			ss << "rlwnm " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr)) << ", " << instr_MB(instr) << ", " << instr_ME(instr);
		else if (instr_Rc(instr) == 1)
			ss << "rlwnm. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr)) << ", " << instr_MB(instr) << ", " << instr_ME(instr);
		else
			ss << "ERROR: Unknown rlwnm[x] Rc = " << instr_Rc(instr);
		break;
	case 24: // ori: OR Immediate
		if (instr_rA(instr) == 0 && instr_rS(instr) == 0 && instr_UIMM(instr) == 0)
			ss << "nop";
		else
			ss << "ori " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << uimm(instr_UIMM(instr));
		break;
	case 25: // oris: OR Immediate Shifted
		ss << "oris " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << uimm(instr_UIMM(instr));
		break;
	case 26: // xori: XOR Immediate
		ss << "xori " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << uimm(instr_UIMM(instr));
		break;
	case 27: // xoris: XOR Immediate Shifted
		ss << "xoris " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << uimm(instr_UIMM(instr));
		break;
	case 28: // andi.: AND Immediate
		ss << "andi. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << uimm(instr_UIMM(instr));
		break;
	case 29: // andis.: AND Immediate Shifted
		ss << "andis. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << uimm(instr_UIMM(instr));
		break;
	case 31:
		switch (instr_XO2230(instr))
		{
		case 8: // subfc[x]: Subtract from Carrying
			if (instr_OE(instr) == 0 && instr_Rc(instr) == 0)
				ss << "subfc " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else if (instr_OE(instr) == 0 && instr_Rc(instr) == 1)
				ss << "subfc. " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else
				ss << "ERROR: Unknown subfc[x] OE = " << instr_OE(instr) << ", Rc = " << instr_Rc(instr);
			break;
		case 10: // addc[x]: Add Carrying
			if (instr_OE(instr) == 0 && instr_Rc(instr) == 0)
				ss << "addc " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else
				ss << "ERROR: Unknown addc[x] OE = " << instr_OE(instr) << ", Rc = " << instr_Rc(instr);
			break;
		case 11: // mulhwu[x]: Multiply High Word Unsigned
			if (instr_Rc(instr) == 0)
				ss << "mulhwu " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else
				ss << "ERROR: Unknown mulhwu[x] Rc = " << instr_Rc(instr);
			break;
		case 40: // subf[x]: Subtract From
			if (instr_OE(instr) == 0 && instr_Rc(instr) == 0)
				ss << "subf " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else if (instr_OE(instr) == 0 && instr_Rc(instr) == 1)
				ss << "subf. " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else
				ss << "ERROR: Unknown subf[x] OE = " << instr_OE(instr) << ", Rc = " << instr_Rc(instr);
			break;
		case 75: // mulhw[x]: Multiply High Word
			if (instr_Rc(instr) == 0)
				ss << "mulhw " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else
				ss << "ERROR: Unknown mulhw[x] Rc = " << instr_Rc(instr);
			break;
		case 104: // neg[x]: Negate
			if (instr_OE(instr) == 0 && instr_Rc(instr) == 0)
				ss << "neg " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr));
			else if (instr_OE(instr) == 0 && instr_Rc(instr) == 1)
				ss << "neg. " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr));
			else
				ss << "ERROR: Unknown neg[x] OE = " << instr_OE(instr) << ", Rc = " << instr_Rc(instr);
			break;
		case 136: // subfe[x]: Subtract from Extended
			if (instr_OE(instr) == 0 && instr_Rc(instr) == 0)
				ss << "subfe " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else if (instr_OE(instr) == 0 && instr_Rc(instr) == 1)
				ss << "subfe. " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else
				ss << "ERROR: Unknown subfe[x] OE = " << instr_OE(instr) << ", Rc = " << instr_Rc(instr);
			break;
		case 138: // adde[x]: Add Extended
			if (instr_OE(instr) == 0 && instr_Rc(instr) == 0)
				ss << "adde " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else
				ss << "ERROR: Unknown adde[x] OE = " << instr_OE(instr) << ", Rc = " << instr_Rc(instr);
			break;
		case 200: // subfze[x]: Subtract from Zero Extended
			if (instr_OE(instr) == 0 && instr_Rc(instr) == 0)
				ss << "subfze " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr));
			else
				ss << "ERROR: Unknown subfze[x] OE = " << instr_OE(instr) << ", Rc = " << instr_Rc(instr);
			break;
		case 202: // addze[x]: Add to Zero Extended
			if (instr_OE(instr) == 0 && instr_Rc(instr) == 0)
				ss << "addze " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr));
			else if (instr_OE(instr) == 0 && instr_Rc(instr) == 1)
				ss << "addze. " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr));
			else
				ss << "ERROR: Unknown addze[x] OE = " << instr_OE(instr) << ", Rc = " << instr_Rc(instr);
			break;
		case 235: // mullw[x]: Multiply Low Word
			if (instr_OE(instr) == 0 && instr_Rc(instr) == 0)
				ss << "mullw " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else if (instr_OE(instr) == 0 && instr_Rc(instr) == 1)
				ss << "mullw. " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else
				ss << "ERROR: Unknown mullw[x] OE = " << instr_OE(instr) << ", Rc = " << instr_Rc(instr);
			break;
		case 266: // add[x]: Add
			if (instr_OE(instr) == 0 && instr_Rc(instr) == 0)
				ss << "add " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else if (instr_OE(instr) == 0 && instr_Rc(instr) == 1)
				ss << "add. " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else
				ss << "ERROR: Unknown add[x] OE = " << instr_OE(instr) << ", Rc = " << instr_Rc(instr);
			break;
		case 459: // divwu[x]: Divide Word Unsigned
			if (instr_OE(instr) == 0 && instr_Rc(instr) == 0)
				ss << "divwu " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else if (instr_OE(instr) == 0 && instr_Rc(instr) == 1)
				ss << "divwu. " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else
				ss << "ERROR: Unknown divwu[x] OE = " << instr_OE(instr) << ", Rc = " << instr_Rc(instr);
			break;
		case 491: // divw[x]: Divide Word
			if (instr_OE(instr) == 0 && instr_Rc(instr) == 0)
				ss << "divw " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else if (instr_OE(instr) == 0 && instr_Rc(instr) == 1)
				ss << "divw. " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
			else
				ss << "ERROR: Unknown divw[x] OE = " << instr_OE(instr) << ", Rc = " << instr_Rc(instr);
			break;
		default:
			switch (instr_XO2130(instr))
			{
			case 0: // cmp: Compare
				if (instr_crfD(instr) == 0 && instr_L(instr) == 0)
					ss << "cmpw " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				else if (instr_L(instr) == 0)
					ss << "cmpw " << crf(instr_crfD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				else
					ss << "cmp " << crf(instr_crfD(instr)) << ", " << instr_L(instr) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 19: // mfcr: Move from Condition Register
				ss << "mfcr " << reg(instr_rD(instr));
				break;
			case 23: // lwzx: Load Word and Zero Indexed
				ss << "lwzx " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 24: // slw[x]: Shift Left Word
				if (instr_Rc(instr) == 0)
					ss << "slw " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				else if (instr_Rc(instr) == 1)
					ss << "slw. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				else
					ss << "ERROR: Unknown slw[x] Rc = " << instr_Rc(instr);
				break;
			case 26: // cntlzw[x]: Count Leading Zeros Word
				if (instr_Rc(instr) == 0)
					ss << "cntlzw " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr));
				else
					ss << "ERROR: Unknown cntlzw[x] Rc = " << instr_Rc(instr);
				break;
			case 28: // and[x]: AND
				if (instr_Rc(instr) == 0)
					ss << "and " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				else if (instr_Rc(instr) == 1)
					ss << "and. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				else
					ss << "ERROR: Unknown and[x] Rc = " << instr_Rc(instr);
				break;
			case 32: // cmpl: Compare Logical
				if (instr_crfD(instr) == 0 && instr_L(instr) == 0)
					ss << "cmplw " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				else if (instr_L(instr) == 0)
					ss << "cmplw " << crf(instr_crfD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				else
					ss << "cmpl " << crf(instr_crfD(instr)) << ", " << instr_L(instr) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 54: // dcbst: Data Cache Block Store
				ss << "dcbst " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 55: // lwzux: Load Word and Zero with Update Indexed
				ss << "lwzux " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 60: // andc[x]: AND with Complement
				if (instr_Rc(instr) == 0)
					ss << "andc " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				else
					ss << "ERROR: Unknown andc[x] Rc = " << instr_Rc(instr);
				break;
			case 83: // mfmsr: Move from Machine State Register
				ss << "mfmsr " << reg(instr_rD(instr));
				break;
			case 86: // dcbf: Data Cache Block Flush
				ss << "dcbf " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 87: // lbzx: Load Byte and Zero Indexed
				ss << "lbzx " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 119: // lbzux: Load Byte and Zero with Update Indexed
				ss << "lbzux " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 124: // nor[x]: NOR
				if (instr_Rc(instr) == 0)
				{
					if (instr_rS(instr) == instr_rB(instr))
						ss << "not " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr));
					else
						ss << "nor " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				}
				else
					ss << "ERROR: Unknown nor[x] Rc = " << instr_Rc(instr);
				break;
			case 144: // mtcrf: Move to Condition Register Fields
				if (instr_CRM(instr) == 0xFF)
					ss << "mtcr " << reg(instr_rS(instr));
				else
					ss << "mtcrf " << crm(instr_CRM(instr)) << ", " << reg(instr_rS(instr));
				break;
			case 146: // mtmsr: Move to Machine State Register
				ss << "mtmsr " << reg(instr_rS(instr));
				break;
			case 151: // stwx: Store Word Indexed
				ss << "stwx " << reg(instr_rS(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 183: // stwux: Store Word with Update Indexed
				ss << "stwux " << reg(instr_rS(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 210: // mtsr: Move to Segment Register
				ss << "mtsr " << instr_SR(instr) << ", " << reg(instr_rS(instr));
				break;
			case 215: // stbx: Store Byte Indexed
				ss << "stbx " << reg(instr_rS(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 247: // stbux: Store Byte with Update Indexed
				ss << "stbux " << reg(instr_rS(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 278: // dcbt: Data Cache Block Touch
				ss << "dcbt " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 279: // lhzx: Load Half Word and Zero Indexed
				ss << "lhzx " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 284: // eqv[x]: Equivalent
				if (instr_Rc(instr) == 0)
					ss << "eqv " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				else
					ss << "ERROR: Unknown eqv[x] Rc = " << instr_Rc(instr);
				break;
			case 311: // lhzux: Load Half Word and Zero with Update Indexed
				ss << "lhzux " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 316: // xor[x]: XOR
				if (instr_Rc(instr) == 0)
					ss << "xor " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				else if (instr_Rc(instr) == 1)
					ss << "xor. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				else
					ss << "ERROR: Unknown xor[x] Rc = " << instr_Rc(instr);
				break;
			case 339: // mfspr: Move from Special-Purpose Register
				if (instr_spr(instr) == 8)
					ss << "mflr " << reg(instr_rD(instr));
				else if (instr_spr(instr) == 9)
					ss << "mfctr " << reg(instr_rD(instr));
				else if (instr_spr(instr) == 26)
					ss << "mfsrr0 " << reg(instr_rD(instr));
				else if (instr_spr(instr) == 27)
					ss << "mfsrr1 " << reg(instr_rD(instr));
				else if (instr_spr(instr) >= 272 && instr_spr(instr) <= 275)
					ss << "mfsprg " << reg(instr_rD(instr)) << ", " << (instr_spr(instr)-272);
				else
					ss << "mfspr " << reg(instr_rD(instr)) << ", " << instr_spr(instr);
				break;
			case 343: // lhax: Load Half Word Algebraic Indexed
				ss << "lhax " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 371: // mftb: Move from Time Base
				if (instr_tbr(instr) == 268)
					ss << "mftb " << reg(instr_rD(instr));
				else
					ss << "mftb " << reg(instr_rD(instr)) << ", " << instr_tbr(instr);
				break;
			case 407: // sthx: Store Half Word Indexed
				ss << "sthx " << reg(instr_rS(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 412: // orc[x]: OR with Complement
				if (instr_Rc(instr) == 0)
					ss << "orc " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				else
					ss << "ERROR: Unknown orc[x] Rc = " << instr_Rc(instr);
				break;
			case 444: // or[x]: OR
				if (instr_Rc(instr) == 0)
				{
					if (instr_rS(instr) == instr_rB(instr))
						ss << "mr " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr));
					else
						ss << "or " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				}
				else if (instr_Rc(instr) == 1)
				{
					if (instr_rS(instr) == instr_rB(instr))
						ss << "mr. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr));
					else
						ss << "or. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				}
				else
					ss << "ERROR: Unknown or[x] Rc = " << instr_Rc(instr);
				break;
			case 467: // mtspr: Move to Special-Purpose Register
				if (instr_spr(instr) == 1)
					ss << "mtxer " << reg(instr_rS(instr));
				else if (instr_spr(instr) == 8)
					ss << "mtlr " << reg(instr_rS(instr));
				else if (instr_spr(instr) == 9)
					ss << "mtctr " << reg(instr_rS(instr));
				else if (instr_spr(instr) == 26)
					ss << "mtsrr0 " << reg(instr_rS(instr));
				else if (instr_spr(instr) == 27)
					ss << "mtsrr1 " << reg(instr_rS(instr));
				else if (instr_spr(instr) >= 272 && instr_spr(instr) <= 275)
					ss << "mtsprg " << (instr_spr(instr)-272) << ", " << reg(instr_rS(instr));
				else
					ss << "mtspr " << instr_spr(instr) << ", " << reg(instr_rS(instr));
				break;
			case 470: // dcbi: Data Cache Block Invalidate
				ss << "dcbi " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 476: // nand[x]: NAND
				if (instr_Rc(instr) == 0)
					ss << "nand " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				else
					ss << "ERROR: Unknown nand[x] Rc = " << instr_Rc(instr);
				break;
			case 512: // mcrxr: Move to Condition Register from XER
				ss << "mcrxr " << crf(instr_crfD(instr));
				break;
			case 535: // lfsx: Load Floating-Point Single Indexed
				ss << "lfsx " << freg(instr_frD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 536: // srw[x]: Shift Right Word
				if (instr_Rc(instr) == 0)
					ss << "srw " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				else
					ss << "ERROR: Unknown srw[x] Rc = " << instr_Rc(instr);
				break;
			case 567: // lfsux: Load Floating-Point Single with Update Indexed
				ss << "lfsux " << freg(instr_frD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 595: // mfsr: Move from Segment Register
				ss << "mfsr " << reg(instr_rD(instr)) << ", " << instr_SR(instr);
				break;
			case 598: // sync: Synchronize
				ss << "sync";
				break;
			case 599: // lfdx: Load Floating-Point Double Indexed
				ss << "lfdx " << freg(instr_frD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 631: // lfdux: Load Floating-Point Double with Update Indexed
				ss << "lfdux " << freg(instr_frD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 663: // stfsx: Store Floating-Point Single Indexed
				ss << "stfsx " << freg(instr_frS(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 695: // stfsux: Store Floating-Point Single with Update Indexed
				ss << "stfsux " << freg(instr_frS(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 727: // stfdx: Store Floating-Point Double Indexed
				ss << "stfdx " << freg(instr_frS(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 790: // lhbrx: Load Half Word Byte-Reverse Indexed
				ss << "lhbrx " << reg(instr_rD(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 792: // sraw[x]: Shift Right Algebraic Word
				if (instr_Rc(instr) == 0)
					ss << "sraw " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				else if (instr_Rc(instr) == 1)
					ss << "sraw. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << reg(instr_rB(instr));
				else
					ss << "ERROR: Unknown sraw[x] Rc = " << instr_Rc(instr);
				break;
			case 824: // srawi[x]: Shift Right Algebraic Word Immediate
				if (instr_Rc(instr) == 0)
					ss << "srawi " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_SH(instr);
				else if (instr_Rc(instr) == 1)
					ss << "srawi. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr)) << ", " << instr_SH(instr);
				else
					ss << "ERROR: Unknown srawi[x] Rc = " << instr_Rc(instr);
				break;
			case 918: // sthbrx: Store Half Word Byte-Reverse Indexed
				ss << "sthbrx " << reg(instr_rS(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 922: // extsh[x]: Extend Sign Half Word
				if (instr_Rc(instr) == 0)
					ss << "extsh " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr));
				else if (instr_Rc(instr) == 1)
					ss << "extsh. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr));
				else
					ss << "ERROR: Unknown extsh[x] Rc = " << instr_Rc(instr);
				break;
			case 954: // extsb[x]: Extend Sign Byte
				if (instr_Rc(instr) == 0)
					ss << "extsb " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr));
				else if (instr_Rc(instr) == 1)
					ss << "extsb. " << reg(instr_rA(instr)) << ", " << reg(instr_rS(instr));
				else
					ss << "ERROR: Unknown extsb[x] Rc = " << instr_Rc(instr);
				break;
			case 982: // icbi: Instruction Cache Block Invalidate
				ss << "icbi " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 983: // stfiwx: Store Floating-Point as Integer Word Indexed
				ss << "stfiwx " << freg(instr_frS(instr)) << ", " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			case 1014: // dcbz: Data Cache Block Clear to Zero
				ss << "dcbz " << reg(instr_rA(instr)) << ", " << reg(instr_rB(instr));
				break;
			default:
				ss << "ERROR: Unknown OPCD:31 XO[21:30] " << instr_XO2130(instr) << " (" << bin(instr_XO2130(instr),10) << ')';
				break;
			}
			break;
		}
		break;
	case 32: // lwz: Load Word and Zero
		ss << "lwz " << reg(instr_rD(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 33: // lwzu: Load Word and Zero with Update
		ss << "lwzu " << reg(instr_rD(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 34: // lbz: Load Byte and Zero
		ss << "lbz " << reg(instr_rD(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 35: // lbzu: Load Byte and Zero with Update
		ss << "lbzu " << reg(instr_rD(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 36: // stw: Store Word
		ss << "stw " << reg(instr_rS(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 37: // stwu: Store Word with Update
		ss << "stwu " << reg(instr_rS(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 38: // stb: Store Byte
		ss << "stb " << reg(instr_rS(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 39: // stbu: Store Byte with Update
		ss << "stbu " << reg(instr_rS(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 40: // lhz: Load Half Word and Zero
		ss << "lhz " << reg(instr_rD(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 41: // lhzu: Load Half Word and Zero with Update
		ss << "lhzu " << reg(instr_rD(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 42: // lha: Load Half Word Algebraic
		ss << "lha " << reg(instr_rD(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 43: // lhau: Load Half Word Algebraic with Update
		ss << "lhau " << reg(instr_rD(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 44: // sth: Store Half Word
		ss << "sth " << reg(instr_rS(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 45: // sthu: Store Half Word with Update
		ss << "sthu " << reg(instr_rS(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 46: // lmw: Load Multiple Word
		ss << "lmw " << reg(instr_rD(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 47: // stmw: Store Multiple Word
		ss << "stmw " << reg(instr_rS(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 48: // lfs: Load Floating-Point Single
		ss << "lfs " << freg(instr_frD(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 49: // lfsu: Load Floating-Point Single with Update
		ss << "lfsu " << freg(instr_frD(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 50: // lfd: Load Floating-Point Double
		ss << "lfd " << freg(instr_frD(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 51: // lfdu: Load Floating-Point Double with Update
		ss << "lfdu " << freg(instr_frD(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 52: // stfs: Store Floating-Point Single
		ss << "stfs " << freg(instr_frS(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 53: // stfsu: Store Floating-Point Single with Update
		ss << "stfsu " << freg(instr_frS(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 54: // stfd: Store Floating-Point Double
		ss << "stfd " << freg(instr_frS(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 55: // stfdu: Store Floating-Point Double with Update
		ss << "stfdu " << freg(instr_frS(instr)) << ", " << displ(instr_d1631(instr)) << '(' << reg(instr_rA(instr)) << ')';
		break;
	case 56: // psq_l: Paired Single Quantized Load
		ss << "psq_l " << freg(instr_frD(instr)) << ", " << displ(instr_d2031(instr)) << '(' << reg(instr_rA(instr)) << "), " << instr_W16(instr) << ", " << instr_I1719(instr);
		break;
	case 57: // psq_lu: Paired Single Quantized Load with Update
		ss << "psq_lu " << freg(instr_frD(instr)) << ", " << displ(instr_d2031(instr)) << '(' << reg(instr_rA(instr)) << "), " << instr_W16(instr) << ", " << instr_I1719(instr);
		break;
	case 59:
		switch (instr_XO2630(instr))
		{
		case 18: // fdivs[x]: Floating Divide Single
			if (instr_Rc(instr) == 0)
				ss << "fdivs " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fdivs[x] Rc = " << instr_Rc(instr);
			break;
		case 20: // fsubs[x]: Floating Subtract Single
			if (instr_Rc(instr) == 0)
				ss << "fsubs " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fsubs[x] Rc = " << instr_Rc(instr);
			break;
		case 21: // fadds[x]: Floating Add Single
			if (instr_Rc(instr) == 0)
				ss << "fadds " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fadds[x] Rc = " << instr_Rc(instr);
			break;
		case 24: // fres[x]: Floating Reciprocal Estimate Single
			if (instr_Rc(instr) == 0)
				ss << "fres " << freg(instr_frD(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fres[x] Rc = " << instr_Rc(instr);
			break;
		case 25: // fmuls[x]: Floating Multiply Single
			if (instr_Rc(instr) == 0)
				ss << "fmuls " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr));
			else
				ss << "ERROR: Unknown fmuls[x] Rc = " << instr_Rc(instr);
			break;
		case 28: // fmsubs[x]: Floating Multiply-Subtract Single
			if (instr_Rc(instr) == 0)
				ss << "fmsubs " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fmsubs[x] Rc = " << instr_Rc(instr);
			break;
		case 29: // fmadds[x]: Floating Multiply-Add Single
			if (instr_Rc(instr) == 0)
				ss << "fmadds " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fmadds[x] Rc = " << instr_Rc(instr);
			break;
		case 30: // fnmsubs[x]: Floating Negative Multiply-Subtract Single
			if (instr_Rc(instr) == 0)
				ss << "fnmsubs " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fnmsubs[x] Rc = " << instr_Rc(instr);
			break;
		case 31: // fnmadds[x]: Floating Negative Multiply-Add Single
			if (instr_Rc(instr) == 0)
				ss << "fnmadds " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fnmadds[x] Rc = " << instr_Rc(instr);
			break;
		default:
			ss << "ERROR: Unknown OPCD:59 XO[26:30] " << instr_XO2630(instr) << " (" << bin(instr_XO2630(instr),5) << ')';
			break;
		}
		break;
	case 60: // psq_st: Paired Single Quantized Store
		ss << "psq_st " << freg(instr_frS(instr)) << ", " << displ(instr_d2031(instr)) << '(' << reg(instr_rA(instr)) << "), " << instr_W16(instr) << ", " << instr_I1719(instr);
		break;
	case 61: // psq_stu: Paired Single Quantized Store with update
		ss << "psq_stu " << freg(instr_frS(instr)) << ", " << displ(instr_d2031(instr)) << '(' << reg(instr_rA(instr)) << "), " << instr_W16(instr) << ", " << instr_I1719(instr);
		break;
	case 63:
		switch (instr_XO2630(instr))
		{
		case 18: // fdiv[x]: Floating Divide (Double-Precision)
			if (instr_Rc(instr) == 0)
				ss << "fdiv " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fdiv[x] Rc = " << instr_Rc(instr);
			break;
		case 20: // fsub[x]: Floating Subtract (Double-Precision)
			if (instr_Rc(instr) == 1)
				ss << "fsub. " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
			else if (instr_Rc(instr) == 0)
				ss << "fsub " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fsub[x] Rc = " << instr_Rc(instr);
			break;
		case 21: // fadd[x]: Floating Add (Double-Precision)
			if (instr_Rc(instr) == 0)
				ss << "fadd " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fadd[x] Rc = " << instr_Rc(instr);
			break;
		case 23: // fsel[x]: Floating Select
			if (instr_Rc(instr) == 0)
				ss << "fsel " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fsel[x] Rc = " << instr_Rc(instr);
			break;
		case 25: // fmul[x]: Floating Multiply (Double-Precision)
			if (instr_Rc(instr) == 0)
				ss << "fmul " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr));
			else
				ss << "ERROR: Unknown fmul[x] Rc = " << instr_Rc(instr);
			break;
		case 26: // frsqrte[x]: Floating Reciprocal Square Root Estimate
			if (instr_Rc(instr) == 0)
				ss << "frsqrte " << freg(instr_frD(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown frsqrte[x] Rc = " << instr_Rc(instr);
			break;
		case 28: // fmsub[x]: Floating Multiply-Subtract (Double-Precision)
			if (instr_Rc(instr) == 0)
				ss << "fmsub " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fmsub[x] Rc = " << instr_Rc(instr);
			break;
		case 29: // fmadd[x]: Floating Multiply-Add (Double-Precision)
			if (instr_Rc(instr) == 0)
				ss << "fmadd " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fmadd[x] Rc = " << instr_Rc(instr);
			break;
		case 30: // fnmsub[x]: Floating Negative Multiply-Subtract (Double-Precision)
			if (instr_Rc(instr) == 0)
				ss << "fnmsub " << freg(instr_frD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frC(instr)) << ", " << freg(instr_frB(instr));
			else
				ss << "ERROR: Unknown fnmsub[x] Rc = " << instr_Rc(instr);
			break;
		default:
			switch (instr_XO2130(instr))
			{
			case 0: // fcmpu: Floating Compare Unordered
				if (instr_crfD(instr) == 0)
					ss << "fcmpu " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
				else
					ss << "fcmpu " << crf(instr_crfD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
				break;
			case 12: // frsp[x]: Floating Round to Single
				if (instr_Rc(instr) == 0)
					ss << "frsp " << freg(instr_frD(instr)) << ", " << freg(instr_frB(instr));
				else
					ss << "ERROR: Unknown frsp[x] Rc = " << instr_Rc(instr);
				break;
			case 14: // fctiw[x]: Floating Convert to Integer Word
				ss << "ERROR: Unknown fctiw[x] Rc = " << instr_Rc(instr);
				break;
			case 15: // fctiwz[x]: Floating Convert to Integer Word with Round toward Zero
				if (instr_Rc(instr) == 0)
					ss << "fctiwz " << freg(instr_frD(instr)) << ", " << freg(instr_frB(instr));
				else
					ss << "ERROR: Unknown fctiwz[x] Rc = " << instr_Rc(instr);
				break;
			case 32: // fcmpo: Floating Compare Ordered
				if (instr_crfD(instr) == 0)
					ss << "fcmpo " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
				else
					ss << "fcmpo " << crf(instr_crfD(instr)) << ", " << freg(instr_frA(instr)) << ", " << freg(instr_frB(instr));
				break;
			case 38: // mtfsb1[x]: Move to FPSCR Bit 1
				if (instr_Rc(instr) == 0)
					ss << "mtfsb1 " << crb(instr_crbD(instr));
				else
					ss << "ERROR: Unknown mtfsb1[x] Rc = " << instr_Rc(instr);
				break;
			case 40: // fneg[x]: Floating Negate
				if (instr_Rc(instr) == 0)
					ss << "fneg " << freg(instr_frD(instr)) << ", " << freg(instr_frB(instr));
				else
					ss << "ERROR: Unknown fneg[x] Rc = " << instr_Rc(instr);
				break;
			case 72: // fmr[x]: Floating Move Register
				if (instr_Rc(instr) == 0)
					ss << "fmr " << freg(instr_frD(instr)) << ", " << freg(instr_frB(instr));
				else
					ss << "ERROR: Unknown fmr[x] Rc = " << instr_Rc(instr);
				break;
			case 264: // fabs[x]: Floating Absolute Value
				if (instr_Rc(instr) == 0)
					ss << "fabs " << freg(instr_frD(instr)) << ", " << freg(instr_frB(instr));
				else
					ss << "ERROR: Unknown fabs[x] Rc = " << instr_Rc(instr);
				break;
			case 583: // mffs[x]: Move from FPSCR
				if (instr_Rc(instr) == 0)
					ss << "mffs " << freg(instr_frD(instr));
				else
					ss << "ERROR: Unknown mffs[x] Rc = " << instr_Rc(instr);
				break;
			case 711: // mtfsf[x]: Move to FPSCR Fields
				if (instr_Rc(instr) == 0)
					ss << "mtfsf " << fm(instr_FM(instr)) << ", " << freg(instr_frB(instr));
				else
					ss << "ERROR: Unknown mtfsf[x] Rc = " << instr_Rc(instr);
				break;
			default:
				ss << "ERROR: Unknown OPCD:63 XO[21:30] " << instr_XO2130(instr) << " (" << bin(instr_XO2130(instr),10) << ')';
				break;
			}
			break;
		}
		break;
	default:
		ss << "ERROR: Unknown OPCD " << opcd << " (" << bin(opcd,6) << ')';
		break;
	}

	return ss.str();
}
