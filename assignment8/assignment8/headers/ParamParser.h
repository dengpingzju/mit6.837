#ifndef _H_PARAM_
#define _H_PARAM_

#include <map>
#include <string>
#include <vector>
#include <iostream>
using namespace std;
class ParamParser {
public:
	ParamParser(int argc, char **argv) {
		int i = 1;
		while (i<argc) {
			if (argv[i][0] == '-') {
				string paramName = string(argv[i] + 1);
				if (paramName.empty()) {
					cerr << "Error: No parameter name." << endl;
					return;
				}
				else {
					params[paramName] = vector<string>();
				}
				int j;
				for (j = i + 1; j < argc; j++) {
					if (argv[j][0] == '-') break;
					else {
						params[paramName].push_back(string(argv[j]));
					}
				}
				i = j;
			}
			else {
				cerr << "Error: No parameter name." << endl;
				return;
			}
		}
	}

	int getParam(const string &name, int index,string &ret) {
		auto iter = params.find(name);
		if (iter == params.end()) return -1; //no such param
		else {
			vector<string> &values = iter->second;
			if (index < 0 || index >= (int)values.size()) {
				return -2;//out of range
			}
			else {
				ret = values[index];
				return 0;//ok
			}
		}
	}
private:
	map<string, vector<string>> params;
};


#endif
