

#include "ParseArguments.hpp"

using namespace std;

//
// Parse functions
//

int parseInt(char ** c, const int a, int * i)
{
	int j = (*i)+1;
	// Failure
	if (j >= a || c[j][0] == '-')
		return 0;
	
	++(*i);
	return atoi(c[j]);
}

int parseInt(wchar_t ** c, const int a, int * i)
{
	int j = (*i)+1;
	// Failure
	if (j >= a || c[j][0] == L'-')
		return 0;
	
	++(*i);
	return _wtoi(c[j]);
}

string parseString(char ** c, const int a, int * i)
{
	string str;
	int t = (*i)+1;
	int j = t;
	while (j < a && c[j][0] != '-')
	{
		if (j == t)
			str = c[j];
		else
			str = str + " " + c[j];
		
		++j;
	}
	// Failure
	if ((j >= a || c[j][0] == '-') && (*i) == j)
		return "";
	
	(*i) = j - 1;
	return str;
}

string parseString(wchar_t ** c, const int a, int * i)
{
	wstring str;
	int t = (*i)+1;
	int j = t;
	while (j < a && c[j][0] != L'-')
	{
		if (j == t)
			str = c[j];
		else
			str = str + L" " + c[j];
		
		++j;
	}
	// Failure
	if ((j >= a || c[j][0] == '-') && (*i) == j)
		return "";
	
	(*i) = j - 1;
	string str2;
	str2.assign(str.begin(), str.end());
	return str2;
}

wstring parseWString(char ** c, const int a, int * i)
{
	string str;
	int t = (*i)+1;
	int j = t;
	while (j < a && c[j][0] != '-')
	{
		if (j == t)
			str = c[j];
		else
			str = str + " " + c[j];
		
		++j;
	}
	// Failure
	if ((j >= a || c[j][0] == '-') && (*i) == j)
		return L"";
	
	(*i) = j - 1;
	wstring str2;
	str2.assign(str.begin(), str.end());
	return str2;
}

wstring parseWString(wchar_t ** c, const int a, int * i)
{
	wstring str;
	int t = (*i)+1;
	int j = t;
	while (j < a && c[j][0] != '-')
	{
		if (j == t)
			str = c[j];
		else
			str = str + L" " + c[j];
		
		++j;
	}
	// Failure
	if ((j >= a || c[j][0] == '-') && (*i) == j)
		return L"";
	
	(*i) = j - 1;
	return str;
}

list<int> parseIntList(char ** c, const int a, int * i, Parameter * p)
{
	list<int> l;
	int t = (*i)+1;
	int j = t;
	while (j < a && c[j][0] != '-')
	{
		int f = 0;
		bool d = false;
		while ((f = p->eStr.find_first_of(c[j][0], f)) >= 0)
		{
			++f;
			l.push_back(p->eStr[f]);
			d = true;
		}
		if (d)
		if (d)
		{
			++j;
			continue;
		}
		int t = atoi(c[j]);
		l.push_back(t);
		++j;
	}
	// Failure
	if ((j >= a || c[j][0] == '-') && t == j)
		return list<int>();
	
	(*i) = j - 1;
	return l;
}

list<int> parseIntList(wchar_t ** c, const int a, int * i, Parameter * p)
{
	list<int> l;
	int t = (*i)+1;
	int j = t;
	while (j < a && c[j][0] != '-')
	{
		int f = 0;
		bool d = false;
		while ((f = p->eWstr.find_first_of(c[j][0], f)) >= 0)
		{
			++f;
			l.push_back(p->eWstr[f]);
			d = true;
		}
		if (d)
		{
			++j;
			continue;
		}
		int t = _wtoi(c[j]);
		l.push_back(t);
		++j;
	}
	// Failure
	if ((j >= a || c[j][0] == '-') && t == j)
		return list<int>();
	
	(*i) = j - 1;
	return l;
}

/*list<string> parseStringList(const char ** c, const int a, const int i)
{
}

list<string> parseStringList(const wchar_t ** c, const int a, const int i)
{
}

list<wstring> parseWStringList(const char ** c, const int a, const int i)
{
}

list<wstring> parseWStringList(const wchar_t ** c, const int a, const int i)
{
}*/

