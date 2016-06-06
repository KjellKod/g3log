#include "g3log/time.hpp"
#include "g3log/timestamp.hpp"
#include <sstream>


namespace g3 {


	Timestamp::Timestamp(std::string& defaultFormat) :
		_time(g3::systemtime_now()),
		_microsec(0),
		_defaultFormat(defaultFormat) {
	}

	Timestamp::Timestamp(std::time_t time, int64_t microsec, std::string& defaultFormat) :
		_time(time),
		_microsec(microsec),
		_defaultFormat(defaultFormat) {
	}


	Timestamp::Timestamp(const Timestamp* copy) :
		_time(copy->_time),
		_microsec(copy->_microsec),
		_defaultFormat(copy->_defaultFormat) {
	}


	Timestamp::~Timestamp() {
	}


	std::string Timestamp::getLocalTimestamp(const std::string& format) const {
		std::ostringstream stringStream;
		stringStream << _microsec;

		return g3::localtime_formatted(_time, (format.empty() ? _defaultFormat : format)) + "|" + stringStream.str() + "|";
	}


	Timestamp& Timestamp::operator=(Timestamp other) {
	    swap(*this, other);
	    return *this;
	}


	void swap(Timestamp& first, Timestamp& second) {
		using std::swap;
		swap(first._time, second._time);
		swap(first._microsec, second._microsec);
		swap(first._defaultFormat, second._defaultFormat);
	}
}
