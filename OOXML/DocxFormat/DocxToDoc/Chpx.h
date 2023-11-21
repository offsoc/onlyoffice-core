#pragma once

#include "Prl.h"

#include "../../../DesktopEditor/common/Types.h"

namespace Docx2Doc
{
	class Chpx : public IStreamWriter
	{

	public:
		Chpx() : cb(0)
		{
		}

		Chpx(const std::vector<Prl>& _grpprl) : cb(0)
		{
			for (unsigned int i = 0; i < _grpprl.size(); ++i)
			{
				BYTE grpprlSize = (BYTE)_grpprl[i].Size();
				if( this->cb + grpprlSize >= 256 )
					break;
				this->cb += grpprlSize;
				this->grpprl.push_back( _grpprl[i] );
			}
		}

		Chpx(const Chpx& chpx) : cb(0)
		{
			this->cb = chpx.cb;

			for ( unsigned int i = 0; i < chpx.grpprl.size(); i++ )
			{
				this->grpprl.push_back( chpx.grpprl[i] );
			}
		}

		virtual ~Chpx()
		{
		}

		bool operator == (const Chpx& chpx)
		{
			if ( ( this->cb != chpx.cb ) || ( this->grpprl.size() != chpx.grpprl.size() ) )
			{
				return false;
			}
			else
			{
				unsigned int minPrlCount = std::min( this->grpprl.size(), chpx.grpprl.size() );

				for ( unsigned int i = 0; i < minPrlCount; i++ )
				{
					if ( this->grpprl[i] != chpx.grpprl[i] )
					{
						return false;
					}
				}
			}

			return true;
		}

		bool operator != (const Chpx& chpx)
		{
			return !( *this == chpx );
		}

		Chpx& operator = (const Chpx& chpx)
		{
			if ( *this != chpx )
			{
				this->cb = chpx.cb;

				this->grpprl.clear();

				for ( unsigned int i = 0; i < chpx.grpprl.size(); i++ )
				{
					this->grpprl.push_back( chpx.grpprl[i] );
				}
			}

			return *this;
		}

		virtual BYTE* GetBytes(unsigned long* size) const
		{
			BYTE* bytes = NULL;

			if ( size != NULL )
			{
				*size = sizeof(this->cb) + this->cb;
				bytes = new BYTE[*size];

				if ( bytes != NULL )
				{
					memset( bytes, 0, *size );
					bytes[0] = this->cb;

					BYTE* prlBytes = NULL;
					unsigned long prlSize = 0;
					unsigned int prlPrevSize = 0; 

					for ( unsigned int i = 0; i < this->grpprl.size(); i++ )
					{
						prlBytes = this->grpprl[i].GetBytes( &prlSize );

						if ( prlBytes != NULL )
						{
							memcpy( ( bytes + sizeof(this->cb) + prlPrevSize ), prlBytes, prlSize );
							prlPrevSize += prlSize;

							RELEASEARRAYOBJECTS (prlBytes);
						}
					}
				}
			}

			return bytes;
		}

		unsigned long Size() const
		{
			return ( sizeof(this->cb) + this->cb );
		}

		inline size_t GetPrlSize ()
		{
			return grpprl.size();
		}

	private:

		BYTE cb; //Size in bytes of the grpprl
		std::vector<Prl> grpprl;
	};
}
