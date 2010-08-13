//#!g++ -I/usr/include -lboost_filesystem cpposxom.cpp -o a.out && ./a.out ; rm ./a.out ;:

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#include <boost/shared_ptr.hpp>
#include "boost/filesystem/operations.hpp"

#define TYPE(x) #x
#define DEBUG(x) std::cerr << #x << " = " << (x) << " (L" << __LINE__ << ")" << " " << __FILE__ << std::endl;

using namespace boost::filesystem;
using namespace std;

class Entry;

typedef boost::shared_ptr<Entry> EntryPtr;

class Entry {
	public:
	
	string title;
	string body;
	string path;
	time_t mtime;

	Entry(boost::filesystem::path p) : path(p.native_file_string()) {
		mtime = last_write_time(p);
		ifstream fin(path.c_str());

		getline(fin, title);

		while (!fin.eof()) {
			string line;
			getline(fin, line);
			body.append(line);
		}
	}

	~Entry() {
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


		cout << "Content-Type: text/plain\n";
		cout << "\n";

		for (unsigned int i = 0; i < entries.size(); i++) {
			EntryPtr entry = entries[i];
			DEBUG(entry->title);
			DEBUG(entry->mtime);
		}
	}

	void collect_entries (const string dir) {
		directory_iterator end_itr;
		for (directory_iterator itr(dir); itr != end_itr; itr++) {
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
	Cpposxom foo("data");
	foo.run();
}
