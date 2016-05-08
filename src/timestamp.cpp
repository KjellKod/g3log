#include <g3log/timestamp.hpp>


namespace g3 {


	DefaultTimestamp::DefaultTimestamp(const std::string& defaultFormat) :
		_timestamp(g3::systemtime_now()),
		_defaultFormat(defaultFormat) {
	}


	DefaultTimestamp::DefaultTimestamp(const DefaultTimestamp* copy) :
		_timestamp(copy->_timestamp),
		_defaultFormat(copy->_defaultFormat) {
	}


	DefaultTimestamp::~DefaultTimestamp() {
	}


	std::string DefaultTimestamp::getLocalTimestamp(const std::string& format) {
		return g3::localtime_formatted(_timestamp, (format.empty() ? _defaultFormat : format));
	}


	TimestampProvider::TimestampProvider(const std::string defaultFormat) :
		_defaultFormat(defaultFormat) {
		setGenerator();
	}


	TimestampProvider::~TimestampProvider() {
	}


	std::shared_ptr<Timestamp> TimestampProvider::generate() {
		return _generator();
	}


	void TimestampProvider::setGenerator(std::function<std::shared_ptr<Timestamp> ()> generator) {
		if (generator) {
			_generator = generator;
		} else {
			_generator = std::bind(&TimestampProvider::defaultGenerator, this);
		}
	}


	void TimestampProvider::setDefaultFormat(std::string format) {
		_defaultFormat = format;
	}


	std::shared_ptr<Timestamp> TimestampProvider::defaultGenerator() {
		return std::make_shared<DefaultTimestamp>(new DefaultTimestamp(_defaultFormat));
	}
}
