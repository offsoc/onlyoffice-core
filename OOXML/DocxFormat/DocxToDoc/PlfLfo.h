#pragma once

#include "LFO.h"
#include "LFOData.h"

namespace Docx2Doc
{
	class PlfLfo: public IOperand
	{
	private:
		BYTE* bytes;
		unsigned int sizeInBytes;

	public:
		PlfLfo() : bytes(NULL), sizeInBytes(sizeof(unsigned int))
		{
			this->bytes = new BYTE[this->sizeInBytes];

			if ( this->bytes != NULL )
			{
				memset( this->bytes, 0, this->sizeInBytes );
			}
		}

		explicit PlfLfo( const std::vector<LFO>& _rgLfo, const std::vector<LFOData>& _rgLfoData ) : bytes(NULL), sizeInBytes(sizeof(unsigned int))
		{
			if ( _rgLfo.size() != _rgLfoData.size() )
			{
				this->bytes = new BYTE[this->sizeInBytes];

				if ( this->bytes != NULL )
				{
					memset( this->bytes, 0, this->sizeInBytes );
				}
			}
			else
			{
				for ( std::vector<LFO>::const_iterator iter = _rgLfo.begin(); iter != _rgLfo.end(); iter++ )
				{
					this->sizeInBytes += iter->Size();
				}

				for ( std::vector<LFOData>::const_iterator iter = _rgLfoData.begin(); iter != _rgLfoData.end(); iter++ )
				{
					this->sizeInBytes += iter->Size();
				}

				this->bytes = new BYTE[this->sizeInBytes];

				if ( this->bytes != NULL )
				{
					memset( this->bytes, 0, this->sizeInBytes );

					DocFileFormat::FormatUtils::SetBytes( this->bytes, (unsigned int)_rgLfo.size() );

					unsigned int offset = sizeof(unsigned int);

					if ( this->bytes != NULL )
					{
						for ( std::vector<LFO>::const_iterator iter = _rgLfo.begin(); iter != _rgLfo.end(); iter++ )
						{
							memcpy( ( this->bytes + offset ), (BYTE*)(*iter), iter->Size() );
							offset += iter->Size();
						}

						for ( std::vector<LFOData>::const_iterator iter = _rgLfoData.begin(); iter != _rgLfoData.end(); iter++ )
						{
							memcpy( ( this->bytes + offset ), (BYTE*)(*iter), iter->Size() );
							offset += iter->Size();
						}
					}
				}
			}
		}

		PlfLfo( const PlfLfo& _plfLfo ):
		bytes(NULL), sizeInBytes(_plfLfo.sizeInBytes)
		{
			this->bytes = new BYTE[this->sizeInBytes];

			if (bytes)
			{
				memset( this->bytes, 0, this->sizeInBytes );
				memcpy( this->bytes, _plfLfo.bytes, this->sizeInBytes );
			}
		}

		bool operator == ( const PlfLfo& _plfLfo )
		{
			return ( ( this->sizeInBytes == _plfLfo.sizeInBytes ) && 
				( memcmp( this->bytes, _plfLfo.bytes, this->sizeInBytes ) == 0 ) );
		}

		bool operator != ( const PlfLfo& _plfLfo )
		{
			return !( this->operator == ( _plfLfo ) );
		}

		PlfLfo& operator = ( const PlfLfo& _plfLfo )
		{
			if ( *this != _plfLfo )
			{
				RELEASEARRAYOBJECTS(bytes);

				this->sizeInBytes = _plfLfo.sizeInBytes;

				this->bytes = new BYTE[this->sizeInBytes];

				if ( this->bytes != NULL )
				{
					memcpy( this->bytes, _plfLfo.bytes, this->sizeInBytes );
				}  
			}

			return *this;
		}

		virtual ~PlfLfo()
		{
			RELEASEARRAYOBJECTS(bytes);
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
	};
}
