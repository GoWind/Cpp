#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>
#include<algorithm>
#include<cstring>
#include<map>
/* This is an implementation of the Backward Chaining Algorithm 
 * in Artificial Intelligence
 * The input format is given in the file backchain_input.txt
 */
using namespace std;

bool backchain(vector<string>& facts, vector<string>& clauses, string ask);
map<string, string> mapVarToConst(string, string);
void trim(string&);

void getImplication(string clause, string& ot) {
		int index = clause.find("=>");
		if(index == string::npos) {
				/* error */
				ot =  "";
		}
		string t = clause.substr(index+2);
		trim(t);
		ot = t;
}

vector<string> split(string input, char delim) {
		string buf = "";
		vector<string> splits;
		for(int i = 0; i < input.size();i++) {
				if(input[i] == delim) {
							splits.push_back(buf);
							buf = "";
				} else {
						buf += input[i];
				}
		}
		if( buf.length() > 0 ) 
				splits.push_back(buf);
		return splits;
}



/* String with open and closing braces */
vector<string> getParams(string line){
	int openB, closeB;
	vector<string> v;
	openB = line.find('(');
	closeB = line.find(')');
	vector<string> params;
	string paramsString = line.substr(openB+1, closeB-openB-1);
	/* split by ',' here */
	params = split(paramsString, ',');
	return params;
}
		


vector<string> getFacts(const vector<string>& kb) {
			vector<string> temp;
			for(int i = 0; i < kb.size(); i++ ) {
					int n = kb[i].find("=>");
					if( n == string::npos) {
							temp.push_back(kb[i]);
					} else {
							continue;
					}
			}
			return temp;
}

vector<string> getClauses(const vector<string>& kb) {
			vector<string> temp;
			for(int i = 0; i < kb.size(); i++ ) {
					int n = kb[i].find("=>");
					if( n != string::npos) {
							temp.push_back(kb[i]);
					} else {
							continue;
					}
			}
			return temp;
}
/* Trim space before and after 
 * Do not use it for lines input via cin.
 * */

void trim(string& s) {
		string::size_type first = s.find_first_not_of(' ');
		string::size_type last = s.find_last_not_of(' ');
		if( (first == string::npos) && (last == string::npos) )
				return;
		if( first == string::npos)
				first = 0;
		if( last == string::npos)
				last = s.length()-1;
		s = s.substr(first, last-first+1);
}

template<class T>
void print(vector<T> v) {
		typename vector<T>::iterator i;
		for(i = v.begin(); i != v.end();i++) {
					cout << *i << ":"<< (*i).size() <<endl;
		}
}

string getPredicateName(string predicate) {
	int endingIndex = predicate.find('(');
	return predicate.substr(0, endingIndex);
}

map<string, string> mapVarToConst(string ask, string predicate) {
		vector<string> formalp = getParams(predicate);
		vector<string> actualp = getParams(ask);
		map<string, string> m;
		for(int i=0;i< formalp.size();i++) {
				if( formalp[i].size() == 1 && islower(formalp[i][0]) )
						m[formalp[i]] = actualp[i];
				else
						m[formalp[i]] = formalp[i];
		}
		return m;
}

bool compare(string clause, string predicate) {
		string implication;
		getImplication(clause, implication);
		if(getPredicateName(predicate) != getPredicateName(implication)) {
				return false;
		}
		/* compare Params here */
		vector<string> formalParams = getParams(implication);
		vector<string> actualParams = getParams(predicate);
		/* compare them */
		if(formalParams.size() != actualParams.size()) {
				return false;
		}
		for(int i =0 ; i < formalParams.size();i++) {
				if(formalParams[i].size() == 1 && islower(formalParams[i][0])) {
						//variable , so ignore
						continue;
				} else {
						if(formalParams[i] != actualParams[i]) {
								return false;
						}else {
								continue;
						}
				}
		}
		return true;
}
		
vector<string> getPredicates(string clause) {
		int index = clause.find("=>");
		string s = clause.substr(0,index);
		return split(s, '&');
}
bool otherThanX(string predicate) { 
		vector<string> params = getParams(predicate);
		for(int i = 0 ; i < params.size();i++) {
				if(params[i].size() == 1 && islower(params[i][0])) 
						if(params[i][0] != 'x')
							return true;
		}
		return false;
}
using namespace std;

