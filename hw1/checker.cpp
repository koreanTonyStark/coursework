/* Advanced Computation Theory 
 * Assignment1  
 * Program Name : Checker
 * Registration Number : 2018-26716
 * Author : Jae-Hwan Jeong
 * E-Mail : jhjeong@rubis.snu.ac.kr
 */ 

#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <queue>
#include <fstream>


using namespace std;

#define ALPHABET_SIZE 26 

int answer_array[101][101];



void Prefix(string pattern, int* failure_func, int length)
{	
	//Do prefix computation 
	// f(1)=0
	// f(1) ~ f(q-1) already computed, compute f(q)
	// first k=f(q-1), if P[k+1]=P[q], f(q) = f(q-1)+1;
	//                 else k=f(k) repeat above loop
	// for convenience of array idx, start with pattern idx 0 
	// and q starts with 0
	failure_func[1]=0;
	int k=0;
	for(int q=2; q<length; ++q)
	{
		while(k>0 && pattern[k+1]!=pattern[q])
		{
			k=failure_func[k];
			
		}
		if( k>0 || pattern[k+1]==pattern[q]) ++k;
		failure_func[q]=k;
	}	
}

void KMP(string text, string pattern)
{
	int failure_func[pattern.size()];
	Prefix(pattern, failure_func, pattern.size());
	//printPrefix(pattern, failure_func, pattern.size());
	size_t q=0;
	size_t m=pattern.size()-1; // m = length of pattern 
	for(size_t i=1;i<text.size();++i)
	{
		while(q>0 && pattern[q+1]!=text[i])
			q=failure_func[q];
		if(q>0 || pattern[q+1]==text[i]) ++q;
		if(q==m)
		{	
			cout<<"occurrence at "<<i-m+1<<'\n';;
			q=failure_func[q];
		
		}
	
	}
}

/* state total num = number of characters with each patterns + start state(1)
 * 
 */
int computeNodeSize(string *patterns, size_t num_ptrns)
{
	int m=0;
	for(int i=1;i<=num_ptrns;++i)
		for(int j=1;j<patterns[i].size();++j)
			++m;
	
	return (m+1);

}


//build goto function 
int computeAutomata(string *patterns, size_t num_ptrns, int g[][ALPHABET_SIZE], vector<string> *output)
{
	//set '-1' denotes fail 
	//2D array implementation
	//alphabet size = {a ... z}
	//mapping a->0, b->1, ... z->25
	//output function -> vector<string> 
	
	//constructing rooted directed tree 
	int newstate=0;
	for(int i=1; i<=num_ptrns;++i)
	{
		int state=0;
		int j=1;
		string tmp = patterns[i];
		while(g[state][tmp[j]-97]!=-1)
		{
			state=g[state][tmp[j]-97];
			++j;
		}	
		int pattern_size=tmp.size();
		for(int p=j;p<pattern_size;++p)
		{
			++newstate;
			g[state][tmp[p]-97]=newstate;
			state=newstate;
		
		}
		output[state].push_back(patterns[i]);
	
	
	}
	for(int alphabet=0;alphabet<ALPHABET_SIZE;++alphabet)
	{
		if(g[0][alphabet]==-1)
			g[0][alphabet]=0;
		
	}
	return newstate;
}

int* computeFailureFunction(int g[][ALPHABET_SIZE], vector<string>* output, int node_size)
{
	int *failure_func = new int[node_size];
	queue<int> q;
	for(int i=0; i<ALPHABET_SIZE;++i)
	{
		if(g[0][i]!=0 && g[0][i]!=-1)
		{
			q.push(g[0][i]);
			failure_func[g[0][i]]=0;
		}
	}

	while(!q.empty())
	{
		int r=q.front();
		q.pop();
		for(int a=0;a<ALPHABET_SIZE;++a)
		{
			if(g[r][a]!=-1)
			{
				int s=g[r][a];
				q.push(s);
				int state=failure_func[r];
				while(g[state][a]==-1)
					state=failure_func[state];
				failure_func[s]=g[state][a];
				for(int i=0; i<output[failure_func[s]].size();++i)
					output[s].push_back(output[failure_func[s]][i]);
			}		
		}
		
	}

	return failure_func;
}

