#pragma once

#include "LPStshiGrpPrl.h"

//!!!A structure that has no effect and MUST be ignored.!!!

namespace Docx2Doc
{
	class STSHIB: public IOperand
	{
	private:
		LPStshiGrpPrl grpprlChpStandard;
		LPStshiGrpPrl grpprlPapStandard;

		BYTE* bytes;
		unsigned int sizeInBytes;

	public:
		STSHIB():
		  grpprlChpStandard(), grpprlPapStandard(), bytes(NULL), sizeInBytes(0)
		  {
		  }

		  explicit STSHIB( const LPStshiGrpPrl& _grpprlChpStandard, const LPStshiGrpPrl& _grpprlPapStandard ):
		  grpprlChpStandard(_grpprlChpStandard), grpprlPapStandard(_grpprlPapStandard), bytes(NULL), sizeInBytes(0)
		  {
			  this->sizeInBytes = ( grpprlChpStandard.Size() + grpprlPapStandard.Size() );

			  this->bytes = new BYTE[this->sizeInBytes];

			  if ( this->bytes != NULL )
			  {
				  memset( this->bytes, 0, this->sizeInBytes );
				  memcpy( this->bytes, (BYTE*)(this->grpprlChpStandard), this->grpprlChpStandard.Size() );
				  memcpy( ( this->bytes + this->grpprlChpStandard.Size() ), (BYTE*)(this->grpprlPapStandard), this->grpprlPapStandard.Size() );
			  }
		  }

		  STSHIB( const STSHIB& _stshib ):
		  grpprlChpStandard(_stshib.grpprlChpStandard), grpprlPapStandard(_stshib.grpprlPapStandard), bytes(NULL), sizeInBytes(_stshib.sizeInBytes)
		  {
			  this->bytes = new BYTE[this->sizeInBytes];

			  if ( this->bytes != NULL )
			  {
				  memset( this->bytes, 0, this->sizeInBytes );
				  memcpy( this->bytes, _stshib.bytes, this->sizeInBytes );
			  }
		  }

		  virtual ~STSHIB()
		  {
			  RELEASEARRAYOBJECTS( bytes );
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
