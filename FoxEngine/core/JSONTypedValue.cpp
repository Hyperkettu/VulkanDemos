#include "pch.h"

namespace Fox {

	namespace Core {

		namespace Json {

			template<class U>
			std::ostream& operator<<(std::ostream& outputStream, const Fox::Core::Json::TypedValue<U>& jsonValue) {
				outputStream << jsonValue.value;
				return outputStream;
			}

			template<>
			inline std::ostream& operator<<(std::ostream& outputStream, const Fox::Core::Json::TypedValue<std::string>& jsonValue) {
				outputStream << "\"" << jsonValue.value << "\"";
				return outputStream;
			}

			template<>
			inline std::ostream& operator<<(std::ostream& outputStream, const Fox::Core::Json::TypedValue<bool>& jsonValue) {
				outputStream << (jsonValue.value ? "true" : "false");
				return outputStream;
			}
		}
	}
}