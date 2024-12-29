#pragma once

#include <string>
#include <vector>

std::string getBaseName(const std::string& path);
std::string wrapText(const std::string& text, unsigned int lineLength);
void filterMusicFiles(const std::vector<std::string>& musicFiles, const std::string& query, std::vector<std::string>& filtered);