#pragma once

#include <vector>
#include <cassert>

#include "core/JSONObject.h"
#include "core/JSONValueArray.h"
#include "core/JSONTypedValue.h"

namespace Fox {

	namespace Core {

		namespace Json {

			class JSON {

			public:
				JSON() {}
				~JSON();

				template<class ValueType, typename std::enable_if<std::is_base_of<Fox::Core::Json::JSONValue, ValueType>::value>::type* = nullptr>
				void SetRoot(ValueType& value) {
					root = &value;
				}

				template<class U>
				U& Get() {
					U* ptr = dynamic_cast<U*>(root);
					return *ptr;
				}

				void Parse(std::string& file);

				friend std::ostream& operator<<(std::ostream& outputStream, const Fox::Core::Json::JSON& object);

			private:
				void ParseValue(Fox::Core::Json::JSONObject* parent, std::string& file);
				bool ParseBoolValue(Fox::Core::Json::JSONObject* object, std::string& attributeName, size_t comma, size_t endOfObject, std::string& file);
				bool ParseNumberValue(Fox::Core::Json::JSONObject* object, const std::string& attributeName, size_t comma, size_t endOfObject, std::string& file);
				bool ParseStringValue(Fox::Core::Json::JSONObject* object, const std::string& attributeName, size_t comma, size_t endOfObject, std::string& file);
				bool ParseArray(Fox::Core::Json::JSONValueArray* array, std::string& file);
				bool ParseIndexData(std::vector<Fox::Core::Json::JSONValue*>& array, std::string& indexData);

				Fox::Core::Json::JSONValue* root;
			};
		}
	}
}