#pragma once

#include "OfficeArtFOPTEOPID.h"
#include "../../../../DesktopEditor/common/Types.h"

namespace OfficeArt
{
	class OfficeArtFOPTE
	{
	public:
		OfficeArtFOPTE () : opid( 0, false, false ), op(0)
		{
			memset( this->bytes, 0, sizeof(this->bytes) );
		}

		explicit OfficeArtFOPTE( OfficeArtFOPTEOPID _opid, int _op ) : opid(_opid), op(_op)
		{
			memset( this->bytes, 0, sizeof(this->bytes) );

			unsigned short opidValue = this->opid.GetValue();

			memcpy( this->bytes, &opidValue, sizeof(opidValue) );
			memcpy( ( this->bytes + sizeof(opidValue) ), &(this->op), sizeof(this->op) );
		}

		OfficeArtFOPTE (const OfficeArtFOPTE& _officeArtFOPTE) : opid(_officeArtFOPTE.opid), op(_officeArtFOPTE.op)
		{
			memset( this->bytes, 0, sizeof(this->bytes) );
			memcpy( this->bytes, _officeArtFOPTE.bytes, sizeof(_officeArtFOPTE.bytes) );
		}

		OfficeArtFOPTEOPID GetOpid() const
		{
			return this->opid;
		}

		int GetValue() const
		{
			return this->op;
		}

		virtual operator BYTE*() const
		{
			return (BYTE*)(this->bytes);
		}

		virtual operator const BYTE*() const
		{
			return (const BYTE*)(this->bytes);
		}

		virtual unsigned int Size() const
		{
			return (unsigned int)sizeof(this->bytes);
		}

		virtual ~OfficeArtFOPTE()
		{
		}

	protected:

		OfficeArtFOPTEOPID opid;
		int op;

		static const BYTE SIZE_IN_BYTES = ( sizeof(OfficeArtFOPTEOPID) + sizeof(int) );
		BYTE bytes[SIZE_IN_BYTES];
	};
}
