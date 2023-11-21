#pragma once

#include "PapxInFkp.h"
#include "BxPap.h"
#include "Clx.h"
#include "BinaryStorage.h"

#include "../../../DesktopEditor/common/Types.h"

namespace Docx2Doc
{
	class PapxFkp: public IStreamWriter
	{
	public:
		static const unsigned short PAPX_FKP_SIZE = 512;
		static const BYTE PAPX_FKP_MAX_CPARA = 0x1D;

		PapxFkp (const std::vector<unsigned int>& _rgfc, const std::vector<PapxInFkp>& _papxInFkps) : cpara(0)
		{
			this->cpara = _papxInFkps.size();
			this->rgfc = _rgfc;

			unsigned long papxSizeInBytes = 0;

			this->papxInFkps.push_back( _papxInFkps[0] );
			papxSizeInBytes = this->papxInFkps[0].Size();
			this->rgbx.push_back( BxPap( (BYTE)( ( PAPX_FKP_SIZE - 1 - papxSizeInBytes ) / 2 ) ) );

			for ( int i = 1; i < this->cpara; i++ )
			{
				this->papxInFkps.push_back( _papxInFkps[i] );
				papxSizeInBytes = this->papxInFkps[i].Size();
				this->rgbx.push_back( BxPap( (BYTE)( this->rgbx[i-1].GetOffset() - 1 - ( papxSizeInBytes / 2 ) ) ) );
			}
		}

		virtual ~PapxFkp()
		{
		}

		virtual BYTE* GetBytes( unsigned long* size ) const
		{
			BYTE* bytes = NULL;

			if ( size != NULL )
			{
				*size = 512;
				bytes = new BYTE[*size];

				if ( bytes != NULL )
				{
					memset( bytes, 0, *size );

					int i = 0;

					for ( ; i < ( this->cpara + 1); i++ )
					{
						FormatUtils::SetBytes( ( bytes + ( i * sizeof(this->rgfc[i]) ) ), (int)this->rgfc[i] );
					}

					i = ( this->cpara + 1) * sizeof(this->rgfc[i]);

					BYTE *papxsbytes = NULL;
					unsigned long papxssize = 0;

					for ( int j = 0; j < this->cpara; j++, i += ( BxPap::RESERVED_SIZE + 1 ) )
					{
						bytes[i] = this->rgbx[j].GetOffset();
						BYTE *papxsbytes = this->papxInFkps[j].GetBytes( &papxssize );

						if ( papxsbytes != NULL )
						{
							memcpy( ( bytes + ( bytes[i] * 2 ) ), papxsbytes, papxssize );
							RELEASEARRAYOBJECTS (papxsbytes);
						}
					}

					bytes[511] = this->cpara;
				}
			}

			return bytes;
		}

		unsigned int GetEndOffset() const
		{
			return ( *(this->rgfc.end() - 1 ) );
		}

		static std::vector<PapxFkp> GetAllPapxFkps( const std::vector<unsigned int>& _rgfc, const std::vector<PapxInFkp>& _papxInFkps )
		{
			std::vector<PapxFkp> allPapxFkps;
			std::vector<unsigned int> rgfc;
			std::vector<PapxInFkp> papxInFkps;

			rgfc.push_back( _rgfc[0] );
			unsigned int allPapxInFkpsSize = 0;
			unsigned int rgfcCount = 2;

			for ( unsigned int i = 0; i < _papxInFkps.size(); i++ )
			{
				unsigned int papxInFkpSize = _papxInFkps[i].Size();
				allPapxInFkpsSize += papxInFkpSize;

				//Check if all rgfcs and rgbxs + PapxInFkps less then 512 bytes
				if ( ( ( rgfcCount * sizeof(unsigned int) ) + ( ( rgfcCount - 1 ) * ( BxPap::RESERVED_SIZE + 1 ) ) ) >= ( PAPX_FKP_SIZE / 2 ) ||
					( allPapxInFkpsSize ) >= ( PAPX_FKP_SIZE / 2 ) )
				{
					PapxFkp papxFkp( rgfc, papxInFkps );
					allPapxFkps.push_back( papxFkp );
					rgfc.clear();
					papxInFkps.clear();
					rgfc.push_back( _rgfc[i] );
					allPapxInFkpsSize = papxInFkpSize;
					rgfcCount = 2;
				}

				rgfc.push_back( _rgfc[i+1] );

				if ( ( ( rgfcCount * sizeof(unsigned int) ) + ( ( rgfcCount - 1 ) * ( BxPap::RESERVED_SIZE + 1 ) ) ) >= ( PAPX_FKP_SIZE / 2 ) ||
					( papxInFkpSize ) >= ( PAPX_FKP_SIZE / 2 ) )
				{
					PrcData prcData( _papxInFkps[i].GetPrls() );

					BinaryStorageSingleton* binaryStorage = BinaryStorageSingleton::Instance();

					if ( binaryStorage != NULL )
					{
						unsigned int prcDataOffset = binaryStorage->PushData( (const BYTE*)prcData, prcData.Size() );

						std::vector<Prl> prls;
						prls.push_back( Prl( (short)0x6646, (BYTE*)(&prcDataOffset) ) );
						PapxInFkp papxInFkp( GrpPrlAndIstd( 0, prls ) );

						papxInFkps.push_back( papxInFkp );
					}
				}
				else
				{
					papxInFkps.push_back( _papxInFkps[i] );
				}

				rgfcCount++;
			}

			if ( !rgfc.empty() && !papxInFkps.empty() )
			{
				PapxFkp papxFkp( rgfc, papxInFkps );
				allPapxFkps.push_back( papxFkp );
				rgfc.clear();
				papxInFkps.clear();
			}

			return allPapxFkps;
		}

	private:

		std::vector<unsigned int> rgfc;
		std::vector<BxPap> rgbx;
		std::vector<PapxInFkp> papxInFkps;
		BYTE cpara;
	};
}
