//
// Column.h
//
// $Id: //poco/Main/Data/include/Poco/Data/Column.h#5 $
//
// Library: Data
// Package: DataCore
// Module:  Column
//
// Definition of the Column class.
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef Data_Column_INCLUDED
#define Data_Column_INCLUDED


#include "Poco/Data/Data.h"
#include "Poco/Data/MetaColumn.h"
#include "Poco/SharedPtr.h"
#include <vector>
#include <list>
#include <deque>


namespace Poco {
namespace Data {


template <class T, class C = std::vector<T> >
class Column
	/// Column class is column data container.
	/// Data (a pointer to container) is assigned to the class 
	/// through either constructor or set() member function.
	/// Construction with null pointer is not allowed.
	/// This class owns the data assigned to it and deletes the storage on destruction.
{
public:
	typedef C Container;
	typedef typename Container::const_iterator Iterator;
	typedef typename Container::const_reverse_iterator RIterator;
	typedef typename Container::size_type Size;

	Column(const MetaColumn& metaColumn, C* pData): _metaColumn(metaColumn), _pData(pData)
		/// Creates the Column.
	{
		poco_check_ptr (_pData);
	}

	Column(const Column& col): _metaColumn(col._metaColumn), _pData(col._pData)
		/// Creates the Column.
	{
	}

	~Column()
		/// Destroys the Column.
	{
	}

	Column& operator = (const Column& col)
		/// Assignment operator.
	{
		Column tmp(col);
		swap(tmp);
		return *this;
	}

	void swap(Column& other)
		/// Swaps the column with another one.
	{
		std::swap(_metaColumn, other._metaColumn);
		std::swap(_pData, other._pData);
	}

	Container& data()
		/// Returns reference to contained data.
	{
		return *_pData;
	}

	const T& value(std::size_t row) const
		/// Returns the field value in specified row.
	{
		try
		{
			return _pData->at(row);
		}
		catch (std::out_of_range& ex)
		{ 
			throw RangeException(ex.what()); 
		}
	}

	const T& operator [] (std::size_t row) const
		/// Returns the field value in specified row.
	{
		return value(row);
	}

	Size rowCount() const
		/// Returns number of rows.
	{
		return _pData->size();
	}

	void reset()
		/// Clears and shrinks the storage.
	{
		C().swap(*_pData);
	}

	const std::string& name() const
		/// Returns column name.
	{
		return _metaColumn.name();
	}

	std::size_t length() const
		/// Returns column maximum length.
	{
		return _metaColumn.length();
	}

	std::size_t precision() const
		/// Returns column precision.
		/// Valid for floating point fields only (zero for other data types).
	{
		return _metaColumn.precision();
	}

	std::size_t position() const
		/// Returns column position.
	{
		return _metaColumn.position();
	}

	MetaColumn::ColumnDataType type() const
		/// Returns column type.
	{
		return _metaColumn.type();
	}

	Iterator begin() const
		/// Returns iterator pointing to the beginning of data storage vector.
	{
		return _pData->begin();
	}

	Iterator end() const
		/// Returns iterator pointing to the end of data storage vector.
	{
		return _pData->end();
	}

private:
	Column();

	MetaColumn _metaColumn;
	Poco::SharedPtr<Container> _pData;
};


template <>
class Column<bool, std::vector<bool> >
	/// The std::vector<bool> specialization for the Column class.
	/// 
	/// This specialization is necessary due to the nature of std::vector<bool>.
	/// For details, see the standard library implementation of vector<bool> 
	/// or
	/// S. Meyers: "Effective STL" (Copyright Addison-Wesley 2001),
	/// Item 18: "Avoid using vector<bool>."
	/// 
	/// The workaround employed here is using deque<bool> as an
	/// internal "companion" container kept in sync with the vector<bool>
	/// column data.
{
public:
	typedef std::vector<bool> Container;
	typedef Container::const_iterator Iterator;
	typedef Container::const_reverse_iterator RIterator;
	typedef Container::size_type Size;

	Column(const MetaColumn& metaColumn, Container* pData): 
		_metaColumn(metaColumn), 
		_pData(pData)
		/// Creates the Column.
	{
		poco_check_ptr (_pData);
		_deque.assign(_pData->begin(), _pData->end());
	}

	Column(const Column& col): 
		_metaColumn(col._metaColumn), 
		_pData(col._pData)
		/// Creates the Column.
	{
		_deque.assign(_pData->begin(), _pData->end());
	}

	~Column()
		/// Destroys the Column.
	{
	}

	Column& operator = (const Column& col)
		/// Assignment operator.
	{
		Column tmp(col);
		swap(tmp);
		return *this;
	}

	void swap(Column& other)
		/// Swaps the column with another one.
	{
		std::swap(_metaColumn, other._metaColumn);
		std::swap(_pData, other._pData);
		std::swap(_deque, other._deque);
	}

	Container& data()
		/// Returns reference to contained data.
	{
		return *_pData;
	}

	const bool& value(std::size_t row) const
		/// Returns the field value in specified row.
	{
		if (_deque.size() < _pData->size())
			_deque.resize(_pData->size());

		try
		{
			return _deque.at(row) = _pData->at(row);
		}
		catch (std::out_of_range& ex)
		{ 
			throw RangeException(ex.what()); 
		}
	}

	const bool& operator [] (std::size_t row) const
		/// Returns the field value in specified row.
	{
		return value(row);
	}

	Size rowCount() const
		/// Returns number of rows.
	{
		return _pData->size();
	}

	void reset()
		/// Clears and shrinks the storage.
	{
		Container().swap(*_pData);
		_deque.clear();
	}

	const std::string& name() const
		/// Returns column name.
	{
		return _metaColumn.name();
	}

	std::size_t length() const
		/// Returns column maximum length.
	{
		return _metaColumn.length();
	}

	std::size_t precision() const
		/// Returns column precision.
		/// Valid for floating point fields only (zero for other data types).
	{
		return _metaColumn.precision();
	}

	std::size_t position() const
		/// Returns column position.
	{
		return _metaColumn.position();
	}

	MetaColumn::ColumnDataType type() const
		/// Returns column type.
	{
		return _metaColumn.type();
	}

	Iterator begin() const
		/// Returns iterator pointing to the beginning of data storage vector.
	{
		return _pData->begin();
	}

	Iterator end() const
		/// Returns iterator pointing to the end of data storage vector.
	{
		return _pData->end();
	}

private:
	Column();

	MetaColumn _metaColumn;
	Poco::SharedPtr<Container> _pData;
	mutable std::deque<bool> _deque;
};


template <class T>
class Column<T, std::list<T> >
	/// Column specialization for std::list
{
public:
	typedef std::list<T> List;
	typedef typename List::const_iterator Iterator;
	typedef typename List::const_reverse_iterator RIterator;
	typedef typename List::size_type Size;

	Column(const MetaColumn& metaColumn, std::list<T>* pData): _metaColumn(metaColumn), _pData(pData)
		/// Creates the Column.
	{
		poco_check_ptr (_pData);
	}

	Column(const Column& col): _metaColumn(col._metaColumn), _pData(col._pData)
		/// Creates the Column.
	{
	}

	~Column()
		/// Destroys the Column.
	{
	}

	Column& operator = (const Column& col)
		/// Assignment operator.
	{
		Column tmp(col);
		swap(tmp);
		return *this;
	}

	void swap(Column& other)
		/// Swaps the column with another one.
	{
		std::swap(_metaColumn, other._metaColumn);
		std::swap(_pData, other._pData);
	}

	List& data()
		/// Returns reference to contained data.
	{
		return *_pData;
	}

	const T& value(std::size_t row) const
		/// Returns the field value in specified row.
		/// This is the std::list specialization and std::list
		/// is not the optimal solution for cases where random 
		/// access is needed.
		/// However, to allow for compatibility with other
		/// containers, this functionality is provided here.
		/// To alleviate the problem, an effort is made
		/// to start iteration from beginning or end,
		/// depending on the position requested.
	{
		if (row <= (std::size_t) (_pData->size() / 2))
		{
			Iterator it = _pData->begin();
			Iterator end = _pData->end();
			for (int i = 0; it != end; ++it, ++i)
				if (i == row) return *it;
		}
		else
		{
			row = _pData->size() - row;
			RIterator it = _pData->rbegin();
			RIterator end = _pData->rend();
			for (int i = 1; it != end; ++it, ++i)
				if (i == row) return *it;
		}

		throw RangeException("Invalid row number."); 
	}

	const T& operator [] (std::size_t row) const
		/// Returns the field value in specified row.
	{
		return value(row);
	}

	Size rowCount() const
		/// Returns number of rows.
	{
		return _pData->size();
	}

	void reset()
		/// Clears the storage.
	{
		_pData->clear();
	}

	const std::string& name() const
		/// Returns column name.
	{
		return _metaColumn.name();
	}

	std::size_t length() const
		/// Returns column maximum length.
	{
		return _metaColumn.length();
	}

	std::size_t precision() const
		/// Returns column precision.
		/// Valid for floating point fields only (zero for other data types).
	{
		return _metaColumn.precision();
	}

	std::size_t position() const
		/// Returns column position.
	{
		return _metaColumn.position();
	}

	MetaColumn::ColumnDataType type() const
		/// Returns column type.
	{
		return _metaColumn.type();
	}

	Iterator begin() const
		/// Returns iterator pointing to the beginning of data storage vector.
	{
		return _pData->begin();
	}

	Iterator end() const
		/// Returns iterator pointing to the end of data storage vector.
	{
		return _pData->end();
	}

private:
	Column();

	MetaColumn _metaColumn;
	Poco::SharedPtr<List> _pData;
};


} } // namespace Poco::Data


#endif // Data_Column_INCLUDED

