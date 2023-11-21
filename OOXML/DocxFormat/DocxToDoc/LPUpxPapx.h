#pragma once

#include "Prl.h"

namespace Docx2Doc
{
	class UpxPapx: public IOperand
	{
	public:

		UpxPapx() : istd(0), bytes(NULL), sizeInBytes(0), sizeInBytesWithoutPadding(0)
		{
		}

		explicit UpxPapx( unsigned short _istd, const std::vector<Prl>& _grpprlPapx ) : istd(_istd), grpprlPapx(_grpprlPapx), bytes(NULL), sizeInBytes(0), sizeInBytesWithoutPadding(0)
		{
			this->sizeInBytesWithoutPadding = sizeof(this->istd);

			for ( std::vector<Prl>::const_iterator iter = this->grpprlPapx.begin(); iter != this->grpprlPapx.end(); iter++ )
			{
				this->sizeInBytesWithoutPadding += iter->Size();
			}

			this->sizeInBytes = this->sizeInBytesWithoutPadding;

			if ( this->sizeInBytes > 0 )
			{
				//The padding to be an even length
				if ( this->sizeInBytes % 2 != 0 )
				{
					this->sizeInBytes++;
				}

				this->bytes = new BYTE[this->sizeInBytes];

				if ( this->bytes != NULL )
				{
					memset( this->bytes, 0, this->sizeInBytes );

					DocFileFormat::FormatUtils::SetBytes( this->bytes, this->istd );

					BYTE* prlBytes = NULL;
					unsigned long prlSize = 0;
					unsigned int prlPrevSize = 0;

					for ( std::vector<Prl>::iterator iter = this->grpprlPapx.begin(); iter != this->grpprlPapx.end(); iter++ )
					{
						prlBytes = iter->GetBytes( &prlSize );

						if ( prlBytes != NULL )
						{
							memcpy( ( this->bytes + sizeof(this->istd) + prlPrevSize ), prlBytes, prlSize );
							prlPrevSize += prlSize;
						
							RELEASEARRAYOBJECTS (prlBytes);
						}
					}
				}
			}
		}

		UpxPapx( const UpxPapx& _upxPapx ) : istd(_upxPapx.istd), grpprlPapx(_upxPapx.grpprlPapx), bytes(NULL), sizeInBytes(_upxPapx.sizeInBytes), sizeInBytesWithoutPadding(_upxPapx.sizeInBytesWithoutPadding)
		{
			this->bytes = new BYTE[this->sizeInBytes];

			if ( this->bytes != NULL )
			{
				memset( this->bytes, 0, this->sizeInBytes );
				memcpy( this->bytes, _upxPapx.bytes, this->sizeInBytes );
			}
		}

		virtual ~UpxPapx()
		{
			RELEASEARRAYOBJECTS (bytes);
		}

		UpxPapx& operator = ( const UpxPapx& _upxPapx )
		{
			if ( this != &_upxPapx )
			{
				RELEASEARRAYOBJECTS (bytes);
				this->istd = _upxPapx.istd;
				this->grpprlPapx = _upxPapx.grpprlPapx;
				this->sizeInBytes = _upxPapx.sizeInBytes;
				this->sizeInBytesWithoutPadding = _upxPapx.sizeInBytesWithoutPadding;

				this->bytes = new BYTE[this->sizeInBytes];

				if ( this->bytes != NULL )
				{
					memcpy( this->bytes, _upxPapx.bytes, this->sizeInBytes );
				}  
			}

			return *this;
		}

		const std::vector<Prl> GetProperties() const
		{
			return this->grpprlPapx;
		}

		virtual operator BYTE*() const
		{
			return this->bytes;
		}

		virtual operator const BYTE*() const
		{
			return (const BYTE*)this->bytes;
		}

		virtual unsigned int Size() const
		{
			return this->sizeInBytes;
		}

		unsigned int SizeWithoutPadding() const
		{
			return this->sizeInBytesWithoutPadding;
		}

	private:

