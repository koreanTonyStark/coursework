/* @ Advanced Theory of Computation
 * @ Program Name : hw1 (2D pattern matching algorithm)
 * @ Author : Jae-Hwan Jeong
 * @ Registration Number : 2018-26716
 * @ E-Mail : jhjeong@rubis.snu.ac.kr
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

/* function : Prefix 
 * Input : 1)pattern : use each row for one symbol(character), and compute
 *         longest proper suffix and it satisfies prefix. 
 *	   2)failure_func : unprocessed failure_function table 
 *	   3)length : length of failure_function table
 * 
 * Execution 
 * : Given pattern,
 * in example r1="aabba", r2="aaabb", r3="ababa", r4="aabba", r5="aaabb"
 * treat them as one symbol, and compute f table with "12312"
 * 
 * Use for changing 1D vector table's value when if match occurs
 */
void Prefix(string *patterns, int* failure_func, int num_ptrns)
{	
	failure_func[1]=0;
	int k=0;
	for(int q=2; q<=num_ptrns; ++q)
	{
		while(k>0 && patterns[k+1].compare(patterns[q])!=0)
		{
			k=failure_func[k];
			
		}
		if(patterns[k+1].compare(patterns[q])==0) ++k;
		failure_func[q]=k;
	}	
}

/* Function : Next 
 * Input : pattern set
 * Ouput : next function table 
 * Description ()
 *  'next[p]=s' denotes maximum number 0 or 
 *                                     P[1..p-1]=P[p-s+1...p-1] and P[p]!=P[s]
 * |======|O.......|======|X (X's idx is p)
 *                 |-------|        
 *                     s
 */
void Next(string* patterns, int *h_func, int num_ptrns)
{
	int t=0;
	h_func[1]=0;
	for(int p=2;p<=num_ptrns;++p)
	{
		while(t>0 && patterns[p-1].compare(patterns[t])!=0)
			t=h_func[t];
		t++;
		if(patterns[p].compare(patterns[t])!=0)
			h_func[p]=t;
		else
			h_func[p]=h_func[t];
	}
}

/* function : computeNodeSize
 * input : pattern set
 * output : number of nodes for computing goto function 
 * Description : for computing goto function with sigma X m X m 
 *               we need m (Note that m denotes whole # of  pattern's characters 
 * state total num = number of characters with each patterns + start state(1)
 */
int computeNodeSize(string *patterns, size_t num_ptrns)
{
	int m=0;
	for(int i=1;i<=num_ptrns;++i)
		for(int j=1;j<patterns[i].size();++j)
			++m;
	
	return (m+1);

}


/* Function : computeAutomata
 * input : pattern set, goto function(not assigned), output func(not assigned)
 * output : goto function, output function (not finally modified)
 * Description : Constructing AC Automaton. 
 */
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
		int pattern_size=tmp.size();
		while(g[state][tmp[j]-'a']!=-1 && j<pattern_size)
		{
			state=g[state][tmp[j]-'a'];
			++j;
		}	
		for(int p=j;p<pattern_size;++p)
		{
			++newstate;
			g[state][tmp[p]-'a']=newstate;
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

/* Function : computeFailureFunction 
 * Input : goto function, output function (not completed)
 * Output :  failure_function, output function (completed)
 * Description : for multiple pattern matching problem, have to compute the 
 * f(state) for each state except 0
 * Used data structure queue for induction 
 */
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


/* Baker-Bird Algorithm 
 * Preprocessing 
 *  - 1) f-table : same as KMP, but treat one row as one symbol. 
 *  - 2) next-table : same as KMP, but treat one row as one symbol.  
 *  - 3) Automata : AC Automata: goto function, output function, failure function
 *  - 4) 1D vector table : initialize all elements with 1.
 * Algorithm 
 *  - We have to maintain only O(n) vector table for satisfy the HW constraint 
 *  - Then, we need to make algorithm interleaving column matching and row
 *  matching. 1D vector table denotes array a, a[i] means with each row matching
 *  step, a[i] denotes R[1]...R[a[i]-1] matches with that row and column site.  
 *  So if it matches, then we have to assign a[i] with longest proper suffix,
 *  and prefix. if we mismatches with no reaching m. we have to assign the
 *  sliding size of that pattern(h denotes that value) 
 */
void BakerBird(string* text, string* patterns,int num_ptrns, int num_text, ofstream& osf)
{
//--------------------Preprocessing---------------------------------------------
	int f[num_ptrns+1];
	Prefix(patterns, f, num_ptrns);

	int h[num_ptrns+1];
	Next(patterns, h, num_ptrns);

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
	
//------------------------Algorithm--------------------------------------------
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
					osf<<row-1<<" "<<column-1<<'\n';
					a[column]=f[k]+1;
				}
			}
			else
				a[column]=1;
		}	
	}

}


/* Function : main
 * Description : File IO, execute BakerBird function 
 */
int main(int argc, char *argv[])
{

	if(argc!=3)
	{
		cout<<"usage : ./hw1 [input] [output]\n";
		exit(1);
	
	}

	ifstream fp_read;
	fp_read.open(argv[1]);
	if(!fp_read.is_open())
	{
		cerr<<"Can't open file for read\n";
		exit(1);
	}

	ofstream fp_write;
	fp_write.open(argv[2]);
	if(!fp_write.is_open())
	{
		cerr<<"Can't open file for write\n";
		exit(1);
	}

	int num1;
	int num2;
	
	fp_read>>num1>>num2;
	
	
	size_t num_ptrns=(size_t)num1;
	size_t num_text =(size_t)num2;

	string *text =  new string[num_text+1];
	string *ptrns = new string[num_ptrns+1];


	for(int i=1;i<=num_ptrns;++i)
	{
		fp_read>>ptrns[i];
		ptrns[i]=" "+ptrns[i];
	}
	
	for(int i=1;i<=num_text;++i)
	{
		fp_read>>text[i];
		text[i]=" "+text[i];
	}
	fp_read.close();	

	BakerBird(text, ptrns, num_ptrns, num_text, fp_write);	

	fp_write.close();

	return 0;


}

