#ifndef COUNTED_STRING_H
#define COUNTED_STRING_H
#include <stdexcept>
#include <string>
#include <atomic>
#include <cstring>

struct CountedStringHandle
{
	uint32_t         length{0};
	std::atomic<int> refCount{1};
	char             data[1]{0};
};

class counted_string
{
public:
	static counted_string Get(const char * str);
	static counted_string Get(std::string const& str) { return Get(str.c_str()); }

	static counted_string MakeShared(const char * str);
	static counted_string MakeShared(std::string const& str) { return MakeShared(str.c_str()); }

	static counted_string MakeUnique(const char * str);
	static counted_string MakeUnique(std::string const& str) { return MakeShared(str.c_str()); }

	counted_string();
	counted_string(counted_string const& in);
	counted_string(counted_string && in);
	~counted_string();

	void clear();

	inline int  ptr_compare(counted_string const &in) const { return (intptr_t)m_string - (intptr_t)in.m_string; }

	inline int  compare(counted_string const&in) const { return compare(in.c_str()); }
	inline int  compare(std::string const&in) const { return compare(in.c_str()); }
	inline int  compare(const char * in) const { return strcmp(m_string->data, in); }

	inline bool operator==(std::string const& in) const { return compare(in) == 0; };
	inline bool operator!=(std::string const& in) const { return compare(in) != 0; };

	inline bool operator==(const char * in) const { return compare(in) == 0; };
	inline bool operator!=(const char * in) const { return compare(in) != 0; };

	inline bool operator==(counted_string const& in) const { return m_string == in.m_string; };
	inline bool operator!=(counted_string const& in) const { return m_string != in.m_string; };

	template<typename T> inline	bool operator< (T const& in) const { return compare(in) <  0; };
	template<typename T> inline	bool operator<=(T const& in) const { return compare(in) <= 0; };
	template<typename T> inline	bool operator> (T const& in) const { return compare(in) >  0; };
	template<typename T> inline bool operator>=(T const& in) const { return compare(in) >= 0; };


	counted_string & operator=(counted_string const& in);
	counted_string & operator=(counted_string && in);

	char at(size_t it) const
	{
		if(it >= length())
			throw std::out_of_range("counted_string");

		return m_string->data[it];
	}

	inline const char & operator[](size_t it) const { return m_string->data[it]; }

	inline const char * c_str() const { return &m_string->data[0]; }

	inline bool   empty()  const { return m_string->length == 0; }
	inline size_t size()   const { return m_string->length; }
	inline size_t length() const { return m_string->length; }

	std::string toStdString() const;

	void swap(counted_string & it);

private:
typedef CountedStringHandle handle;
	static handle null_handle;
	static handle * GetString(const char * string);
	static void     ReleaseString(handle *);

	explicit counted_string(handle * it);

	handle * m_string{0L};
};


#endif // COUNTED_STRING_H