void AC(string* patterns, string text, int num_patterns)
{
	int node_size = computeNodeSize(patterns, num_patterns);
	int g[node_size][ALPHABET_SIZE];
	for(int i=0;i<node_size;i++)
		for(int j=0;j<ALPHABET_SIZE;j++)
			g[i][j]=-1;
	
	vector<string> output[node_size];			
	int num_state=computeAutomata(patterns, num_patterns, g, output)+1;
	int *failure_func=computeFailureFunction(g, output,num_state);

	//Text scanning using automaton 
	int state=0;
	for(int i=1;i<text.size();++i)
	{
		int a_i = text[i]-97;
		while(g[state][a_i]==-1)
			state=failure_func[state];
		state=g[state][a_i];
		if(!output[state].empty())
		{
			cout<<"occurrence at "<< i<<endl;
			for(int j=0;j<output[state].size();++j)
				cout<<output[state][j]<<" ";
			cout<<'\n';
		}
	}

// for Debug
	for(int i=0;i<num_state;++i)
	{
		cout<<i<<" "<<failure_func[i]<<'\n';
	}



	for(int i=0;i<num_state;++i)
	{
		if(output[i].size()!=0)
		{
			cout<<"state: "<<i<<'\n';
			for(int j=0;j<output[i].size();j++)
				cout<<output[i][j]<<' ';
			cout<<'\n';
		
		}	
	}
	
	for(int i=0;i<node_size;++i){
		for(int j=0;j<ALPHABET_SIZE;j++)
		{
			if(g[i][j]!=0&& g[i][j]!=-1)
				cout<<i<<" "<<(char)(j+97)<<" "<<g[i][j]<<'\n';
		}
		
	}
	
}


void BakerBird(string* text, string* patterns,int num_ptrns, int num_text, ofstream& osf)
{
//--------needed input list-----------------
	//h-table : with each rows 
	//failure function : f for multiple pattern
	//goto function : g
	//output function : output 
	
	int t=0;
	int h[num_ptrns+1];
	h[1]=0;
	for(int p=2; p<=num_ptrns;++p)
	{
		while(t>0 && patterns[p-1].compare(patterns[t])!=0)
		      t=h[t];
		t++;
		if( patterns[p].compare(patterns[t])!=0)
			h[p]=t;
		else
			h[p]=h[t];
	
	}
		
	int f[num_ptrns+1];
	f[1]=0;
	int k=0;
	for(int q=2; q<=num_ptrns;++q)
	{
		while(k>0 && patterns[k+1].compare(patterns[q])!=0)
			k=f[k];
		if(patterns[k+1].compare(patterns[q])==0)
			k++;
		f[q]=k;
	
	}


	
/* Debug 
	for(int i=1;i<=num_ptrns;++i)
		cout<<f[i]<<" ";
	cout<<'\n';

	for(int i=1;i<=num_ptrns;++i)
		cout<<h[i]<<" ";
	cout<<'\n';
*/

	int node_size=computeNodeSize(patterns, num_ptrns);
	int g[node_size][ALPHABET_SIZE];
	for(int i=0;i<node_size;++i)
		for(int j=0;j<ALPHABET_SIZE;++j)
			g[i][j]=-1;
	
	vector<string> output[node_size];
	int num_state=computeAutomata(patterns, num_ptrns,g,output)+1;
	int *failure_func=computeFailureFunction(g, output, num_state);
	int a[num_text+1];

	for(int i=1;i<=num_text;++i)
		a[i]=1;
	
	
	for(int row=1; row<=num_text;++row)
	{
		int state=0;
		for(int column=1; column<=num_text;++column)
		{
			while(g[state][text[row][column]-'a']==-1)
				state=failure_func[state];
			state=g[state][text[row][column]-'a'];
			if(!output[state].empty()) // row matching step 
			{
				//interleave column matching step 
				int k=a[column];
				while(true)
				{
					bool flag=true;
					if(k<=0)
						flag=false;
					for(int i=0; i<output[state].size();++i)
					{
						if(patterns[k].compare(output[state][i])==0)
							flag=false;
					}
					if(flag==false)
						break;
					k=h[k];
				}	
				a[column]=k+1;
				if(k==num_ptrns)
				{
					//cout<<"found at " << row-1 <<" "<<column-1<<'\n';
					//osf<<row-1<<" "<<column-1<<'\n';
					//use row-1 , column-1
					a[column]=f[k]+1;
					answer_array[row-1][column-1]=1;
				}
			}
			else
				a[column]=1;
		}	
	}


/*	
	for(int i=0;i<num_state;++i)
		cout<<i<<" "<<failure_func[i]<<'\n';
	for(int i=0;i<num_state;++i)
	{
					cout<<"found at" << row-num_ptrns<<" "<<column-num_ptrns<<'\n';
		if(output[i].size()!=0)
		{
			cout<<"state: "<<i<<'\n';
			for(int j=0; j<output[i].size();++j)
				cout<<output[i][j]<<' ';
			cout<<'\n';
		}
	}


	
	for(int i=0;i<node_size;++i)
	{
		for(int j=0;j<ALPHABET_SIZE;++j)
		{
			if(g[i][j]!=0 && g[i][j]!=-1)
				cout<<i<<" "<<(char)(j+97)<<" "<<g[i][j]<<'\n';
		}
	}
*/


}



