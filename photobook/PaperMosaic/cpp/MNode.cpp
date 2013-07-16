#import "MNode.h"

MNode::MNode(void)
{
	data = NULL;
}

MNode::~MNode(void)
{
	Destroy();
}

void MNode::Destroy(){
	// FIXME: remove void *
	if(data)
		delete data;
	data = NULL;
	int ChildSize;
	ChildSize = child.size();
	for(int i = 0; i < ChildSize; i++){
		if(child[i]){
			child[i]->Destroy();
			delete child[i];
		}
	}
	child.clear();
}