#include "bnb_MPI.cpp"
class TSP_Problem;

class TSP_Solution: public Solution{
	public:
		int cost;
		vector<int> visited_vertices; // list of visited vertices
		int cur_path_length;
		int get_cost();
		bool is_feasible();
		void print_sol();
		int get_bound();
		int to_str(char *buf);
};

class TSP_Problem: public Problem<TSP_Solution>{
	public:
    	vector<vector<int> > graph;
			int n; // num of vertices in the graph
			int min_edge;
			bool get_goal();
			TSP_Solution empty_sol();
			TSP_Solution worst_sol();
			vector<TSP_Solution> expand(TSP_Solution s);
			int to_str(char *buf);
			static TSP_Problem decode_prob(char *buf, int &cur_path_length);
			static TSP_Solution decode_sol(char *buf, int &cur_path_length);
};

int TSP_Solution::to_str(char *buf){
	int p = 0;
	p += sprintf(buf+p," %d %d ",cur_path_length,cost);
	for(int i=0;i<cur_path_length;i++)
		p += sprintf(buf+p," %d ",visited_vertices[i]);
	return p;
}

TSP_Solution TSP_Problem::decode_sol(char *buf, int &cur_path_length){
	TSP_Solution s;
	int b;
	sscanf(buf+cur_path_length,"%d%d%n",&(s.cur_path_length),&(s.cost),&b);
	cur_path_length += b;
	for(int i=0;i<s.cur_path_length;i++){
		int temp;
		sscanf(buf+cur_path_length,"%d%n",&temp,&b);
		cur_path_length += b;
		s.visited_vertices.push_back(temp);
	}
	return s;
}

int TSP_Problem::to_str(char *buf){
	int p = 0;
	p += sprintf(buf+p," %d %d ",n,min_edge);
	for(int i=0;i<n;i++)
		for(int j=0;j<n;j++)
			p += sprintf(buf+p," %d ",graph[i][j]);
	return p;
}

TSP_Problem TSP_Problem::decode_prob(char *buf, int &cur_path_length){
	TSP_Problem prob;
	int b;
	sscanf(buf+cur_path_length,"%d%d%n",&(prob.n),&(prob.min_edge),&b);
	cur_path_length += b;
	for(int i=0;i<prob.n;i++){
		vector<int> v;
		for(int j=0;j<prob.n;j++){
			int temp;
			sscanf(buf+cur_path_length,"%d%n",&temp,&b);
			cur_path_length += b;
			v.push_back(temp);
		}
		prob.graph.push_back(v);
	}
	return prob;
}

int TSP_Solution::get_cost(){
	return cost;
}

int TSP_Solution::get_bound(){
  // need to give a lower bound!
	int left = (((TSP_Problem *)problem_ptr)->n) - visited_vertices.size();
  return cost+(left*(((TSP_Problem *)problem_ptr)->min_edge));
  /*t bound = cost;
	for(int i=cur_path_length;i<((TSP_Problem *)prob_ptr)->n;i++){
		bound += ((TSP_Problem *)prob_ptr) -> costs[i];
	} // loose upper bound assuming all remaining items are visited_vertices
	return bound;*/
}


bool TSP_Solution::is_feasible(){
   //cout<<"checking feasibility"<<endl;
   return (cur_path_length == (((TSP_Problem *) problem_ptr) -> n));
   /*
   if(cur_path_length == (((TSP_Problem *)prob_ptr)->n))
   {
      bool flag[((TSP_Problem *)prob_ptr)->n];
      for(int i=0;i<((TSP_Problem *)prob_ptr)->n;i++)
        flag[i] = false;
      for(int i=0;i<visited_vertices.size();i++)
      {
        if(!flag[visited_vertices[i]])
          flag[visited_vertices[i]] = true;
        else return false;
      }
      return true;
   }
   return false;*/
}

void TSP_Solution::print_sol()
{
	cout << "Minimum cost of " << cost << " achieved for the path with vertices ";
	for (int i = 0; i < visited_vertices.size(); i++)
		cout << visited_vertices[i] << ", ";
	cout << endl;
}

bool TSP_Problem::get_goal()
{
	return false; // it is a minimization problem
}

TSP_Solution TSP_Problem::empty_sol()
{
	TSP_Solution s1;
	// include the starting vertex
	s1.cur_path_length = 0;
	s1.problem_ptr = this;
	s1.cost = 0;
	return s1;
}

TSP_Solution TSP_Problem::worst_sol()
{
	TSP_Solution s1;
	s1.problem_ptr = this;
	s1.cost = 100000000;
	s1.cur_path_length = 0;
	return s1;
}

vector<TSP_Solution> TSP_Problem::expand(TSP_Solution s){
  bool flag[((TSP_Problem*)s.problem_ptr)->n];
  vector<TSP_Solution> ret;
  memset(flag,false,sizeof(flag));
  for(int i=0;i<s.visited_vertices.size();i++)
  {   
    flag[s.visited_vertices[i]] = true;
  }
  if(s.visited_vertices.size()==0)
  {
     for(int i=0;i<((TSP_Problem *)s.problem_ptr)->n;i++)
     {
       TSP_Solution first = s;
       first.cur_path_length = 1;
       first.visited_vertices.push_back(i);
       ret.push_back(first);
     }
     return ret;
  }
  for(int i=0;i<((TSP_Problem *)s.problem_ptr)->n;i++)
  {
    if(!flag[i])
    {
	    TSP_Solution s1 = s;
      s1.cur_path_length++;
      s1.visited_vertices.push_back(i);
      int prev;
      prev = s1.visited_vertices[s1.visited_vertices.size()-2];
      //cout<<"cur_path_lengthition of the new q element"<<s1.cur_path_length<<endl;
      s1.cost += graph[prev][i];
      ret.push_back(s1);
      //return;
    }
      
  }
  return ret;
}

int main()
{
	TSP_Problem problem;
	BnB_solver<TSP_Problem, TSP_Solution> bnbs;
	cin >> problem.n;
	problem.min_edge = 1000000;
	for (int i = 0; i < problem.n; i++)
	{
		vector<int> temp;
		for (int j = 0; j < problem.n; j++)
		{
			int val;
			cin >> val;
			problem.min_edge = min(problem.min_edge, val);
			temp.push_back(val);
		}
		problem.graph.push_back(temp);
	}

	bnbs.solve(problem);
	return 0;
}
