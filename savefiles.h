#include <fstream>

class savefiles {
public:
	std::vector<std::string> read_file(std::string path) {
		std::ifstream file(path.c_str());
	}

	void write_file(const char* path, std::vector<std::string>) {

	}
};