#pragma once

#include "core/JSONValue.h"

#include <vector>

namespace Fox {

	namespace Core {

		namespace Json {

			class JSONValueArray : public JSONValue {
			public:
				JSONValueArray() = default;
				~JSONValueArray();

				void fromArray(Fox::Core::Json::JSONValue* array[], size_t arrayLength) {
					for (size_t i = 0u; i < arrayLength; i++) {
						values.push_back(array[i]);
					}
				}

				void Set(size_t index, Fox::Core::Json::JSONValue& value) {
					values[index] = &value;
				}

				template<class U>
				U& Get(size_t index) {
					U* value = dynamic_cast<U*>(values[index]);
					if (value) {
						return *value;
					} else {
						U dummy;
						return dummy;
					}
				}

				JSONValue& operator[](size_t index);

				friend std::ostream& operator<<(std::ostream& outputStream, const Fox::Core::Json::JSONValueArray& object);

			private:
				std::vector<JSONValue*> values;
			};
		}
	}
}