		unsigned short istd;
		std::vector<Prl> grpprlPapx;

		BYTE* bytes;
		unsigned int sizeInBytes;
		unsigned int sizeInBytesWithoutPadding;
	};

	class LPUpxPapx: public IOperand
	{
	private:
		UpxPapx upxPapx;

		BYTE* bytes;
		unsigned int sizeInBytes;
		unsigned int sizeInBytesWithoutPadding;

	private: 

		void Init()
		{
			this->sizeInBytes = ( sizeof(unsigned short) + this->upxPapx.Size() );
			this->sizeInBytesWithoutPadding = ( sizeof(unsigned short) + this->upxPapx.SizeWithoutPadding() );

			this->bytes = new BYTE[this->sizeInBytes];

			if ( this->bytes != NULL )
			{
				memset( this->bytes, 0, this->sizeInBytes );

				DocFileFormat::FormatUtils::SetBytes( this->bytes, (unsigned short)this->upxPapx.SizeWithoutPadding() );

				memcpy( ( this->bytes + sizeof(unsigned short) ), (BYTE*)this->upxPapx, this->upxPapx.Size() );
			}
		}
	public:

		LPUpxPapx() : upxPapx(), bytes(NULL), sizeInBytes(0), sizeInBytesWithoutPadding(0)
		{
			Init();
		}

		explicit LPUpxPapx( const UpxPapx& _upxPapx ) : upxPapx(_upxPapx), bytes(NULL), sizeInBytes(0), sizeInBytesWithoutPadding(0)
		{
			Init();
		}

		LPUpxPapx( const LPUpxPapx& _lPUpxPapx ) : upxPapx(_lPUpxPapx.upxPapx), bytes(NULL), sizeInBytes(_lPUpxPapx.sizeInBytes), sizeInBytesWithoutPadding(_lPUpxPapx.sizeInBytesWithoutPadding)
		{
			this->bytes = new BYTE[this->sizeInBytes];

			if ( this->bytes != NULL )
			{
				memset( this->bytes, 0 ,this->sizeInBytes );

				memcpy( this->bytes, _lPUpxPapx.bytes, this->sizeInBytes );
			}
		}

		bool operator == ( const LPUpxPapx& _lPUpxPapx )
		{
			return ( ( this->sizeInBytes == _lPUpxPapx.sizeInBytes ) && 
				( this->sizeInBytesWithoutPadding == _lPUpxPapx.sizeInBytesWithoutPadding ) &&
				( memcmp( this->bytes, _lPUpxPapx.bytes, this->sizeInBytes ) == 0 ) );
		}

		bool operator != ( const LPUpxPapx& _lPUpxPapx )
		{
			return !( this->operator == ( _lPUpxPapx ) );
		}

		LPUpxPapx& operator = ( const LPUpxPapx& _lPUpxPapx )
		{
			if ( this != &_lPUpxPapx )
			{
				RELEASEARRAYOBJECTS (bytes);

				this->upxPapx = _lPUpxPapx.upxPapx;
				this->sizeInBytes = _lPUpxPapx.sizeInBytes;
				this->sizeInBytesWithoutPadding = _lPUpxPapx.sizeInBytesWithoutPadding;

				this->bytes = new BYTE[this->sizeInBytes];

				if ( this->bytes != NULL )
				{
					memcpy( this->bytes, _lPUpxPapx.bytes, this->sizeInBytes );
				}  
			}

			return *this;
		}

		virtual ~LPUpxPapx()
		{
			RELEASEARRAYOBJECTS (bytes);
		}

		const std::vector<Prl> GetProperties() const
		{
			return this->upxPapx.GetProperties();
		}

		virtual operator BYTE*() const
		{
			return this->bytes;
		}

		virtual operator const BYTE*() const
		{
			return (const BYTE*)this->bytes;
		}

		virtual unsigned int Size() const
		{
			return this->sizeInBytes;
		}

		unsigned int SizeWithoutPadding() const
		{
			return this->sizeInBytesWithoutPadding;
		}
	};
}
