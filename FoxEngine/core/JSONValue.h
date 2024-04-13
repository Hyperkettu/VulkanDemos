#pragma once

#include <iostream>
#include <string>
#include <memory>

namespace Fox {

	namespace Core {

		namespace Json {

			class JSONValue {
			public:
				JSONValue() = default;
				virtual ~JSONValue() = default;
				friend std::ostream& operator<<(std::ostream& outputStream, const Fox::Core::Json::JSONValue& jsonValue);

				static size_t objectDepth;
			};
		}
	}
}