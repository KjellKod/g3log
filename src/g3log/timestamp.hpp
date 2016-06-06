#pragma once
#include <ctime>
#include <string>


namespace g3 {


	struct Timestamp {
			    Timestamp(std::string&);
			    Timestamp(std::time_t, int64_t, std::string&);
			    Timestamp(const Timestamp*);
			   ~Timestamp();
		std::string getLocalTimestamp(const std::string& = "") const;
		Timestamp&  operator=(Timestamp other);
		friend void swap(Timestamp& first, Timestamp& second);

		std::time_t  _time;           /* time as std::time_t which is #sec since enoch */
		uint64_t     _microsec;       /* microsecond part of the timestamp [0-1000000] */
		std::string& _defaultFormat;  /* default format used by getLocalTimestamp      */
	};
}
