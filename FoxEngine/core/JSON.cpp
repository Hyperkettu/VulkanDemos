#include "pch.h"

#include <algorithm>
#include <sstream>
#include <cassert>

namespace Fox {

	namespace Core {

		namespace Json {

			JSON::~JSON() {
				delete root;
			}

			void JSON::Parse(std::string& file) {

				char chars[] = "\t \n";

				for (size_t index = 0u; index < strlen(chars); index++)
					file.erase(std::remove(file.begin(), file.end(), chars[index]), file.end());

				size_t index = file.find('{');

				if (index == 0u) {
					JSONObject* object = new Fox::Core::Json::JSONObject;
					ParseValue(object, file);
					SetRoot(*object);
				}

				index = file.find('[');

				if (index == 0u) {
					JSONValueArray* objectArray = new Fox::Core::Json::JSONValueArray;
					ParseArray(objectArray, file);
					SetRoot(*objectArray);
				}

			}

			void JSON::ParseValue(Fox::Core::Json::JSONObject* object, std::string& file) {

				size_t objectEnd;
				do {
					objectEnd = file.rfind('}');
					size_t objectEnds = file.find('}');

					if (objectEnd == 0u || objectEnds == 0u) {
						file = file.substr(1u);
						break;
					}

					size_t index = file.find('\"', 1u);
					if (index > 0u) {
						size_t endIndex = file.find('\"', index + 1u);
						if (endIndex > index) {
							std::string attributeName = file.substr(index + 1u, endIndex - index - 1u);
#if defined(_DEBUG)
							std::cout << "Parsing attribute: " << attributeName << std::endl;
#endif
							file = file.substr(endIndex + 1);

							index = file.find(':');

							if (index != 0u) {
								assert("Not valid JSON file. Missing semicolon.");
							}

							size_t valueEnd = file.find(',');
							size_t valueEnd2 = file.find('}');

							if (valueEnd > 0u || valueEnd2 > 0u) {
								bool boolValueSolved = ParseBoolValue(object, attributeName, valueEnd, valueEnd2, file);
								bool numberValueSolved = false;
								bool stringValueSolved = false;

								if (!boolValueSolved) {
									numberValueSolved = ParseNumberValue(object, attributeName, valueEnd, valueEnd2, file);
								}

								if (!boolValueSolved && !numberValueSolved) {
									stringValueSolved = ParseStringValue(object, attributeName, valueEnd, valueEnd2, file);
								}

								bool objectValueSolved = false;

								if (!boolValueSolved && !numberValueSolved && !stringValueSolved) {
									index = file.find('{', 1u);

									if (index == 1u) {
										file = file.substr(1u);
										Fox::Core::Json::JSONObject* child = new Fox::Core::Json::JSONObject;
										ParseValue(child, file);
										object->Set(attributeName, child);
										objectValueSolved = true;
									}
								}

								bool arrayValueSolved = false;

								if (!boolValueSolved && !numberValueSolved && !stringValueSolved && !objectValueSolved) {
									index = file.find('[', 1u);

									if (index == 1u) {
										Fox::Core::Json::JSONValueArray* array = new Fox::Core::Json::JSONValueArray;
										ParseArray(array, file);
										object->Set(attributeName, array);
										arrayValueSolved = true;
									}
								}

								if (!boolValueSolved && !numberValueSolved && !stringValueSolved && !objectValueSolved && !arrayValueSolved) {
									assert("File is not valid JSON. Could not solve value.");
								} else {

									if (objectValueSolved || arrayValueSolved) {
										valueEnd = file.find(',');
										valueEnd2 = file.find('}');
									}

									if (valueEnd < valueEnd2) {
										file = file.substr(valueEnd);
									} else {
										file = file.substr(valueEnd2);
									}
								}
							} else {
								std::cout << "File is not valid JSON." << std::endl;
							}

						} else {
							std::cout << "Could not find object attributes names. " << std::endl;
						}
					} else {
						std::cout << "Could not find object attributes names anymore." << std::endl;
					}
				} while (objectEnd != 0u);
			}

