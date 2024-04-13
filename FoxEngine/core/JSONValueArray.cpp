#include "pch.h"

#include "core/JSONObject.h"

namespace Fox {

	namespace Core {

		namespace Json {

			JSONValueArray::~JSONValueArray() {
				for (size_t i = 0u; i < values.size(); i++) {
					if (values[i])
						delete values[i];
				}
			}

			JSONValue& JSONValueArray::operator[](size_t index) {
				return *values[index];
			}


			std::ostream& operator<<(std::ostream& outputStream, const Fox::Core::Json::JSONValueArray& object) {

				size_t elementCount = 0u;

				std::string paddingObject = "";

				for (size_t i = 0u; i < Fox::Core::Json::JSONValue::objectDepth; i++) {
					paddingObject += "\t";
				}

				outputStream << "\n" << paddingObject << "[\n";

				for (std::vector<Fox::Core::Json::JSONValue*>::const_iterator iter = object.values.begin(); iter != object.values.end(); iter++) {

					outputStream << paddingObject << **iter;

					if (elementCount < object.values.size() - 1) {
						outputStream << ",";
					}
					outputStream << "\n";
					elementCount++;
				}

				outputStream << paddingObject << "]";

				return outputStream;
			}
		}
	}
}