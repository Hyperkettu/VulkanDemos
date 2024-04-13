#pragma once

#include "core/JSONValue.h"

namespace Fox {

	namespace Core {

		namespace Json {

			template<class ValueType>
			class TypedValue : public JSONValue {
			public:
				TypedValue() = default;
				TypedValue(const char* string) : value(std::string(string)) {}
				TypedValue(ValueType value) : value(value) {}
				virtual ~TypedValue() = default;
				ValueType& GetValue() {
					return value;
				}
				template<class U>
				friend std::ostream& operator<<(std::ostream& outputStream, const Fox::Core::Json::TypedValue<U>& jsonValue);
			public:
				ValueType value;
			};

			typedef Fox::Core::Json::TypedValue<std::string> StringValue;
			typedef Fox::Core::Json::TypedValue<bool> BoolValue;
			typedef Fox::Core::Json::TypedValue<int> IntValue;
			typedef Fox::Core::Json::TypedValue<float> FloatValue;
		}
	}
}