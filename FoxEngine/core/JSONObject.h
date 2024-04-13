#pragma once

#include <map>
#include <string>

#include "core/JSONValue.h"

namespace Fox {

	namespace Core {

		namespace Json {

			class JSONObject : public Fox::Core::Json::JSONValue {
			public:

				JSONObject() = default;
				JSONObject(JSONObject& object) {
					attributes = object.attributes;
				}
				virtual ~JSONObject();

				void Set(const std::string& key, Fox::Core::Json::JSONValue* value) {
					attributes[key] = value;
				}

				Fox::Core::Json::JSONValue& operator[](std::string& key);

				Fox::Core::Json::JSONObject& Get() {
					return *this;
				}

				template<class U>
				U& Get(const char* key) {
					const std::string str(key);
					U* typedValuePtr = dynamic_cast<U*>(attributes[str]);
					if (typedValuePtr) {
						return *typedValuePtr;
					} else {
						U value;
						return value;
					}

				}

				friend std::ostream& operator<<(std::ostream& outputStream, const Fox::Core::Json::JSONObject& object);

			private:
				std::map<std::string, Fox::Core::Json::JSONValue*> attributes;
			};
		}
	}
}