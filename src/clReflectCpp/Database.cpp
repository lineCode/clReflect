
#include <clcpp\Database.h>
#include "DatabaseLoader.h"


namespace
{
	int CompareNames(clcpp::Name name, unsigned int hash)
	{
		return hash - name.hash;
	}


	int ComparePrimitives(const clcpp::Primitive* primitive, unsigned int hash)
	{
		return hash - primitive->name.hash;
	}


	int ComparePrimitives(const clcpp::Primitive& primitive, unsigned int hash)
	{
		return hash - primitive.name.hash;
	}


	template <typename ARRAY_TYPE, typename COMPARE_L_TYPE, typename COMPARE_R_TYPE, int (COMPARE_FUNC)(COMPARE_L_TYPE, COMPARE_R_TYPE)>
	int BinarySearch(const clcpp::CArray<ARRAY_TYPE>& entries, COMPARE_R_TYPE compare_value)
	{
		// TODO: Return multiple entries

		int first = 0;
		int last = entries.size() - 1;

		// Binary search
		while (first <= last)
		{
			// Identify the mid point
			int mid = (first + last) / 2;

			int cmp = COMPARE_FUNC(entries[mid], compare_value);
			if (cmp > 0)
			{
				// Shift search to local upper half
				first = mid + 1;
			}
			else if (cmp < 0)
			{
				// Shift search to local lower half
				last = mid - 1;
			}
			else
			{
				// Exact match found
				return mid;
			}
		}

		return -1;
	}
}


const clcpp::Primitive* clcpp::internal::FindPrimitive(const CArray<const Primitive*>& primitives, unsigned int hash)
{
	int index = BinarySearch<const Primitive*, const Primitive*, unsigned int, ComparePrimitives>(primitives, hash);
	if (index == -1)
	{
		return 0;
	}
	return primitives[index];
}


clcpp::Database::Database()
	: m_DatabaseMem(0)
{
}


clcpp::Database::~Database()
{
	delete (char*)m_DatabaseMem;
}


clcpp::Name clcpp::Database::GetName(const char* text) const
{
	// Null pointer
	if (text == 0)
	{
		return clcpp::Name();
	}

	// Hash and exit on no value
	unsigned int hash = internal::HashNameString(text);
	if (hash == 0)
	{
		return clcpp::Name();
	}

	// Lookup the name by hash and see
	int index = BinarySearch<Name, Name, unsigned int, CompareNames>(m_DatabaseMem->names, hash);
	if (index == -1)
	{
		return clcpp::Name();
	}

	return m_DatabaseMem->names[index];
}


const clcpp::Type* clcpp::Database::GetType(unsigned int hash) const
{
	return FindPrimitive(m_DatabaseMem->type_primitives, hash);
}


const clcpp::Namespace* clcpp::Database::GetNamespace(unsigned int hash) const
{
	int index = BinarySearch<Namespace, const Primitive&, unsigned int, ComparePrimitives>(m_DatabaseMem->namespaces, hash);
	if (index == -1)
	{
		return 0;
	}
	return &m_DatabaseMem->namespaces[index];
}


const clcpp::Function* clcpp::Database::GetFunction(unsigned int hash) const
{
	int index = BinarySearch<Function, const Primitive&, unsigned int, ComparePrimitives>(m_DatabaseMem->functions, hash);
	if (index == -1)
	{
		return 0;
	}
	return &m_DatabaseMem->functions[index];
}


bool clcpp::Database::Load(IFile* file)
{
	m_DatabaseMem = internal::LoadMemoryMappedDatabase(file);
	return m_DatabaseMem != 0;
}