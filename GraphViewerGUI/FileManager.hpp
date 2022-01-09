#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "GraphViewer.hpp"

// https://iq.opengenus.org/write-file-in-cpp/

class FileManager {
public:
	static FileManager* getFileManager() {
		static FileManager onlyInstance;
		return &onlyInstance;
	}

	void saveFile(std::string fileName);
	void loadFile(std::string fileName);

private:
	FileManager() = default;

	Model::graph_sptr currentGraph_ = nullptr;
	std::vector<std::string> lineWords_;
	std::string delimiter_ = "<!>";
};