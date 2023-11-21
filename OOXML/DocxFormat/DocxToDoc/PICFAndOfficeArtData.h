#pragma once

#include "PICF.h"
#include "OfficeArt/OfficeArtInlineSpContainer.h"

namespace Docx2Doc
{
	class PICFAndOfficeArtData : public IOperand
	{
	public:

		PICFAndOfficeArtData () : bytes(NULL), sizeInBytes(0)
		{
		}

		explicit PICFAndOfficeArtData (const PICF& _picf, const OfficeArt::OfficeArtInlineSpContainer& _picture, const char* _stPicName = NULL ) : bytes(NULL), sizeInBytes(0)
		{
			this->sizeInBytes = _picf.Size();

			if ( ( _picf.GetPictureDataFormat() == Constants::MM_SHAPEFILE ) && ( _stPicName != NULL ) )
			{
				this->sizeInBytes += ( sizeof(BYTE) + ( sizeof(char) * strlen( _stPicName ) ) );
			}

			this->sizeInBytes += _picture.Size();

			if ( this->sizeInBytes != 0 )
			{
				this->bytes = new BYTE[this->sizeInBytes];

				if ( this->bytes != NULL )
				{
					memset( this->bytes, 0, this->sizeInBytes );

					unsigned int offset = 0;

					memcpy( ( this->bytes + offset ), (BYTE*)_picf, _picf.Size() );
					offset += _picf.Size();

					if ( ( _picf.GetPictureDataFormat() == Constants::MM_SHAPEFILE ) && ( _stPicName != NULL ) )
					{
						BYTE cchPicName = ( sizeof(char) * strlen( _stPicName ) );

						this->bytes[offset] = cchPicName;
						offset += sizeof(cchPicName);

						memcpy( ( this->bytes + offset ), _stPicName, cchPicName );
						offset += cchPicName;
					}

					memcpy( ( this->bytes + offset ), (BYTE*)_picture, _picture.Size() );
					offset += _picture.Size();
				}
			}
		}

		PICFAndOfficeArtData( const PICFAndOfficeArtData& _pICFAndOfficeArtData ) : bytes(NULL), sizeInBytes(_pICFAndOfficeArtData.sizeInBytes)
		{
			if ( this->sizeInBytes != 0 )
			{
				this->bytes = new BYTE[this->sizeInBytes];

				if ( this->bytes != NULL )
				{
					memset( this->bytes, 0, this->sizeInBytes );

					memcpy( this->bytes, _pICFAndOfficeArtData.bytes, this->sizeInBytes );
				}
			}
		}

		bool operator == ( const PICFAndOfficeArtData& _pICFAndOfficeArtData )
		{
			return ( ( this->sizeInBytes == _pICFAndOfficeArtData.sizeInBytes ) && 
				( memcmp( this->bytes, _pICFAndOfficeArtData.bytes, this->sizeInBytes ) == 0 ) );
		}

		bool operator != ( const PICFAndOfficeArtData& _pICFAndOfficeArtData )
		{
			return !( this->operator == ( _pICFAndOfficeArtData ) );
		}

		PICFAndOfficeArtData& operator = ( const PICFAndOfficeArtData& _pICFAndOfficeArtData )
		{
			if ( *this != _pICFAndOfficeArtData )
			{
				RELEASEARRAYOBJECTS (bytes);

				this->sizeInBytes = _pICFAndOfficeArtData.sizeInBytes;

				this->bytes = new BYTE[this->sizeInBytes];

				if ( this->bytes != NULL )
				{
					memcpy( this->bytes, _pICFAndOfficeArtData.bytes, this->sizeInBytes );
				}  
			}

			return *this;
		}

		virtual ~PICFAndOfficeArtData()
		{
			RELEASEARRAYOBJECTS (bytes);
		}

		virtual operator BYTE*() const
		{
			return (BYTE*)(this->bytes);
		}

		virtual operator const BYTE*() const
		{
			return (const BYTE*)this->bytes;
		}

		virtual unsigned int Size() const
		{
			return this->sizeInBytes;
		}

	private:

		BYTE* bytes;
		unsigned int sizeInBytes;
	};
}
