#include "pch.h"

namespace Fox {

	namespace Core {

		namespace Json {

			JSONObject::~JSONObject() {
				for (auto iter = attributes.begin(); iter != attributes.end(); iter++) {
					delete iter->second;
				}
			}

			Fox::Core::Json::JSONValue& JSONObject::operator[](std::string& key) {
				return *attributes[key];
			}

			std::ostream& operator<<(std::ostream& outputStream, const JSONObject& object) {
				outputStream << "{\n";
				size_t attributeCount = 0u;
				std::string paddingObject = "";
				std::string paddingObjectEnd = "";

				for (size_t i = 0u; i < Fox::Core::Json::JSONValue::objectDepth; i++) {
					paddingObject += "\t";

					if (i < Fox::Core::Json::JSONValue::objectDepth - 1)
						paddingObjectEnd += "\t";
				}

				for (std::map<std::string, Fox::Core::Json::JSONValue*>::const_iterator iter = object.attributes.begin(); iter != object.attributes.end(); iter++) {

					Fox::Core::Json::JSONObject* childObject = dynamic_cast<Fox::Core::Json::JSONObject*>(iter->second);
					std::string sign = childObject ? "\n" : " ";

					outputStream << paddingObject << "\"" << iter->first << "\":" << sign;
					outputStream << paddingObject << *(iter->second);

					if (attributeCount < object.attributes.size() - 1) {
						outputStream << ",\n";
					}
					attributeCount++;
				}
				outputStream << ("\n" + paddingObjectEnd + "}");
				return outputStream;
			}
		}
	}
}