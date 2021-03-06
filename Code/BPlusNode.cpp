#include "BPlusNode.h"
#include <memory>
#include <iostream>
// interNode
// remove the key and the pointer to corresponding children
void interNode::remove(int keyIndexFrom, int keyIndexTo){
	// update the total weight of the node
	int deleteWeight = 0;
	for (int i = keyIndexFrom; i <= keyIndexTo; ++i){
		deleteWeight += getKeyValue(i);
	}
	setTotalWeight(getTotalWeight() - deleteWeight);

	// copy the latter keys 
	int moveCount = getCurKeyNum() - keyIndexTo - 1;
	// int limit = keyIndexFrom + moveCount - 1;
	for(int i = 0; i < moveCount; ++i){
		setKeyValue(keyIndexFrom + i, getKeyValue(keyIndexTo + 1 + i));
		setChild(keyIndexFrom + i, getChild(keyIndexTo + 1 + i));
	}
	// update the current size of keys 
	setCurKeyNum(getCurKeyNum() - (keyIndexTo - keyIndexFrom));
}

// insert a key into the node
void interNode::insert(int targetPosition, BPlusNode* childNode){
	int tempNum = getCurKeyNum();
	// if the new key should be the end of the keys
	if (targetPosition == tempNum){
		setKeyValue(targetPosition, childNode->getTotalWeight());
		setChild(targetPosition, childNode);
	}
	else{
		// the new key is in the middle or start of the keys
		for (int i = getCurKeyNum(); i > targetPosition; --i){
			setKeyValue(i, getKeyValue(i - 1));
			setChild(i, getChild(i - 1));
		}
		setKeyValue(targetPosition, childNode->getTotalWeight());
		setChild(targetPosition, childNode);
	}
	// update the total weight
	setTotalWeight(getTotalWeight() + childNode->getTotalWeight());
	// update the size of keys
	setCurKeyNum(getCurKeyNum() + 1);
	// set the childnode's parent
	childnode->setParent(this);
	// set the childnode's index 
	childnode->setIndexParent(targetPosition); 
}

// split the node, creat a new node in the right side
void interNode::split(){
	interNode* newNode = new interNode();
	int leftNum = getCurKeyNum() / 2 + 1;
	int rightNum = getCurKeyNum() - leftNum;
	// update the new right node's size
	newNode->setCurKeyNum(rightNum);

	// copy the right part to the new node
	int newTotalWeight = 0;
	for (int i = 0; i < rightNum; ++i){
		newNode->keys[i] = getKeyValue(leftNum + i);
		newNode->childs[i] = getChild(leftNum + i);
		newTotalWeight += newNode->keys[i];
	}
	// remove the splited keys in the old left node
	remove(leftNum, getCurKeyNum() - 1);
	// update the total weight of new right node
	newNode->setTotalWeight(newTotalWeight);
	// set the parent pointer for new right node
	interNode* parentNode = getParent();
	parentNode->insert(getIndexParent() + 1, newNode);
	// update the preNode and the nextNode pointers
	newNode->setPreNode(this);
	newNode->setNextNode(getNextNode());
	setNextNode(newNode);
}

// merge the right node into the left one
void interNode::merge(interNode* mergeNode){
	int plusLen = mergeNode->getCurKeyNum();
	for (int i = getCurKeyNum(); i < getCurKeyNum() + plusLen; i++){
		insert(i, mergeNode->getChild(i - getCurKeyNum()));
	}
	// update the new left node's pointer
	setNextNode(mergeNode->getNextNode());
	// remove the key things in the parent of right node
	mergeNode->remove(mergeNode->getIndexParent(), mergeNode->getIndexParent());
	// to release the old right node
	//
}

// clear the node and its subtree
void interNode::clear(){
	for (int i = 0; i < getCurKeyNum(); ++i){
		interNode* temp = getChild[i];
		temp->clear();
		delete temp;
	}
}

// borrow from the left brother node or right
void interNode::borrow(interNode* brotherNode, int left, int num){
	if (left){
		// this complexcity can be improved....
		int tempNumBro = brotherNode->getCurKeyNum();
		// int tempNum = getCurKeyNum();
		// for (int i = 0; i < num; ++i){

		// }
		for (int i = 0; i < num; ++i){
			insert(0, brotherNode->getChild(tempNumBro - 1 - i));
		}
		brotherNode->remove(tempNumBro - num, tempNumBro - 1);
	}
	else{
		for (int i = 0; i < num; ++i){
			insert(getCurKeyNum() + i, brotherNode->getChild(i));
		}
		brotherNode->remove(0, num - 1);
	}
}