//
// Parameter
//

#include <sstream>

Parameter::Parameter()
	: boolean(false), integer(PARAM_DEFAULT), exist(false)
{
	memset(&name, 0, sizeof(SCHAR));
}

Parameter::Parameter(const Parameter & a)
{
	memcpy(this, &a, sizeof(Parameter));
}

Parameter & Parameter::operator=(const Parameter & a)
{
	memcpy(this, &a, sizeof(Parameter));
	return *this;
}
Parameter & Parameter::operator=(const bool & a)
{
	nlist.clear();
	boolean = a;
	integer = a;
	nlist.push_back(a);
	stringstream s;
	s << a;
	str = s.str();
	wstringstream ws;
	ws << a;
	wstr = ws.str();
	exist = true;
	return *this;
}
Parameter & Parameter::operator=(const int & a)
{
	nlist.clear();
	integer = a;
	boolean = !!a;
	nlist.push_back(a);
	stringstream s;
	s << a;
	str = s.str();
	wstringstream ws;
	ws << a;
	wstr = ws.str();
	exist = true;
	return *this;
}
Parameter & Parameter::operator=(const string & a)
{
	nlist.clear();
	str = a;
	integer = atoi(a.c_str());
	boolean = !!atoi(a.c_str());
	wstr.assign(a.begin(), a.end());
	nlist.push_back(integer);
	exist = true;
	return *this;
}
Parameter & Parameter::operator=(const wstring & a)
{
	nlist.clear();
	wstr = a;
	integer = _wtoi(a.c_str());
	boolean = !!_wtoi(a.c_str());
	str.assign(a.begin(), a.end());
	nlist.push_back(integer);
	exist = true;
	return *this;
}
Parameter & Parameter::operator=(const list<int> & a)
{
	nlist = a;
	exist = true;
	return *this;
}
Parameter & Parameter::operator=(const list<string> & a)
{
	nlist.clear();
	slist = a;
	exist = true;
	return *this;
}
Parameter & Parameter::operator=(const list<wstring> a)
{
	nlist.clear();
	wlist = a;
	exist = true;
	return *this;
}
bool Parameter::Get(bool * a)
{
	*a = boolean;
	return exist;
}
bool Parameter::Get(int * a)
{
	*a = integer;
	return exist;
}
bool Parameter::Get(string * a)
{
	*a = str;
	return exist;
}
bool Parameter::Get(wstring * a)
{
	*a = wstr;
	return exist;
}
bool Parameter::Get(list<int> * a)
{
	*a = nlist;
	return exist;
}
bool Parameter::Get(list<string> * a)
{
	*a = slist;
	return exist;
}
bool Parameter::Get(list<wstring> * a)
{
	*a = wlist;
	return exist;
}

//
// ParseArguments
//


ParseArguments::~ParseArguments()
{
	while (!params.empty())
	{
		Parameter * t = params.back();
		params.pop_back();
		delete t;
	}
	while (!aparams.empty())
	{
		Parameter * t = aparams.back();
		aparams.pop_back();
		delete t;
	}
}

unsigned int ParseArguments::AddParameter(char p, int type, const char * l)
{
	unsigned int s = params.size();
	Parameter * param = new Parameter();
	param->name.c = p;
	param->type = type;
	if (l)
		param->eStr = l;
	params.push_back(param);
	return s;
}

unsigned int ParseArguments::AddParameter(wchar_t p, int type, const wchar_t * l)
{
	unsigned int s = params.size();
	Parameter * param = new Parameter();
	param->name.wc = p;
	param->type = type;
	if (l)
		param->eWstr = l;
	params.push_back(param);
	return s;
}

unsigned int ParseArguments::AddParameter(const char * p, int type, const char * l)
{
	unsigned int s = aparams.size();
	Parameter * param = new Parameter();
	param->name.s = p;
	param->type = type;
	if (l)
		param->eStr = l;
	aparams.push_back(param);
	return s;
}

unsigned int ParseArguments::AddParameter(const wchar_t * p, int type, const wchar_t * l)
{
	unsigned int s = aparams.size();
	Parameter * param = new Parameter();
	param->name.ws = p;
	param->type = type;
	if (l)
		param->eWstr = l;
	aparams.push_back(param);
	return s;
}

