#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <utility>
#include <fstream>
#include <iterator>
#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

struct string_data
{
	string str;
	uint32_t offset;
};

optional<pair<uint32_t, string>> get_chars_amount(int argc, char *argv[]);
void get_strings(vector<string_data>& vec, uint32_t chars_amount, string filename);
string get_string(const string& str, uint32_t& i, uint32_t chars_amount);
bool is_readable_char(const char ch);

int main(int argc, char* argv[])
{		
	setlocale(0, "rus");

	auto chars_amount = get_chars_amount(argc, argv);
	if (chars_amount.has_value())
	{
		vector<string_data> strings;
		int delim = chars_amount.value().second.find("\noffset");
		bool print_offset = (delim != string::npos ? true : false);
		string filename = chars_amount.value().second.substr(0, delim);

		get_strings(strings, chars_amount.value().first, filename);

		if (print_offset)
		{
			for (const auto& i : strings)
			{
				cout << i.offset << ": " << i.str << '\n';
			}
		}
		else
		{
			for (const auto& i : strings)
			{
				cout << i.str << '\n';
			}
		}
	}

	return 0;
}

optional<pair<uint32_t, string>> get_chars_amount(int argc, char* argv[])
{
	pair<uint32_t, string> data_pair;
	po::options_description desc("Available commands");
	desc.add_options()
		("help", "Prints commands")
		("chars", po::value<uint32_t>(&data_pair.first)->default_value(12),
			"Select minimum amount of characters that should contains in the string")
		("file", po::value<string>(&data_pair.second), "Choose filename")
		("offset", "Print nummber of byte");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		cout << desc << "\n";
		return { };
	}

	if (vm.count("chars"))
	{
		data_pair.first = vm["chars"].as<uint32_t>();
	}

	if (vm.count("file"))
	{
		data_pair.second = vm["file"].as<string>();
	}

	if (vm.count("offset"))
	{
		data_pair.second += "\noffset";
	}

	return ((data_pair.first == 0 || data_pair.second == "") 
		? optional<pair<uint32_t, string>>() 
		: optional<pair<uint32_t, string>>(data_pair));
}

void get_strings(vector<string_data>& vec,  uint32_t chars_amount, string filename)
{
	ifstream iff(filename, ios::binary);
	string data;
	copy(istream_iterator<char>(iff), istream_iterator<char>(), back_inserter(data));	
	iff.close();

	for (uint32_t i = 0; i < data.length(); ++i)
	{
		if (is_readable_char(data[i]) && i + chars_amount < data.length())
		{
			uint32_t off = i;
			string str = get_string(data, i, chars_amount);

			if (str.length() >= chars_amount)
			{
				vec.push_back({ str, off });
			}
		}
	}
}

string get_string(const string& str, uint32_t &i, uint32_t chars_amount)
{
	string result;
	for (;; ++i)
	{
		if (is_readable_char(str[i]))
		{
			result += str[i];
		}
		else
		{
			break;
		}
	}
	return result;
}

bool is_readable_char(const char ch)
{
	int v = static_cast<int>(ch);
	if (!(v > 32 && v < 127))
	{
		return false;
	}	
	return true;
}