
#include "IOfficeArtAbstractContainer.h"
#include "OfficeArtRecordHeader.h"
#include "OfficeArtDgContainer.h"

namespace OfficeArt
{
	class OfficeArtDggContainer: public IOfficeArtAbstractContainer<IOfficeArtRecord>
	{
	protected:
		OfficeArtRecordHeader rh;
		std::list<OfficeArtRecordPtr> officeArtRecords;

		BYTE* bytes;
		unsigned int size;

	public:
		OfficeArtDggContainer() : rh( 0xF, 0x000, 0xF000, 0 ), bytes(NULL), size(sizeof(OfficeArtRecordHeader))
		{
			Initialize();
		}

		OfficeArtDggContainer( const OfficeArtDggContainer& _officeArtDgContainer ): rh( _officeArtDgContainer.rh ),
			officeArtRecords(_officeArtDgContainer.officeArtRecords), size(_officeArtDgContainer.size), bytes(NULL)
		{
			if ( this->size != 0 )
			{
				this->bytes = new BYTE[this->size];

				if ( this->bytes != NULL )
				{
					memset( this->bytes, 0, this->size );

					memcpy( this->bytes, _officeArtDgContainer.bytes, this->size );
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
			return new OfficeArtDgContainer;
		}

		virtual IOfficeArtRecord* Clone() const
		{
			return new OfficeArtDggContainer( *this );
		}

		virtual void PushBack( const IOfficeArtRecord& _officeArtRecord )
		{
			this->officeArtRecords.push_back( OfficeArtRecordPtr( _officeArtRecord.Clone() ) );

			this->Initialize();
		}

		virtual unsigned int Count() const
		{
			return (unsigned int)this->officeArtRecords.size();
		}

		virtual void Clear()
		{
			this->officeArtRecords.clear();

			this->Initialize();
		}

		virtual ~OfficeArtDggContainer() 
		{
			RELEASEARRAYOBJECTS (bytes);
		}

	private:

		void Initialize()
		{
			size = 0;

			for (std::list<OfficeArtRecordPtr>::const_iterator iter = officeArtRecords.begin(); iter != officeArtRecords.end(); ++iter)
			{
				size += (*iter)->Size();
			}

			rh		=	OfficeArtRecordHeader( 0xF, 0x000, 0xF000, size);
			size	+=	sizeof(rh);

			RELEASEARRAYOBJECTS(bytes);

			if (0 != size)
			{
				bytes = new BYTE[size];
				if (bytes)
				{
					memset(bytes, 0, size);

					unsigned int offset = 0;

					memcpy((bytes + offset), (BYTE*)(rh), sizeof(rh));
					offset += sizeof(rh);

					for (std::list<OfficeArtRecordPtr>::const_iterator iter = officeArtRecords.begin(); iter != officeArtRecords.end(); ++iter)
					{
						const IOfficeArtRecord* officeArtRecord =  iter->operator->();
						if (officeArtRecord)
						{
							memcpy( ( this->bytes + offset ), (BYTE*)(*officeArtRecord), officeArtRecord->Size() );
							offset += officeArtRecord->Size();
						}
					}
				}
			}
		}
	};
}