int main(int argc, char *argv[])
{

	if(argc!=4)
	{
		cout<<"usage : ./hw1 [input] [output_for_check] [check_result]\n";
		exit(1);
	
	}

	ifstream fp_read;
	fp_read.open(argv[1]);
	if(!fp_read.is_open())
	{
		cerr<<"Can't open file for read\n";
		exit(1);
	}

	ifstream check_output;
	check_output.open(argv[2]);
	if(!check_output.is_open())
	{
		cerr<<"Can't open file for check_output\n";
		exit(1);
	}

	ofstream fp_write;
	fp_write.open(argv[3]);
	if(!fp_write.is_open())
	{
		cerr<<"Can't open file for write\n";
		exit(1);
	}

	int num1;
	int num2;
	
	fp_read>>num1>>num2;
	
	//cout<<num1<<" "<<num2<<endl;
	size_t num_ptrns=(size_t)num1;
	size_t num_text =(size_t)num2;

	string *text =  new string[num_text+1];
	string *ptrns = new string[num_ptrns+1];




	for(int i=1;i<=num_ptrns;++i)
	{
		fp_read>>ptrns[i];
		ptrns[i]=" "+ptrns[i];
		//cout<<ptrns[i].size()<<endl;
	}
	
	for(int i=1;i<=num_text;++i)
	{
		fp_read>>text[i];
		text[i]=" "+text[i];
		//cout<<text[i].size()<<endl;
	}
	fp_read.close();	

	//AC(ptrns, text, num_ptrns);
	BakerBird(text, ptrns, num_ptrns, num_text, fp_write);	
	

	int check_array[num_text][num_text];

	for(int i=0;i<num_text;++i)
		for(int j=0;j<num_text;++j)
			check_array[i][j]=0;

	while(true)
	{
		int row,column;
		check_output>>row>>column;
		check_array[row][column]=1;

		if(check_output.eof()) break;
	}

	check_output.close();
	bool flag=true;

	for(int i=0;i<num_text;++i)
	{
		for(int j=0;j<num_text;++j)
		{
			if(answer_array[i][j]!=check_array[i][j])
			{
				flag=false;
				break;
			}
		}
	}

	if(flag==true)
		fp_write<<"yes\n";
	else
		fp_write<<"no\n";




	fp_write.close();

	return 0;


}





/* unnecessary function - only in order to study index changes - my own
 *
 *
 *
 *
 *
 *
 */
void Prefix2(char* pattern, int* failure_func, int length)
{

	failure_func[0]=0;
	int k=0;
	for(int q=1;q<length;++q)
	{
		while(k>0 && pattern[k]!=pattern[q])
			k=failure_func[k];
		if(k>0 || pattern[k]==pattern[q]) ++k;
		failure_func[q]=k;
	
	}


}

void printPrefix2(char *pattern, int *fail, int length)
{
	for(int i=0; i<length;++i)
		cout<<pattern[i]<<" ";
	cout<<'\n';
	for(int i=0;i<length;++i)
		cout<<fail[i]<<" ";
	cout<<'\n';
		
	
}


void printInputInfo(string text, string ptrn)
{	
	cout<<"---------------------"<<endl;
	cout<<"text   :"<<text<<'\n';
	cout<<"pattern:"<<ptrn<<'\n';
}


void printPrefix(string pattern, int *fail, int length)
{
	cout<<"------------------------"<<'\n';
	cout<<"failure function"<<'\n';
	for(int i=1; i<pattern.size();++i)
		cout<<pattern[i]<<" ";
	cout<<'\n';
	for(int i=1; i<length;++i)
		cout<<fail[i]<<" ";
	cout<<'\n';
	cout<<"-------------------------"<<'\n';
}

