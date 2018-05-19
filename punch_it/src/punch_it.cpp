#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>


struct result_data {
	std::vector<std::string> dunno;

};

std::string trim(std::string str) {
	std::size_t first = str.find_first_not_of(' ');
	std::string tempStr = str;
	if (first == std::string::npos) {
		return "";
	}
	else {
		while (tempStr.find_first_of(' ') != std::string::npos) {
			tempStr.erase(tempStr.find_first_of(' '), 1);
		}
	}
	return tempStr;
}

void replace_spaces(std::string &str) {
	std::size_t position = str.find_first_of(' ');
	while (position != std::string::npos) {
		str.replace(position, 1, "_");
		position = str.find_first_of(' ');
	}
}

void remove_double_spaces(std::string &str) {
	std::size_t position = str.find_first_of("  ");
	while (position != std::string::npos) {
		str.replace(position, 2, " ");
		position = str.find_first_of("  ");
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "No punch files entered" << std::endl;
		std::cin.get();
		return 0;
	}
	std::vector<std::string> file_names;
	bool mod_flags[4] = { false, false, false , false};
	// Check for modifying flags
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-t") == 0) {
			mod_flags[0] = true;
			mod_flags[1] = true;
			continue;
		}
		if (strcmp(argv[i], "-s") == 0) {
			mod_flags[0] = true;
			mod_flags[2] = true;
			continue;
		}
		if (strcmp(argv[i], "-l") == 0) {
			mod_flags[0] = true;
			mod_flags[3] = true;
			continue;
		}
		file_names.push_back(argv[i]);
	}
	if (!mod_flags[0]) {
		mod_flags[2] = true;
	}
	std::fstream working_file;
	std::string input_line, title, subtitle, label, subcase, result_type, result_line;
	std::string compare_values[5] = { "$TITLE", "$SUBTITLE", "$LABEL", "$SUBCASE ID", "-CONT-" };
	std::map<std::string, std::vector<std::string> > results_map;
	std::map<std::string, std::vector<std::string> >::iterator results_map_it;
	bool data_region = false, result_type_flag = false, skip_entry = true;
	for (int i = 1; i < file_names.size(); i++) {
		working_file.open(file_names[i], std::ios_base::in);
		if (working_file.is_open()) {
			std::cout << "Reading file: " << file_names[i] << std::endl;
			while (!working_file.eof()) {
				std::getline(working_file, input_line);
				// Check EOF
				if (working_file.eof()) {
					if (result_line.compare("") != 0) {
						results_map_it->second.push_back(result_line);
						result_line.clear();
					}
					continue;
				}
				// TITLE
				if (compare_values[0].compare(input_line.substr(0, 6)) == 0) {
					data_region = false;
					if (result_line.compare("") != 0) {
						results_map_it->second.push_back(result_line);
						result_line.clear();
					}
					title = trim(input_line.substr(10, 62));
					continue;
				}
				// Get data
				if (data_region) {
					if (compare_values[4].compare(input_line.substr(0, 6)) != 0) {
						if (skip_entry) {
							skip_entry = false;
						}
						else {
							results_map_it->second.push_back(result_line);
							result_line.clear();
						}
						if (mod_flags[1]) {
							result_line.append(title);
							result_line.append(",");
						}
						if (mod_flags[2]) {
							result_line.append(subtitle);
							result_line.append(",");
						}
						if (mod_flags[3]) {
							result_line.append(label);
							result_line.append(",");
						}
						result_line.append(subcase);
						result_line.append(",");
						result_line.append(trim(input_line.substr(0, 18)));
					}
					result_line.append(",");
					result_line.append(trim(input_line.substr(18, 18)));
					result_line.append(",");
					result_line.append(trim(input_line.substr(36, 18)));
					result_line.append(",");
					result_line.append(trim(input_line.substr(54, 18)));
					continue;
				}
				// SUBCASE
				if (compare_values[3].compare(input_line.substr(0, 11)) == 0) {
					subcase = trim(input_line.substr(13, 59));
					if (working_file.peek() == '$') {
						std::getline(working_file, input_line);
						result_type.append(trim(input_line.substr(15, 57)));
					}
					remove_double_spaces(result_type);
					replace_spaces(result_type);
					std::vector<std::string> entry_vector;
					auto ret = results_map.insert(std::make_pair(result_type, entry_vector));
					results_map_it = ret.first;
					data_region = true;
					skip_entry = true;
					continue;
				}
				if (result_type_flag) {
					result_type_flag = false;
					result_type = trim(input_line.substr(1, 71));
					continue;
				}
				// LABEL
				if (compare_values[2].compare(input_line.substr(0, 6)) == 0) {
					label = trim(input_line.substr(10, 62));
					result_type_flag = true;
					continue;
				}
				// SUBTITLE
				if (compare_values[1].compare(input_line.substr(0, 9)) == 0) {
					subtitle = trim(input_line.substr(10, 62));
					continue;
				}
			}
			working_file.close();
		}
		else {
			std::cout << "Unable to open file: " << file_names[i] << std::endl;
		}
	}
	for (results_map_it = results_map.begin(); results_map_it != results_map.end(); results_map_it++) {
		std::string file_name = results_map_it->first;
		file_name.append(".csv");
		working_file.open(file_name.c_str(), std::ios_base::out);
		if (working_file.is_open()) {
			std::cout << "Writing data to " << file_name << std::endl;
			if (mod_flags[1]) {
				working_file << "Title,";
			}
			if (mod_flags[2]) {
				working_file << "Subtitle,";
			}
			if (mod_flags[3]) {
				working_file << "Label,";
			}
			working_file << "Subcase\n";
			for (std::size_t i = 0; i < results_map_it->second.size(); i++)
				working_file << results_map_it->second[i] << '\n';
		}
		else {
			std::cout << "Unable to open " << file_name << " for output" << std::endl;
		}
		working_file.close();
	}
	std::cout << "Writing to csv files complete, press ENTER to exit" << std::endl;
	std::cin.get();
}