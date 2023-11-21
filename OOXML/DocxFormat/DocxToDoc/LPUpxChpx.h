#pragma once

#include "Prl.h"

namespace Docx2Doc
{
	class UpxChpx: public IOperand
	{
	public:
		UpxChpx() : bytes(NULL), sizeInBytes(0), sizeInBytesWithoutPadding(0)
		{
		}

		UpxChpx( const std::vector<Prl>& _grpprlChpx ) : grpprlChpx(_grpprlChpx), bytes(NULL), sizeInBytes(0), sizeInBytesWithoutPadding(0)
		{
			for ( std::vector<Prl>::const_iterator iter = this->grpprlChpx.begin(); iter != this->grpprlChpx.end(); iter++ )
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

					BYTE* prlBytes = NULL;
					unsigned long prlSize = 0;
					unsigned int prlPrevSize = 0;

					for ( std::vector<Prl>::iterator iter = this->grpprlChpx.begin(); iter != this->grpprlChpx.end(); iter++ )
					{
						prlBytes = iter->GetBytes( &prlSize );

						if ( prlBytes != NULL )
						{
							memcpy( ( this->bytes + prlPrevSize ), prlBytes, prlSize );
							prlPrevSize += prlSize;

							RELEASEARRAYOBJECTS (prlBytes);
						}
					}
				}
			}
		}

		UpxChpx( const UpxChpx& _upxChpx ) : grpprlChpx(_upxChpx.grpprlChpx), bytes(NULL), sizeInBytes(_upxChpx.sizeInBytes),sizeInBytesWithoutPadding(_upxChpx.sizeInBytesWithoutPadding)
		{
			this->bytes = new BYTE[this->sizeInBytes];

			if ( this->bytes != NULL )
			{
				memset( this->bytes, 0, this->sizeInBytes );
				memcpy( this->bytes, _upxChpx.bytes, this->sizeInBytes );
			}
		}

		virtual ~UpxChpx()
		{
			RELEASEARRAYOBJECTS (bytes);
		}

		UpxChpx& operator = ( const UpxChpx& _upxChpx )
		{
			if ( this != &_upxChpx )
			{
				RELEASEARRAYOBJECTS (bytes);

				this->grpprlChpx = _upxChpx.grpprlChpx;
				this->sizeInBytes = _upxChpx.sizeInBytes;
				this->sizeInBytesWithoutPadding = _upxChpx.sizeInBytesWithoutPadding;

				this->bytes = new BYTE[this->sizeInBytes];

				if ( this->bytes != NULL )
				{
					memcpy( this->bytes, _upxChpx.bytes, this->sizeInBytes );
				}  
			}

			return *this;
		}

		const std::vector<Prl> GetProperties() const
		{
			return this->grpprlChpx;
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
		std::vector<Prl> grpprlChpx;

		BYTE* bytes;
		unsigned int sizeInBytes;
		unsigned int sizeInBytesWithoutPadding;
	};

	class LPUpxChpx : public IOperand
	{
	private: 

		void Init()
		{
			this->sizeInBytes = ( sizeof(unsigned short) + this->upxChpx.Size() );
			this->sizeInBytesWithoutPadding = ( sizeof(unsigned short) + this->upxChpx.SizeWithoutPadding() );

			this->bytes = new BYTE[this->sizeInBytes];

			if ( this->bytes != NULL )
			{
				memset( this->bytes, 0, this->sizeInBytes );

				DocFileFormat::FormatUtils::SetBytes( this->bytes, (unsigned short)this->upxChpx.SizeWithoutPadding() );

				memcpy( ( this->bytes + sizeof(unsigned short) ), (BYTE*)this->upxChpx, this->upxChpx.Size() );
			}
		}
	public:

		LPUpxChpx():  upxChpx(), bytes(NULL), sizeInBytes(0), sizeInBytesWithoutPadding(0)
		{
			Init();
		}

		LPUpxChpx( const UpxChpx& _upxChpx ): upxChpx(_upxChpx), bytes(NULL), sizeInBytes(0), sizeInBytesWithoutPadding(0)
		{
			Init();
		}

		LPUpxChpx( const LPUpxChpx& _lPUpxChpx ) : upxChpx(_lPUpxChpx.upxChpx), bytes(NULL), sizeInBytes(_lPUpxChpx.sizeInBytes), sizeInBytesWithoutPadding(_lPUpxChpx.sizeInBytesWithoutPadding)
		{
			this->bytes = new BYTE[this->sizeInBytes];

			if ( this->bytes != NULL )
			{
				memset( this->bytes, 0 ,this->sizeInBytes );

				memcpy( this->bytes, _lPUpxChpx.bytes, this->sizeInBytes );
			}
		}

		bool operator == ( const LPUpxChpx& _lPUpxChpx )
		{
			return ( ( this->sizeInBytes == _lPUpxChpx.sizeInBytes ) &&
				( this->sizeInBytesWithoutPadding == _lPUpxChpx.sizeInBytesWithoutPadding ) &&
				( memcmp( this->bytes, _lPUpxChpx.bytes, this->sizeInBytes ) == 0 ) );
		}

		bool operator != ( const LPUpxChpx& _lPUpxChpx )
		{
			return !( this->operator == ( _lPUpxChpx ) );
		}

		LPUpxChpx& operator = ( const LPUpxChpx& _lPUpxChpx )
		{
			if ( this != &_lPUpxChpx )
			{
				RELEASEARRAYOBJECTS (bytes);

				this->upxChpx = _lPUpxChpx.upxChpx;
				this->sizeInBytes = _lPUpxChpx.sizeInBytes;
				this->sizeInBytesWithoutPadding = _lPUpxChpx.sizeInBytesWithoutPadding;

				this->bytes = new BYTE[this->sizeInBytes];

				if ( this->bytes != NULL )
				{
					memcpy( this->bytes, _lPUpxChpx.bytes, this->sizeInBytes );
				}  
			}

			return *this;
		}

		virtual ~LPUpxChpx()
		{
			RELEASEARRAYOBJECTS (bytes);
		}

		const std::vector<Prl> GetProperties() const
		{
			return this->upxChpx.GetProperties();
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

		UpxChpx			upxChpx;

		BYTE*			bytes;
		unsigned int	sizeInBytes;
		unsigned int	sizeInBytesWithoutPadding;
	};
}
