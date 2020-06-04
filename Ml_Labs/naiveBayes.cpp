#include "iostream"
#include "vector"
#include "algorithm"
#include "unordered_map"

using namespace std;


int M,K;//M - feat; K = class
int N;//obj

struct Object{
	Object(int id, vector<int> f, int c):id(id),features(f) class(c){
	}
	int id;
	vector<int> features;
	int class;
}

vector<Object*> objs;

struct Node{
	int id;

	Node* left;
	Node* right;
	vector<Object*>  elems;

	bool isLeaf;
	int classId;

	double gini;
	pair<int,double> rule;

	Node(vector<Object> elements):elems(elements){
		gini = calcGini();
	}

	void calcGini(){
		double answer=0;
		unordered_map<int, int> counter;
		for (auto elem : elems)
			(counter.find(elem->class)==counter.end()	?counter[elem->class] = 1;
														: counter[elem->class]++;)

		int total = counter.size();
		for (const auto& [classId, amount] : counter)
			answer += ((double) amount / (double) total)*((double) amount / (double) total);
	
		answer = 1. - answer;
		return answer;
	}

	void setId(int id){
		id = id;
	}

	void splitNode(pair<int, double> rule){
		this.rule = rule;
		featId, featVal = rule.first, rule.second;

		vector<Object> l, r;
		auto iter = partition(elems.begin(), elems.end(),
										[](Object o){o.features[featId]<featVal})
		
		copy(elems.begin(), iter, l);
		copy(iter,elems.end(),r);

		left = new Node(l);
		right = new Node(r);
	}

}

pair<int,double> getBestFeature(vector<Object*> elems, double gini){
	double maxIG = (double)minValue;
	auto iter = elems[0]->features.begin();

	while (iter.h!=features.end()){
		
	}
	
}

enum splitNode {SPLIT, ONECLASS, NOCLASS, TOODEEP};
splitNode checkSplitNessesity(Node* node, int depth){
	if (depth == MAX_DEPTH)
		return TOODEEP;
	if (node->elems.size()==0)
		return NOCLASS;

	bool sameClassed = true;
	int checkClass = node->elems[0].class;
	for (const auto& elem : node->elems)
		sameClassed  = (elem.class == checkClass) && sameClassed;

	return (checkClass ? ONECLASS: SPLIT);
}

int nodes_amount = 0;
void buildDT(Node* node, int depth){
	node.setId(nodes_amount++);

	splitNode desision = checkSplitNessesity(node,depth);

	switch(desision){
		case SPLIT:
			pair<int,double> ruleInfo = getBestFeature(node->elems, node -> gini);
			node->splitNode(ruleInfo);
			buildDT(node->left,depth+1);
			buildDT(node->right,depth+1);
			break;
		case NOCLASS:
			node->classId = 1;
			break;
		case ONECLASS:
			node->classId = node->elems[0].class;
			break;
		case TOODEEP:
			unordered_map<int, int> counter;
			for (auto elem : node->elems)
				(counter.find(elem->class)==counter.end()	? counter[elem->class] = 1;
															: counter[elem->class]++;)
			node->classId = max_element(counter.begin(),counter.end());
			break;
	}
}

Node* buildDT(vector<Object*> elems){
	Node* root = new Node(elems);
	buildDT(root,2);
	return root;
}

void coutTree(Node* node){
	if (node-> isLeaf)
		cout<<"C "<<node->classId<<'\n';
	else{
		cout<<"Q "<<node->rule.first +1<<' '<<node->rule.second<<' ';
		cout<<node->left->id<<' '<<node->right->id<<'\n';
		coutTree(node->left);
		coutTree(node->right);
	}
}

int main(){
	
	cin>>M>>K>>N;

	for (int i=0; i<N; i++){
		int f,c;
		vector<int> features;
		for (int j=0; j<M; j++){
			cin>>f;
			features.push_back(f);
		}
		cin>>c;
		objs.push_back(new Object(i, features, c));
	}

	Node* root = buildDT(objs);
	cout<<nodes_amount<<'\n';
	coutTree(root);
	
	return 0;
}