			bool JSON::ParseBoolValue(Fox::Core::Json::JSONObject* object, std::string& attributeName, size_t comma, size_t endOfObject, std::string& file) {
				std::string truthValue = "true";
				size_t index = file.find(truthValue, 1u);
				if (index > 0u && (index < comma && index < endOfObject)) {
					Fox::Core::Json::BoolValue* value = new Fox::Core::Json::BoolValue(true);
					object->Set(attributeName, value);
#if defined (_DEBUG)
					std::cout << "Resolved with value " << true << std::endl;
#endif
					return true;
				}
				truthValue = "false";
				index = file.find(truthValue, 1u);

				if (index > 0u && (index < comma || (index < endOfObject && comma > endOfObject))) {
					Fox::Core::Json::BoolValue* value = new Fox::Core::Json::BoolValue(false);
					object->Set(attributeName, value);
#if defined (_DEBUG)
					std::cout << "Resolved with value " << false << std::endl;
#endif
					return true;
				}

				return false;
			}

			bool JSON::ParseNumberValue(Fox::Core::Json::JSONObject* object, const std::string& attributeName, size_t comma, size_t endOfObject, std::string& file) {
				std::string number = file.substr(1u, comma - 1u);
				size_t commaIndex = number.find('.');
				std::stringstream stringStream(number);
				bool numberValueSolved = false;

				if (commaIndex > 0u && commaIndex < std::string::npos) {
					float floatValue;
					if (stringStream >> floatValue) {
						Fox::Core::Json::FloatValue* value = new Fox::Core::Json::FloatValue(floatValue);
						object->Set(attributeName, value);
#if defined (_DEBUG)
						std::cout << "Resolved with value " << value->value << std::endl;
#endif
						numberValueSolved = true;
					} else {
						assert("Could not convert to float value");
					}
				} else {
					int intValue;
					if (stringStream >> intValue) {
						IntValue* value = new IntValue(intValue);
						object->Set(attributeName, value);
#if defined (_DEBUG)
						std::cout << "Resolved with value " << value->value << std::endl;
#endif
						numberValueSolved = true;
					} else {
						assert("Could not convert to int value");
					}
				}

				if (!numberValueSolved) {
					std::string number2 = file.substr(1u, endOfObject - 1u);
					size_t commaIndex2 = number2.find('.');
					std::stringstream stringStream2(number2);
					float floatValue;

					if (commaIndex2 > 0u && commaIndex2 < std::string::npos) {
						if (stringStream2 >> floatValue) {
							Fox::Core::Json::FloatValue* value = new Fox::Core::Json::FloatValue(floatValue);
							object->Set(attributeName, value);
#if defined (_DEBUG)
							std::cout << "Resolved with value " << value->value << std::endl;
#endif
							numberValueSolved = true;
						} else {
							assert("Could not convert to float value");
						}

					} else {
						int intValue;
						if (stringStream >> intValue) {
							Fox::Core::Json::IntValue* value = new Fox::Core::Json::IntValue(intValue);
							object->Set(attributeName, value);
#if defined (_DEBUG)
							std::cout << "Resolved with value " << value->value << std::endl;
#endif
							numberValueSolved = true;
						} else {
							assert("Could not convert to int value");
						}
					}
				}

				return numberValueSolved;
			}

