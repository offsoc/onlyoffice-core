#pragma once

#include "Prl.h"

#include "../../../MsBinaryFile/Common/Base/IOperand.h"

namespace Docx2Doc
{
	class PrcData: public IOperand
	{
	public:
		PrcData( const std::vector<Prl>& _grpPrl ) :  cbGrpprl(0), bytes(NULL), sizeInBytes(0)
		{
			short _cbGrpprl = 0;

			for ( std::vector<Prl>::const_iterator iter = _grpPrl.begin(); iter != _grpPrl.end(); iter++ )
			{
				_cbGrpprl += (short)iter->Size();

				if ( _cbGrpprl > 0x3FA2 )
				{
					break;
				}

				this->GrpPrl.push_back( *iter );

				this->cbGrpprl = _cbGrpprl;
			}

			this->sizeInBytes = (unsigned int)( sizeof(this->cbGrpprl) + this->cbGrpprl );

			if ( this->sizeInBytes > 0 )
			{
				this->bytes = new BYTE[this->sizeInBytes];

				if ( this->bytes != NULL )
				{
					memset( this->bytes, 0, this->sizeInBytes );

					DocFileFormat::FormatUtils::SetBytes( this->bytes, this->cbGrpprl );

					BYTE* prlBytes = NULL;
					unsigned long prlSize = 0;
					unsigned int prlPrevSize = 0; 

					for ( unsigned int i = 0; i < this->GrpPrl.size(); i++ )
					{
						prlBytes = this->GrpPrl[i].GetBytes( &prlSize );

						if ( prlBytes != NULL )
						{
							memcpy( ( this->bytes + sizeof(this->cbGrpprl) + prlPrevSize ), prlBytes, prlSize );
							prlPrevSize += prlSize;

							RELEASEARRAYOBJECTS (prlBytes);
						}
					}
				}
			}
		}

		PrcData( const PrcData& _prcData ) : cbGrpprl(_prcData.cbGrpprl), GrpPrl(_prcData.GrpPrl), bytes(NULL), sizeInBytes(_prcData.sizeInBytes)
		{
			if ( this->sizeInBytes != 0 )
			{
				this->bytes = new BYTE[this->sizeInBytes];

				if ( this->bytes != NULL )
				{
					memset( this->bytes, 0, this->sizeInBytes );

					memcpy( this->bytes, _prcData.bytes, this->sizeInBytes );
				}
			}
		}

		virtual ~PrcData()
		{
			RELEASEARRAYOBJECTS (bytes);
		}

		virtual operator BYTE*() const
		{
			return (BYTE*)this->bytes;
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
		short cbGrpprl;
		std::vector<Prl> GrpPrl;
		BYTE* bytes;
		unsigned int sizeInBytes;
	};

	class Prc: public IStreamWriter
	{
	public:
		Prc( const PrcData& _prcData ) : data(_prcData)
		{

		}

		Prc( const Prc& _prc ) :  data(_prc.data)
		{

		}

		virtual ~Prc()
		{
		}

		virtual BYTE* GetBytes( unsigned long* size ) const
		{
			BYTE* bytes = NULL;

			if ( size != NULL )
			{
				*size = sizeof(clxt) + this->data.Size();
				bytes = new BYTE[*size];

				if ( bytes != NULL )
				{
					memset( bytes, 0, *size );
					bytes[0] = clxt;

					memcpy( ( bytes + sizeof(clxt) ), (BYTE*)this->data, this->data.Size() );
				}
			}

			return bytes;
		}

		unsigned int Size() const
		{
			return (unsigned int)( sizeof(clxt) + this->data.Size() ); 
		}
	private:
		static const BYTE clxt = 0x01;
		PrcData data;
	};

	union FcCompressed
	{
		struct
		{
			unsigned int fc:30;
			unsigned int fCompressed:1;
			unsigned int r1:1;
		} FcCompressedStruct;
		unsigned int FcCompressedInt;

		FcCompressed( unsigned int _fcCompressedInt ):
		FcCompressedInt(_fcCompressedInt)
		{
		}

		static const unsigned int SIZE_IN_BYTES = sizeof(unsigned int);
	};

	union Prm
	{
		struct
		{
			unsigned int fComplex:1; //MUST be 0
			unsigned int isprm:7;
			unsigned int val:8;
		} Prm0Struct;
		struct
		{
			unsigned int fComplex:1; // MUST be 1
			unsigned int igrpprl:15;
		} Prm1Struct;
		short PrmShort;

		Prm( short _prmShort):
		PrmShort(_prmShort)
		{
		}

		static const unsigned int SIZE_IN_BYTES = sizeof(short);
	};

	struct Pcd
	{
		union
		{
			struct
			{
				unsigned int fNoParaLast:1;
				unsigned int fR1:1;
				unsigned int fDirty:1;
				unsigned int fR2:13;
			} NoParaStruct;
			short NoParaShort;
		} NoPara;
		FcCompressed fc;
		Prm prm;

		Pcd( bool _fNoParaLast, FcCompressed _fc, Prm _prm ):
		fc(_fc), prm(_prm)
		{
			this->NoPara.NoParaShort = 0;
			( _fNoParaLast ) ? ( this->NoPara.NoParaStruct.fNoParaLast = 1 ) : ( this->NoPara.NoParaStruct.fNoParaLast = 0 );
		}

		static const unsigned int SIZE_IN_BYTES = ( sizeof(short) + FcCompressed::SIZE_IN_BYTES + Prm::SIZE_IN_BYTES );
	};

