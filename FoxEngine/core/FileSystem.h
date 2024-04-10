#pragma once

namespace Fox {

	namespace Core {
	
		class FileSystem {
		public: 
			static std::vector<char> readBinaryFile(const std::string& fileName);
		};
	}
}