			bool JSON::ParseStringValue(JSONObject* object, const std::string& attributeName, size_t comma, size_t endOfObject, std::string& file) {
				std::string value = file.substr(1u, comma - 1u);
				if (value[0] == '\"' && value[value.size() - 1u] == '\"') {
					Fox::Core::Json::StringValue* stringValue = new Fox::Core::Json::StringValue(value.substr(1u, value.size() - 2u));
					object->Set(attributeName, stringValue);
#if defined (_DEBUG) 
					std::cout << "Resolved with value "  << stringValue << std::endl;
#endif
					return true;
				}

				if (value[0] == '\"' && value[value.size() - 2u] == '\"') {
					Fox::Core::Json::StringValue* stringValue = new Fox::Core::Json::StringValue(value.substr(1u, value.size() - 3u));
					object->Set(attributeName, stringValue);
#if defined (_DEBUG) 
					std::cout << "Resolved with value " << stringValue << std::endl;
#endif
					return true;
				}

				size_t count = std::count(value.begin(), value.end(), '}');
				size_t count2 = std::count(value.begin(), value.end(), ']');


				if (count > 0u) {
					if (value[0] == '\"' && value[value.size() - count - count2 - 1u] == '\"') {
						Fox::Core::Json::StringValue* stringValue = new Fox::Core::Json::StringValue(value.substr(1u, value.size() - count - count2 - 2u));
						object->Set(attributeName, stringValue);
#if defined (_DEBUG) 
					
						std::cout << "Resolved with value " << stringValue << std::endl;
#endif
						return true;
					}
				}

				return false;
			}

			bool JSON::ParseArray(JSONValueArray* array, std::string& file) {

				size_t arrayEndIndex = file.find(']', 1u);
				size_t commaIndex = file.find(',', 1u);
				size_t cellStart = 2u;

				if (arrayEndIndex == std::string::npos)
					return false;

				std::vector<Fox::Core::Json::JSONValue*> values;

				do {
					std::string indexData;
					size_t objectStart = file.find("{", cellStart);
					size_t objectEnd = ~0u;
					if (objectStart == 1u || objectStart == 2u || file.find("{") == 1u) {
						objectEnd = file.find("}", cellStart);
						Fox::Core::Json::JSONObject* object = new Fox::Core::Json::JSONObject;
						ParseValue(object, file);
						values.push_back(object);
					}

					arrayEndIndex = file.find(']', 1u);

					if (arrayEndIndex == std::string::npos || file.find("],") == 0u) {
						break;
					}

					if (file.find(',') == 0u) {
						continue;
					}

					commaIndex = file.find(',', 1u);
					indexData = file.substr(cellStart, commaIndex - cellStart);

					file = file.substr(commaIndex);
					arrayEndIndex = file.find(']');
					commaIndex = file.find(',', 1u);
					cellStart = 1u;

					ParseIndexData(values, indexData);


					if (commaIndex > arrayEndIndex) {
						indexData = file.substr(1u, arrayEndIndex - 1u);
						ParseIndexData(values, indexData);
						file = file.substr(arrayEndIndex + 1u);
					}

				} while (commaIndex < arrayEndIndex);

				array->fromArray(&values[0], values.size());

				return true;
			}

			std::ostream& operator<<(std::ostream& outputStream, const JSON& file) {
				outputStream << *file.root;
				return outputStream;
			}

			bool JSON::ParseIndexData(std::vector<Fox::Core::Json::JSONValue*>& array, std::string& indexData) {
				if (indexData == "false") {
					array.push_back(new Fox::Core::Json::BoolValue(false));
					return true;
				} else if (indexData == "true") {
					array.push_back(new Fox::Core::Json::BoolValue(true));
					return true;
				} else if (indexData[0] == '\"' && indexData[indexData.size() - 1u] == '\"') {
					array.push_back(new Fox::Core::Json::StringValue(indexData.substr(1u, indexData.size() - 2u)));
					return true;
				} else {

					size_t objectStartIndex = indexData.find('{');

					if (objectStartIndex == 0u) {
						Fox::Core::Json::JSONObject* object = new Fox::Core::Json::JSONObject;
						ParseValue(object, indexData);
						array.push_back(object);
						return true;
					} else if (std::isdigit(indexData[0]) || std::isdigit(indexData[1])) {
						size_t commaIndex = indexData.find('.');
						std::stringstream stringStream(indexData);
						if (commaIndex > 0u && commaIndex != std::string::npos) {
							float value;
							stringStream >> value;
							array.push_back(new Fox::Core::Json::FloatValue(value));
							return true;
						} else {
							int value;
							stringStream >> value;
							array.push_back(new Fox::Core::Json::IntValue(value));
							return true;
						}
					}

				}

				return false;

			}
		}
	}
}