
#pragma once

#include "../../../DesktopEditor/common/Types.h"

namespace Docx2Doc
{
union DTTM
{
private:
	struct
	{
		unsigned int mint:6;
		unsigned int hr:5;
		unsigned int dom:5;
		unsigned int mon:4;
		unsigned int yr:9;
		unsigned int wdy:3;
	} DTTMStruct;
	unsigned int DTTMUnsignedInt;

public:
	explicit DTTM( unsigned int _dttm = 0 ):
		DTTMUnsignedInt(_dttm)
	{
	}

	DTTM( BYTE _mint, BYTE _hr, BYTE _dom, BYTE _mon, unsigned short _yr, BYTE _wdy ):
		DTTMUnsignedInt(0)
	{
		this->DTTMStruct.mint = _mint;
		this->DTTMStruct.hr = _hr;
		this->DTTMStruct.dom = _dom;
		this->DTTMStruct.mon = _mon;
		this->DTTMStruct.yr = _yr;
		this->DTTMStruct.wdy = _wdy;
	}

	operator unsigned int () const
	{
		return this->DTTMUnsignedInt;
	}
};
}
