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

	void saveFile(std::string fileName) { // writing on a file, it will be created auto if it does not exist
		std::ofstream outFile(fileName, std::ios::out); // file(fileName, std::ios::app) app to append on current elem instead of overwriting
		if (outFile.is_open()) {
			Model::GraphViewer* graphViewer = Model::GraphViewer::getGraphViewer();
			for (auto&& graph : graphViewer->getGraphsVector()) {
				outFile << "Graph " << graph->getName() << " " << std::endl; // important to put space at the end for the delimiter
				for (auto&& node : graph->getNodes()) {
					outFile << "Node " << node->getName() << " " << node->getPos().first << " " << node->getPos().second << " " << std::endl;
				}
				for (auto&& vertex : graph->getVertices()) {
					outFile << "Vertex " << vertex->getPreviousNode()->getName() << " " << vertex->getNode()->getName() << " " << vertex->getWeight() << " " << std::endl;
				}
				outFile << "GraphCompleted " << std::endl;
			}
		}
		outFile.close();
	}

	void loadFile(std::string fileName) {
		std::ifstream inFile(fileName, std::ios::in); // it won't be created auto if the file does not exist
		//verify if it exist

		if (inFile.is_open()) {
			for (std::string line; getline(inFile, line);) {
				// put the words of the line in an array of words
				size_t pos;
				while ((pos = line.find(delimiter_)) != std::string::npos) {
					lineWords_.push_back(line.substr(0, pos));
					line.erase(0, pos + delimiter_.length());
				}
				if (lineWords_[0] == "Graph") {
					Model::graph_sptr newGraph = std::make_shared<Model::Graph>(lineWords_[1]); // lineWords_[1] is the graph name
					currentGraph_ = newGraph; // useless line?
					Model::GraphViewer::getGraphViewer()->addGraph(currentGraph_); // also update the view
					lineWords_.clear();
				}
				else if (lineWords_[0] == "Node") {
					Model::node_sptr newNode = std::make_shared<Model::Node>(lineWords_[1]); // lineWords_[1] is the node name
					int x = stoi(lineWords_[2]); // lineWords_[2] is the node x pos
					int y = stoi(lineWords_[3]); // lineWords_[3] is the node y pos

					newNode->setPos(x, y);
					currentGraph_->addNodeAndChildren(newNode); // will also update the view by emiting a signal
					lineWords_.clear();
				}
				else if (lineWords_[0] == "Vertex") {
					Model::node_sptr parentNode;
					Model::node_sptr childNode;
					std::string parentNodeName = lineWords_[1]; //lineWords_[1] is the parentNodeName
					std::string childNodeName = lineWords_[2]; //lineWords_[2] is the childNodeName
					int weight = stoi(lineWords_[3]); // lineWords_[3] is the weight
					// If we are adding vertex it means that all the node of the current graph have been added
					for (auto&& node : currentGraph_->getNodes()) {
						if (node->getName() == parentNodeName) {
							parentNode = node;
						}
						else if (node->getName() == childNodeName) {
							childNode = node;
						}
					}
					currentGraph_->connectNodes(parentNode, childNode, weight); // will emit the signal and also update view
					lineWords_.clear();
				}
				else if (lineWords_[0] == "GraphCompleted") {
					lineWords_.clear();
				}
			}
		}
	}
private:
	FileManager() = default;

	Model::graph_sptr currentGraph_ = nullptr;
	std::vector<std::string> lineWords_;
	std::string delimiter_ = " ";
};