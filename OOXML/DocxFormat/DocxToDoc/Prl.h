#pragma once

#include "IStreamWriter.h"
#include "../../../DesktopEditor/common/Types.h"
#include "../../../MsBinaryFile/Common/Base/SPRMCodes.h"
#include "../../../MsBinaryFile/Common/Base/FormatUtils.h"
#include "../../../MsBinaryFile/Common/Base/IOperand.h"

namespace Docx2Doc
{
	class Prl: public IStreamWriter
	{
	private:
		union
		{
			struct
			{
				unsigned int ispmd:9;
				unsigned int fSpec:1;
				unsigned int sgc:3;
				unsigned int spra:3;
			} sprmstruct;
			unsigned short sprmshort;
		} sprm;

		DocFileFormat::OperationCode OpCode;
		BYTE* operand;
		unsigned short operandSize;

		/// Get be used to get the size of the sprm's operand.
		/// Returns 0 if the Operation failed and 255 if the size is variable
		BYTE GetOperandSize( BYTE spra )
		{
			switch ( spra )
			{
			case 0: return 1;
			case 1: return 1;
			case 2: return 2;
			case 3: return 4;
			case 4: return 2;
			case 5: return 2;
			case 6: return 255;
			case 7: return 3;
			default: return 0;
			}
		}

	public:

		Prl ()		  
		{
			operand			=	NULL;
			operandSize		=	0;

			sprm.sprmshort	=	0;
			OpCode			=	(DocFileFormat::OperationCode)0;
		}

		Prl (short inSprm, BYTE* operandBytes)
		{
			operand			=	NULL;
			operandSize		=	0;

			sprm.sprmshort	=	inSprm;
			OpCode			=	(DocFileFormat::OperationCode)(sprm.sprmshort);
			BYTE opSize		=	GetOperandSize(sprm.sprmstruct.spra);

			if ( opSize == 255 )
			{
				switch (sprm.sprmshort)
				{
				case 0xD608: //sprmTDefTable
				case 0xD606: //sprmTDefTable10
					{
						//!!!TODO!!!
						this->operandSize = DocFileFormat::FormatUtils::BytesToUInt16( operandBytes, 0, 2 ) + 1;
						this->operand = new BYTE[this->operandSize];
						memset( this->operand, 0, this->operandSize );

						if ( operandBytes != NULL )
						{
							memcpy( this->operand, operandBytes, this->operandSize );
						}
					}
					break;

				case 0xC615: //sprmPChgTabs 
					{
						//!!!TODO!!!
					}
					break;

				default:
					{
						//!!!TODO!!!
						this->operandSize = ( operandBytes[0] + 1 );
						this->operand = new BYTE[this->operandSize];
						memset( this->operand, 0, this->operandSize );

						if ( operandBytes != NULL )
						{
							memcpy( this->operand,  operandBytes, this->operandSize );
						}
					}

					break;
				}
			}
			else if ( opSize != 0 )
			{
				operandSize =	opSize;
				operand		=	new BYTE[operandSize];
				
				if (NULL != operand)
					memset(operand, 0, operandSize);

				if (NULL != operandBytes)
					memcpy(operand,  operandBytes, operandSize);
			}
		}

		Prl (const Prl& oPrl)
		{
			sprm.sprmshort	=	oPrl.sprm.sprmshort;
			OpCode			=	oPrl.OpCode;
			operandSize		=	oPrl.operandSize;

			operand = new BYTE[operandSize];
			if (operand)
			{
				memcpy(operand, oPrl.operand, operandSize);
			}
		}

		bool operator == (const Prl& oPrl)
		{
			unsigned short minOperandSize = std::min(operandSize, oPrl.operandSize);
			return ((sprm.sprmshort == oPrl.sprm.sprmshort) && (0 == memcmp(operand, oPrl.operand, minOperandSize)));
		}

		bool operator != (const Prl& oPrl)
		{
			return !( this->operator == (oPrl) );
		}

		Prl& operator = (const Prl& oPrl)
		{
			if (*this != oPrl)
			{
				sprm.sprmshort	=	oPrl.sprm.sprmshort;
				OpCode			=	oPrl.OpCode;
				operandSize		=	oPrl.operandSize;

				RELEASEARRAYOBJECTS (operand);
				
				operand			=	new BYTE[operandSize];
				if(operand)
				{
					memcpy(operand, oPrl.operand, operandSize);
				}
			}

			return *this;
		}

		virtual ~Prl()
		{
			RELEASEARRAYOBJECTS (operand);
		}

		virtual BYTE* GetBytes (unsigned long* size) const
		{
			BYTE* bytes = NULL;

			if ( size != NULL )
			{
				*size = ( sizeof(sprm.sprmshort) + operandSize );
				bytes = new BYTE[*size];

				if ( bytes != NULL )
				{
					memset( bytes, 0, *size );
					DocFileFormat::FormatUtils::SetBytes( bytes, sprm.sprmshort );
					memcpy( ( bytes + 2 ), operand, operandSize );
				}
			}

			return bytes;
		}

		unsigned long Size() const
		{
			return ( sizeof(sprm.sprmshort) + operandSize );
		}

		unsigned short GetSprmCode() const
		{
			return sprm.sprmshort;
		}

		//
		inline unsigned short GetOpTwo ()	const							//	return two bytes value
		{
			return (unsigned short)(operand[0] | operand[1] << 8);
		}
	};
}
