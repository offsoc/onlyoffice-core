#pragma once

#include "IOfficeArtRecord.h"
#include "IOfficeArtAbstractContainer.h"

namespace OfficeArt
{
	class OfficeArtDgContainer: public IOfficeArtAbstractContainer<IOfficeArtRecord>
	{
	public:
		OfficeArtDgContainer () : rh( 0xF, 0x000, 0xF002, 0 ), bytes(NULL), size(sizeof(OfficeArtRecordHeader))
		{
			Initialize();
		}

		OfficeArtDgContainer (const OfficeArtDgContainer& _officeArtDgContainer): rh(_officeArtDgContainer.rh),officeArtRecords(_officeArtDgContainer.officeArtRecords), size(_officeArtDgContainer.size), bytes(NULL)
		{
			if (0 != size)
			{
				bytes = new BYTE[size];
				if (bytes)
				{
					memset(bytes, 0, size);
					memcpy(bytes, _officeArtDgContainer.bytes, size);
				}
			}
		}

		virtual operator const BYTE* () const
		{
			return (const BYTE*)(bytes);
		}

		virtual operator BYTE* () const
		{
			return (BYTE*)(bytes);
		}

		virtual unsigned int Size() const
		{
			return size;
		}

		virtual IOfficeArtRecord* New() const
		{
			return new OfficeArtDgContainer;
		}

		virtual IOfficeArtRecord* Clone() const
		{
			return new OfficeArtDgContainer(*this);
		}

		virtual void PushBack(const IOfficeArtRecord& _officeArtRecord)
		{
			officeArtRecords.push_back(OfficeArtRecordPtr(_officeArtRecord.Clone()));
			Initialize();
		}

		virtual unsigned int Count() const
		{
			return (unsigned int)officeArtRecords.size();
		}

		virtual void Clear()
		{
			officeArtRecords.clear();
			Initialize();
		}

		virtual ~OfficeArtDgContainer() 
		{
			RELEASEARRAYOBJECTS (bytes);
		}

	private:

		inline void Initialize()
		{
			size = 0;

			for (std::list<OfficeArtRecordPtr>::const_iterator iter = officeArtRecords.begin(); iter != officeArtRecords.end(); ++iter)
			{
				size += (*iter)->Size();
			}

			rh = OfficeArtRecordHeader( 0xF, 0x000, 0xF002, size);

			size += sizeof(rh);

			RELEASEARRAYOBJECTS (bytes);

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
						const IOfficeArtRecord* officeArtRecord = iter->operator->();
						if (officeArtRecord)
						{
							memcpy((bytes + offset), (BYTE*)(*officeArtRecord), officeArtRecord->Size());
							offset += officeArtRecord->Size();
						}
					}
				}
			}
		}

	protected:

		OfficeArtRecordHeader		rh;
		std::list<OfficeArtRecordPtr>	officeArtRecords;

		BYTE*						bytes;
		unsigned int				size;
	};
}