int main(int argc, char** argv) {
	
	ifstream f("input.txt");
	ofstream fo("output.txt");

	if ( !f ) {
		cout << "cannot open file for reading" << endl;
		return 1;
	}
	if ( !fo ) {
		cout << "cannot open file for writing" << endl;
		return 1;
	}
	string to_check, num_lines;
	getline(f, to_check);
	if( to_check =="") {
			fo << "Non-existent Query" << endl;
			fo.close();
			f.close();
	}
	vector<string> kb;
	vector<string> facts, clauses;
	trim(to_check);
	int rules;
	// get number
	getline(f, num_lines);
	if(num_lines == "") {
				fo << "invalid num rules";
				fo.close();
				f.close();
				return 1;
	}
	istringstream iss(num_lines);
	if( !(iss >> rules) || rules > 10 ) { 
				fo << "improper number";
				fo.close();
				f.close();
				return 1;
	}
	for(int i = 0 ; i < rules ; i++ ) {
			string s;
			if(!getline(f, s)) {
					fo << "File terminated before clauses are read"<< endl;
					fo.close();
					return 1;
			}
			kb.push_back(s);
	}
	

	getFacts(kb).swap(facts);
	getClauses(kb).swap(clauses);
	for(int i = 0 ; i < clauses.size(); i++) {
			trim(clauses[i]);
	}
	for(int j = 0; j < facts.size(); j++)
			trim(facts[j]);
	/* Check if facts/clauses contain vars other than x
	 */

	for(int p = 0 ; p < facts.size();p++) {
		if(otherThanX(facts[p])) {
					fo << "Variable other than x found. "<<endl;
					fo << "FALSE"<<endl;
					goto Final;				
		}
	}
	/* check clauses
	 */
	for(int k = 0 ; k < clauses.size();k++) {
		string s;
		getImplication(clauses[k], s);
		vector<string> predicates = getPredicates(clauses[k]);
		if(otherThanX(s)) { 
				fo << "Variable other than x found in implication. Bing Bing error"<<endl;
				fo << "FALSE" << endl;
				goto Final;
		}
		for(int pr = 0 ; pr < predicates.size();pr++) {
			if(otherThanX(predicates[pr])){
				fo << "Variable other than x found in predicates. Bing Bing error"<<endl;
				fo << "FALSE" << endl;
				goto Final;
			}
		}
	}
	if(backchain(facts, clauses, to_check)) 
			fo <<"TRUE";
	else
			fo <<"FALSE";
	
	Final:
		f.close();
		fo.close();
	
	return 0;
}

void mapvalues(map<string, string> m, string& predic) {
	vector<string> params = getParams(predic);
	string t;
	t = getPredicateName(predic)+'(';
	for(int i = 0 ; i < params.size(); i++) {
			if(m.find(params[i]) != m.end() ) {
				t  += m[params[i]];
				t += ',';
			} else {
				t += params[i];
				t += ',';
			}
	}
	t[t.size()-1] =')';
	
	predic = t;
}

bool backchain(vector<string>& facts, vector<string>& clauses, string ask) {
		vector<string> agenda, entailed;
		agenda.push_back(ask);
		while(!agenda.empty()) {
				string q = agenda.front();
				agenda.erase(agenda.begin());
				entailed.push_back(q);

				if(find(facts.begin(), facts.end(), q) == facts.end() ) { 
					vector<string> predics;
					for(int i = 0 ; i < clauses.size();i++) {
						if(compare(clauses[i], q)) {
							vector<string> temp = getPredicates(clauses[i]);
							/* now map them to actual params how ? */
							string implication;
							getImplication(clauses[i], implication);
							map<string, string> s = mapVarToConst(q, implication);
							for(map<string, string>::iterator it = s.begin(); it != s.end();it++)

							for(int j = 0 ; j < temp.size(); j++) {
							   mapvalues(s, temp[j]);	
							}
							
							predics.insert(predics.end(), temp.begin(), temp.end());
											
						}
					}
					if(predics.size() == 0) {
						/* no element to be inserted. therefroe bc fails */
						return false;
					} else {
							for(int k = 0; k < predics.size();k++) 
								if(find(entailed.begin(), entailed.end(), predics[k]) == entailed.end())
										agenda.push_back(predics[k]);
								predics.clear();
							
					}
				
				} else {
				}

	}
	return true;
}
