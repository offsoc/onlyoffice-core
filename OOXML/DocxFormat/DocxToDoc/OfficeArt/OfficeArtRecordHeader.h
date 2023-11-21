
#pragma once

#include "../../../../DesktopEditor/common/Types.h"

namespace OfficeArt
{
	class OfficeArtRecordHeader
	{
	private:

		struct
		{
			unsigned int recVer:4;
			unsigned int recInstance:12;
			unsigned int recType:16;
			unsigned int recLen:32;
		} OfficeArtRecordHeaderStruct;

	public:

		OfficeArtRecordHeader()
		{
			OfficeArtRecordHeaderStruct.recVer		=	0;
			OfficeArtRecordHeaderStruct.recInstance	=	0;
			OfficeArtRecordHeaderStruct.recType		=	0xF000;
			OfficeArtRecordHeaderStruct.recLen		=	0;
		}

		explicit OfficeArtRecordHeader (BYTE _recVer, unsigned short _recInstance, unsigned short _recType, unsigned int _recLen)
		{
			OfficeArtRecordHeaderStruct.recVer		=	_recVer;
			OfficeArtRecordHeaderStruct.recInstance	=	_recInstance;

			if ( ( _recType >= 0xF000 ) && ( _recType <= 0xFFFF ) )
			{
				OfficeArtRecordHeaderStruct.recType	=	_recType;
			}
			else
			{
				OfficeArtRecordHeaderStruct.recType	=	0xF000; 
			}

			OfficeArtRecordHeaderStruct.recLen		=	_recLen;
		}

		inline BYTE GetVersion() const
		{
			return OfficeArtRecordHeaderStruct.recVer;
		}

		inline unsigned short GetInstance() const
		{
			return OfficeArtRecordHeaderStruct.recInstance;
		}

		inline unsigned short GetType() const
		{
			return OfficeArtRecordHeaderStruct.recType;
		}

		inline unsigned int GetLength() const
		{
			return OfficeArtRecordHeaderStruct.recLen;
		}

		inline operator const BYTE* () const
		{
			return (const BYTE*)(&OfficeArtRecordHeaderStruct);
		}

		inline operator BYTE* () const
		{
			return (BYTE*)(&OfficeArtRecordHeaderStruct);
		}
	};
}
