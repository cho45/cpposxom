//#!g++ -I/usr/include -lboost_filesystem cpposxom.cpp -o a.out && ./a.out ; rm ./a.out ;:

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <time.h>
#include <stdlib.h>

#define TYPE(x) #x
#define DEBUG(x) std::cerr << #x << " = " << (x) << " (L" << __LINE__ << ")" << " " << __FILE__ << std::endl;

namespace fs = boost::filesystem;
using namespace std;

class Entry;

typedef boost::shared_ptr<Entry> EntryPtr;
typedef map<string,string> Params;

class Entry {
	public:
	
	string title;
	string body;
	string path;
	time_t mtime;

	Entry(fs::path p) : path(p.native_file_string()) {
		mtime = last_write_time(p);
		ifstream fin(path.c_str());

		getline(fin, title);

		while (!fin.eof()) {
			string line;
			getline(fin, line);
			body.append(line);
		}
	}

	Params& as_params(Params& params) {
		char t[100];
		

		params["title"] = title;
		params["body"]  = body;
		params["path"]  = path;
		params["mtime"] = mtime;

		strftime(t, 100, "%Y-%m-%d %H:%M:%S", localtime(&mtime));
		params["time"]  = string(t);
		return params;
	}

	~Entry() {
	}
};

class SimpleTemplate {
	public:
	string path;
	string body;

	SimpleTemplate(fs::path p) : path(p.native_file_string()) {
		ifstream fin(path.c_str());
		while (!fin.eof()) {
			string line;
			getline(fin, line);
			body.append(line);
			body.append("\n");
		}
	}

	string& operator()(string& buffer, Params params) {
		buffer = body;
		for (Params::iterator it = params.begin(); it != params.end(); ++it) {
			int start = buffer.find("$" + it->first);
			if (start == string::npos) continue;
			buffer.replace(start, it->first.size() + 1, it->second);
		}
		return buffer;
	}

	string& operator()(string& buffer) {
		return (*this)(buffer, Params());
	}
};

bool sort_by_mtime (const EntryPtr& a, const EntryPtr& b) {
	return a->mtime > b->mtime;
}

class Cpposxom {
	public:

	const string base;
	vector<EntryPtr> entries;

	Cpposxom(const string base) : base(base) {
	}

	void run() {
		collect_entries(base);
		sort(entries.begin(), entries.end(), sort_by_mtime);


		SimpleTemplate content_type ("content_type.html");
		SimpleTemplate head         ("head.html");
		SimpleTemplate story        ("story.html");
		SimpleTemplate foot         ("foot.html");

		string buf;
		cout << "Content-Type: " << content_type(buf);
		cout << "\n\n";
		cout << head(buf);

		for (unsigned int i = 0; i < entries.size(); i++) {
			Params params;
			EntryPtr entry = entries[i];
			cout << story(buf, entry->as_params(params));
		}

		cout << foot(buf);
	}

	void collect_entries (const string dir) {
		fs::directory_iterator end_itr;
		for (fs::directory_iterator itr(dir); itr != end_itr; itr++) {
			if (is_directory(itr->status())) {
				collect_entries(itr->path().native_file_string());
			} else {
				EntryPtr entry( new Entry(itr->path()) );
				entries.push_back(entry);
			}
		}
	}
};

int main (int argc, char *argv[]) {
//	char* path_info = getenv("PATH_INFO");
//	string env = path_info == NULL ? string() : string(path_info);
//	DEBUG(env);
	Cpposxom("data").run();
}
