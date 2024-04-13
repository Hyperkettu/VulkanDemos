#include "pch.h"

#include "core/JSONObject.h"
#include "core/JSONValueArray.h"
#include "core/JSONTypedValue.cpp"

namespace Fox {

	namespace Core {

		namespace Json {

			size_t JSONValue::objectDepth = 0u;

			std::ostream& operator<<(std::ostream& outputStream, const Fox::Core::Json::JSONValue& jsonValue) {

				const Fox::Core::Json::StringValue* stringValue = dynamic_cast<const Fox::Core::Json::StringValue*>(&jsonValue);
				const Fox::Core::Json::BoolValue* booleanValue = dynamic_cast<const Fox::Core::Json::BoolValue*>(&jsonValue);
				const Fox::Core::Json::IntValue* intValue = dynamic_cast<const Fox::Core::Json::IntValue*>(&jsonValue);
				const Fox::Core::Json::FloatValue* floatValue = dynamic_cast<const Fox::Core::Json::FloatValue*>(&jsonValue);
				const Fox::Core::Json::JSONObject* objectValue = dynamic_cast<const Fox::Core::Json::JSONObject*>(&jsonValue);
				const Fox::Core::Json::JSONValueArray* arrayValue = dynamic_cast<const Fox::Core::Json::JSONValueArray*>(&jsonValue);

				if (stringValue) {
					outputStream << *stringValue;
				} else if (booleanValue) {
					outputStream << *booleanValue;
				} else if (intValue) {
					outputStream << *intValue;
				} else if (floatValue) {
					outputStream << *floatValue;
				} else if (arrayValue) {
					Fox::Core::Json::JSONValue::objectDepth++;
					outputStream << *arrayValue;
					Fox::Core::Json::JSONValue::objectDepth--;
				} else if (objectValue) {
					Fox::Core::Json::JSONValue::objectDepth++;
					outputStream << *objectValue;
					Fox::Core::Json::JSONValue::objectDepth--;
				}

				return outputStream;
			}
		}
	}
}