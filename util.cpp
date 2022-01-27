#include <string>
#include <sstream>

std::string GetFileName(std::string original) {
	if (original.find('\\') == std::string::npos) {
		return std::string(original);
	}
	else {
		std::string result;
		std::string buf;
		std::stringstream ss = std::stringstream(original);
		while (std::getline(ss, buf, '\\')) {
			result = buf;
		}
		return result;
	}
}