// leafNode
// clear, release the data includes
void leafNode::clear(){
	for (int i = 0; i < getCurKeyNum(); ++i){
		delete datas[i];
	}
}

// insert the data(a string) into the leafNode
void leafNode::insert(int targetPosition, string& childString){
	int tempNum = getCurKeyNum();
	// if the new key should be the end of the keys
	if (targetPosition == tempNum){
		setKeyValue(targetPosition, childString.size());
		setData(targetPosition, childString);
	}
	else{
		// the new key is in the middle or start of the keys
		for (int i = getCurKeyNum(); i > targetPosition; --i){
			setKeyValue(i, getKeyValue(i - 1));
			setData(i, getData(i - 1));
		}
		setKeyValue(targetPosition, childString.size());
		setData(targetPosition, childString);
	}
	// update the total weight
	setTotalWeight(getTotalWeight() + childString.size());
	// update the size of keys
	setCurKeyNum(getCurKeyNum() + 1);
}

// remove the indexing key from the leaf node
void leafNode::remove(int keyIndexFrom, int keyIndexTo){
	// update the total weight of the node
	int deleteWeight = 0;
	for (int i = keyIndexFrom; i <= keyIndexTo; ++i){
		deleteWeight += getKeyValue(i);
	}
	setTotalWeight(getTotalWeight() - deleteWeight);
	int moveCount = getCurKeyNum() - keyIndexTo - 1;
	for(int i = 0; i < moveCount; ++i){
		setKeyValue(keyIndexFrom + i, getKeyValue(keyIndexTo + 1 + i));
		setData(keyIndexFrom + i, getData(keyIndexTo + 1 + i));
	}
	// update the current size of keys 
	setCurKeyNum(getCurKeyNum() - (keyIndexTo - keyIndexFrom));
}

// split
void leafNode::split(){
	leafNode* newNode = new leafNode();
	int leftNum = getCurKeyNum() / 2 + 1;
	int rightNum = getCurKeyNum() - leftNum;
	// update the new right node's size
	newNode->setCurKeyNum(rightNum);

	// copy the right part to the new node
	int newTotalWeight = 0;
	for (int i = 0; i < rightNum; ++i){
		newNode->keys[i] = getKeyValue(leftNum + i);
		newNode->datas[i] = getData(leftNum + i);
		newTotalWeight += newNode->keys[i];
	}
	// remove the splited keys in the old left node
	remove(leftNum, getCurKeyNum() - 1);
	// update the total weight of new right node
	newNode->setTotalWeight(newTotalWeight);
	// set the parent pointer for new right node
	interNode* parentNode = getParent();
	parentNode->insert(getIndexParent() + 1, newNode);
	// update the preNode and the nextNode pointers
	newNode->setPreNode(this);
	newNode->setNextNode(getNextNode());
	setNextNode(newNode);
}

// merge
void leafNode::merge(leafNode* mergeNode){
	int plusLen = mergeNode->getCurKeyNum();
	for (int i = getCurKeyNum(); i < getCurKeyNum() + plusLen; i++){
		insert(i, mergeNode->getData(i - getCurKeyNum()));
	}
	// update the new left node's pointer
	setNextNode(mergeNode->getNextNode());
	// remove the key things in the parent of right node
	mergeNode->remove(mergeNode->getIndexParent(), mergeNode->getIndexParent());
}

// borrow
void leafNode::borrow(leafNode* brotherNode, int left, int num){
	if (left){
		// this complexcity can be improved....
		int tempNumBro = brotherNode->getCurKeyNum();
		for (int i = 0; i < num; ++i){
			insert(0, brotherNode->getData(tempNumBro - 1 - i));
		}
		brotherNode->remove(tempNumBro - num, tempNumBro - 1);
	}
	else{
		for (int i = 0; i < num; ++i){
			insert(getCurKeyNum() + i, brotherNode->getData(i));
		}
		brotherNode->remove(0, num - 1);
	}
}

// search each key of the node to find the right key index, k = weight
int BPlusNode::search(int k){
	int tempWeight = 0;
	int targetIndex;
	for (int i = 0; i < getCurKeyNum(); ++i){
		tempWeight += getKeyValue(i);
		if (tempWeight >= k){
			targetIndex = i;
			break;
		}
	}
	return targetIndex;
}