Parameter * ParseArguments::GetParameter(unsigned int i)
{
	if (i >= params.size())
		return 0;
	return params[i];
}

Parameter * ParseArguments::GetAdvancedParameter(unsigned int i)
{
	if (i >= aparams.size())
		return 0;
	return aparams[i];
}

void ParseArguments::PushArguments(char ** args, const int argc)
{
	for (int i = 0; i < argc; ++i)
	{
		char * cmd = args[i];
		// Normal command
		if (cmd[0] == '-')
		{
			// Advanced command
			if (cmd[1] == '-')
			{
				for (vector<Parameter *>::iterator j = aparams.begin(); j != aparams.end(); ++j)
				{
					Parameter * t = (*j);
					unsigned int size = strlen(t->name.s);
					if (strlen(cmd + 2) == size && memcmp(t->name.s, cmd + 2, size + 1) == 0)
					{
						switch (t->type)
						{
							case PARAM_BOOL:
								(*t) = true;
								break;
							case PARAM_INT:
								(*t) = parseInt(args, argc, &i);
								break;
							case PARAM_STRING:
								(*t) = parseString(args, argc, &i);
								break;
							case PARAM_WSTRING:
								(*t) = parseWString(args, argc, &i);
								break;
							case PARAM_INT | PARAM_LIST:
								(*t) = parseIntList(args, argc, &i, t);
								break;
						}
						break;
					}
				}
			}
			else
			{
				for (vector<Parameter *>::iterator j = params.begin(); j != params.end(); ++j)
				{
					Parameter * t = (*j);
					if (t->name.c == cmd[1])
					{
						switch (t->type)
						{
							case PARAM_BOOL:
								(*t) = true;
								break;
							case PARAM_INT:
								(*t) = parseInt(args, argc, &i);
								break;
							case PARAM_STRING:
								(*t) = parseString(args, argc, &i);
								break;
							case PARAM_WSTRING:
								(*t) = parseWString(args, argc, &i);
								break;
							case PARAM_INT | PARAM_LIST:
								(*t) = parseIntList(args, argc, &i, t);
								break;
						}
						break;
					}
				}
			}
		}
	}
}

void ParseArguments::PushArguments(wchar_t ** args, const int argc)
{
	for (int i = 0; i < argc; ++i)
	{
		wchar_t * cmd = args[i];
		// Normal command
		if (cmd[0] == '-')
		{
			// Advanced command
			if (cmd[1] == '-')
			{
				for (vector<Parameter *>::iterator j = aparams.begin(); j != aparams.end(); ++j)
				{
					Parameter * t = (*j);
					unsigned int size = wcslen(t->name.ws);
					if (wcslen(cmd + 2) == size && wmemcmp(t->name.ws, cmd + 2, size + 1) == 0)
					{
						switch (t->type)
						{
							case PARAM_BOOL:
								(*t) = true;
								break;
							case PARAM_INT:
								(*t) = parseInt(args, argc, &i);
								break;
							case PARAM_STRING:
								(*t) = parseString(args, argc, &i);
								break;
							case PARAM_WSTRING:
								(*t) = parseWString(args, argc, &i);
								break;
							case PARAM_INT | PARAM_LIST:
								(*t) = parseIntList(args, argc, &i, t);
								break;
						}
						break;
					}
				}
			}
			else
			{
				for (vector<Parameter *>::iterator j = params.begin(); j != params.end(); ++j)
				{
					Parameter * t = (*j);
					if (t->name.wc == cmd[1])
					{
						switch (t->type)
						{
							case PARAM_BOOL:
								(*t) = true;
								break;
							case PARAM_INT:
								(*t) = parseInt(args, argc, &i);
								break;
							case PARAM_STRING:
								(*t) = parseString(args, argc, &i);
								break;
							case PARAM_WSTRING:
								(*t) = parseWString(args, argc, &i);
								break;
							case PARAM_INT | PARAM_LIST:
								(*t) = parseIntList(args, argc, &i, t);
								break;
						}
						break;
					}
				}
			}
		}
	}
}