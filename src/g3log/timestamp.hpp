#pragma once
#include <ctime>
#include <functional>
#include <memory>
#include <string>

#include "g3log/time.hpp"


namespace g3 {


	struct Timestamp {
		virtual std::string getLocalTimestamp(const std::string& format = "") = 0;
	};


	struct DefaultTimestamp : public Timestamp {
							DefaultTimestamp(const std::string&);
							DefaultTimestamp(const DefaultTimestamp*);
		virtual            ~DefaultTimestamp();
		virtual std::string getLocalTimestamp(const std::string&);

		const std::time_t  _timestamp;
		const std::string& _defaultFormat;
	};


	class TimestampProvider {
		public:
									   TimestampProvider(const std::string = {g3::internal::date_formatted  + " " + g3::internal::time_formatted});
									  ~TimestampProvider();
			std::shared_ptr<Timestamp> generate();
			void                       setGenerator(std::function<std::shared_ptr<Timestamp> ()> = NULL);
			void                       setDefaultFormat(std::string format);

		protected:
			std::shared_ptr<Timestamp> defaultGenerator();

		private:
			std::function<std::shared_ptr<Timestamp> ()> _generator;
			std::string                                  _defaultFormat;
	};


	TimestampProvider& timestamp_provider();
}
