#pragma once

#include "OfficeArtBlip.h"
#include "OfficeArtMetafileHeader.h"

namespace OfficeArt
{
	class OfficeArtBlipEMF: public OfficeArtBlip
	{
	private:
		static const BYTE rgbUidsSize = 16;
		BYTE rgbUid1[rgbUidsSize];
		BYTE rgbUid2[rgbUidsSize];
		OfficeArtMetafileHeader metafileHeader;
		BYTE* BLIPFileData;
		unsigned int blipFileDataSize;

		BYTE* bytes;
		unsigned int size;

	public:
		OfficeArtBlipEMF():
		  OfficeArtBlip(OfficeArtRecordHeader( 0x0, 0x3D4, 0xF01A, ( sizeof(metafileHeader) + sizeof(rgbUid1) ) )),
			  metafileHeader(), BLIPFileData(NULL), blipFileDataSize(0), bytes(NULL), size(0)
		  {
			  memset( this->rgbUid1, 0, rgbUidsSize );

			  memset( this->rgbUid2, 0, rgbUidsSize );

			  this->Initialize();
		  }

		  explicit OfficeArtBlipEMF( const OfficeArtMetafileHeader& _metafileHeader,
			  BYTE* _blipFileData, const std::vector<BYTE>& _rgbUid1 = std::vector<BYTE>(),
			  const std::vector<BYTE>& _rgbUid2 = std::vector<BYTE>() ):
		  OfficeArtBlip(OfficeArtRecordHeader( 0x0, 0x3D4, 0xF01A, ( sizeof(metafileHeader) + sizeof(rgbUid1) ) )),
			  metafileHeader(_metafileHeader), BLIPFileData(NULL), blipFileDataSize(0), bytes(NULL), size(0)
		  {
			  memset( this->rgbUid1, 0, rgbUidsSize );

			  memset( this->rgbUid2, 0, rgbUidsSize );

			  this->blipFileDataSize = this->metafileHeader.GetCompressedSize();
			  this->size = this->blipFileDataSize;

			  if ( _rgbUid2.size() != rgbUidsSize )
			  {
				  this->size += ( sizeof(this->metafileHeader) + sizeof(this->rgbUid1) );

				  this->rh = OfficeArtRecordHeader( 0x0, 0x3D4, 0xF01A, this->size );
			  }
			  else
			  {
				  this->size += ( sizeof(this->metafileHeader) + sizeof(this->rgbUid1) + sizeof(this->rgbUid2) );

				  this->rh = OfficeArtRecordHeader( 0x0, 0x3D5, 0xF01A, this->size );
			  }

			  if ( _rgbUid1.size() == rgbUidsSize )
			  {
				  for ( unsigned int i = 0; i < _rgbUid1.size(); i++ )
				  {
					  this->rgbUid1[i] = _rgbUid1[i];
				  }
			  }

			  if ( _rgbUid2.size() == rgbUidsSize )
			  {
				  for ( unsigned int i = 0; i < _rgbUid2.size(); i++ )
				  {
					  this->rgbUid2[i] = _rgbUid2[i];
				  }
			  }

			  if ( ( _blipFileData != NULL ) && ( this->blipFileDataSize != 0 ) )
			  {
				  this->BLIPFileData = new BYTE[this->blipFileDataSize];

				  if ( this->BLIPFileData != NULL )
				  {
					  memset( this->BLIPFileData, 0, this->blipFileDataSize );
					  memcpy( this->BLIPFileData, _blipFileData, this->blipFileDataSize );
				  }
			  }

			  this->Initialize();
		  }

		  OfficeArtBlipEMF( const OfficeArtBlipEMF& _officeArtBlipEMF  ):
		  OfficeArtBlip(OfficeArtRecordHeader( _officeArtBlipEMF.rh )), metafileHeader(_officeArtBlipEMF.metafileHeader),
			  BLIPFileData(NULL), blipFileDataSize(_officeArtBlipEMF.blipFileDataSize), bytes(NULL), size(_officeArtBlipEMF.size)
		  {
			  memset( this->rgbUid1, 0, rgbUidsSize );
			  memcpy( this->rgbUid1, _officeArtBlipEMF.rgbUid1, rgbUidsSize );

			  memset( this->rgbUid2, 0, rgbUidsSize );
			  memcpy( this->rgbUid2, _officeArtBlipEMF.rgbUid2, rgbUidsSize );

			  if ( this->blipFileDataSize != 0 )
			  {
				  this->BLIPFileData = new BYTE[this->blipFileDataSize];

				  if ( this->BLIPFileData != NULL )
				  {
					  memset( this->BLIPFileData, 0, this->blipFileDataSize );

					  memcpy( this->BLIPFileData, _officeArtBlipEMF.BLIPFileData, this->blipFileDataSize );
				  }
			  }

			  if ( this->size != 0 )
			  {
				  this->bytes = new BYTE[this->size];

				  if ( this->bytes != NULL )
				  {
					  memset( this->bytes, 0, this->size );
					  memcpy( this->bytes, _officeArtBlipEMF.bytes, this->size );
				  }
			  }
		  }

		  virtual operator const BYTE* () const
		  {
			  return (const BYTE*)(this->bytes);
		  }

		  virtual operator BYTE* () const
		  {
			  return (BYTE*)(this->bytes);
		  }

		  virtual unsigned int Size() const
		  {
			  return this->size;
		  }

		  virtual IOfficeArtRecord* New() const
		  {
			  return new OfficeArtBlipEMF;
		  }

		  virtual IOfficeArtRecord* Clone() const
		  {
			  return new OfficeArtBlipEMF( *this );
		  }

		  virtual ~OfficeArtBlipEMF() 
		  {
			  RELEASEARRAYOBJECTS (BLIPFileData);
			  RELEASEARRAYOBJECTS (bytes);
		  }

	private:

		void Initialize()
		{
			this->size = ( sizeof(this->rh) + this->rh.GetLength() );

			if ( this->size != 0 )
			{
				this->bytes = new BYTE[this->size];

				if ( this->bytes != NULL )
				{
					memset( this->bytes, 0, this->size );

					unsigned int offset = 0;

					memcpy( ( this->bytes + offset ), (BYTE*)(this->rh), sizeof(this->rh) );
					offset += sizeof(this->rh);

					memcpy( ( this->bytes + offset ), (BYTE*)(this->rgbUid1), sizeof(this->rgbUid1) );
					offset += sizeof(this->rgbUid1);

					if ( this->rh.GetInstance() == 0x3D5 )
					{
						memcpy( ( this->bytes + offset ), (BYTE*)(this->rgbUid2), sizeof(this->rgbUid2) );
						offset += sizeof(this->rgbUid2);  
					}

					memcpy( ( this->bytes + offset ), &(this->metafileHeader), sizeof(this->metafileHeader) );
					offset += sizeof(this->metafileHeader);

					if ( ( this->BLIPFileData != NULL ) && ( this->blipFileDataSize != 0 ) )
					{
						memcpy( ( this->bytes + offset ), this->BLIPFileData, this->blipFileDataSize );
						offset += this->blipFileDataSize;
					}
				}
			}	      
		}
	};
}