	class PlcPcd : public IStreamWriter
	{
	public:
		PlcPcd( const std::vector<unsigned int>& _aCP, const std::vector<Pcd>& _aPcd ) : aCP(_aCP), aPcd(_aPcd), sizeInBytes(0)
		{
			this->sizeInBytes = ( ( this->aCP.size() * sizeof(unsigned int) ) + ( this->aPcd.size() * Pcd::SIZE_IN_BYTES ) );
		}

		virtual BYTE* GetBytes( unsigned long* size ) const
		{
			BYTE* bytes = NULL;

			if ( size != NULL )
			{
				*size = this->sizeInBytes;
				bytes = new BYTE[*size];

				if ( bytes != NULL )
				{
					memset( bytes, 0, *size );
					unsigned int i = 0;

					for ( std::vector<unsigned int>::const_iterator iter = this->aCP.begin(); iter != this->aCP.end(); iter++ )
					{
						DocFileFormat::FormatUtils::SetBytes( ( bytes + i ), (int)(*iter) );
						i += sizeof(unsigned int);
					}

					for ( std::vector<Pcd>::const_iterator iter = this->aPcd.begin(); iter != this->aPcd.end(); iter++ )
					{
						DocFileFormat::FormatUtils::SetBytes( ( bytes + i ), iter->NoPara.NoParaShort );
						i += sizeof(iter->NoPara.NoParaShort);

						DocFileFormat::FormatUtils::SetBytes( ( bytes + i ), (int)iter->fc.FcCompressedInt );
						i += sizeof(iter->fc.FcCompressedInt);

						DocFileFormat::FormatUtils::SetBytes( ( bytes + i ), iter->prm.PrmShort );
						i += sizeof(iter->prm.PrmShort);
					}
				}
			}

			return bytes;   
		}

		unsigned int Size() const
		{
			return this->sizeInBytes;
		}
	private:

		std::vector<unsigned int> aCP;
		std::vector<Pcd> aPcd;
		unsigned int sizeInBytes;
	};

	class Pcdt : public IStreamWriter
	{
	public:
		Pcdt( const PlcPcd& _plcPcd ) : lcb(0), plcPcd(_plcPcd)
		{
			this->lcb = this->plcPcd.Size();
		}

		Pcdt( const std::vector<unsigned int>& _aCP, const std::vector<Pcd>& _aPcd ): lcb(0), plcPcd( _aCP, _aPcd )
		{
			this->lcb = this->plcPcd.Size();
		}

		virtual BYTE* GetBytes( unsigned long* size ) const
		{
			BYTE* bytes = NULL;

			if ( size != NULL )
			{
				*size = ( sizeof(clxt) + sizeof(this->lcb) + this->plcPcd.Size() );
				bytes = new BYTE[*size];

				if ( bytes != NULL )
				{
					memset( bytes, 0, *size );

					bytes[0] = clxt;

					DocFileFormat::FormatUtils::SetBytes( ( bytes + sizeof(clxt) ), (int)this->lcb );

					BYTE* plcPcdBytes = NULL;
					unsigned long plcPcdSize = 0;

					plcPcdBytes = this->plcPcd.GetBytes( &plcPcdSize );

					if ( plcPcdBytes != NULL )
					{
						int size = sizeof(clxt) + sizeof(this->lcb);
						memcpy( ( bytes + sizeof(clxt) + sizeof(this->lcb) ), plcPcdBytes, plcPcdSize );
						RELEASEARRAYOBJECTS (plcPcdBytes);
					}
				}
			}

			return bytes;   
		}

		unsigned int Size() const
		{
			return ( sizeof(clxt) + sizeof(this->lcb) + this->plcPcd.Size() );
		}
	private:

		static const BYTE clxt = 0x02;
		unsigned int lcb;
		PlcPcd plcPcd;
	};

	class Clx : public IStreamWriter
	{
	public:

		Clx (const std::vector<Prc>* _rgPrc, const Pcdt& _pcdt ) : pcdt(_pcdt), sizeInBytes(0)
		{
			if ( _rgPrc != NULL )
			{
				this->RgPrc = *_rgPrc;
			}

			this->sizeInBytes = this->pcdt.Size();

			for ( std::vector<Prc>::iterator iter = this->RgPrc.begin(); iter != this->RgPrc.end(); iter++ )
			{
				this->sizeInBytes += iter->Size();
			}
		}

		virtual BYTE* GetBytes( unsigned long* size ) const
		{
			BYTE* bytes = NULL;

			if ( size != NULL )
			{
				*size = this->sizeInBytes;
				bytes = new BYTE[*size];

				if ( bytes != NULL )
				{
					memset( bytes, 0, *size );

					BYTE* prcBytes = NULL;
					unsigned long prcSize = 0;
					unsigned int prcPrevSize = 0; 

					for ( unsigned int i = 0; i < this->RgPrc.size(); i++ )
					{
						prcBytes = this->RgPrc[i].GetBytes( &prcSize );

						if ( prcBytes != NULL )
						{
							memcpy( ( bytes + prcPrevSize ), prcBytes, prcSize );
							prcPrevSize += prcSize;
							RELEASEARRAYOBJECTS (prcBytes);
						}
					}

					BYTE* pcdtBytes = NULL;
					unsigned long pcdtSize = 0;

					pcdtBytes = this->pcdt.GetBytes( &pcdtSize );

					if ( pcdtBytes != NULL )
					{
						memcpy( ( bytes + prcPrevSize ), pcdtBytes, pcdtSize );
						RELEASEARRAYOBJECTS (pcdtBytes);
					}
				}
			}

			return bytes;   
		}

		unsigned int Size() const
		{
			return this->sizeInBytes;
		}

	private:
		std::vector<Prc> RgPrc;
		Pcdt pcdt;
		unsigned int sizeInBytes;
	};
}
