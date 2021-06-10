#include<stdio.h>
#include<malloc.h>
#include<math.h>
#include<string.h>
#include<dirent.h>

struct element {            //I created the structure
    char fileName[200];
    int score;              //This variable is for relevancy scores.
    int degree;             //This holds the degree of the binomial tree.
    struct element *parent;     //To construct a binomial heap structure we need those 3 pointers.
    struct element *sibling;
    struct element *child;
};

typedef struct element Element;

Element *mainHeader = NULL;         //This holds main tree's header.
Element *childHeader = NULL;        //This holds a temporary tree while we extract the minimum element.
int count = -2;                     //This holds the number of files. It starts from -2 because first 2 directories will be "." and "..".

void connect(Element* largeElement, Element* smallElement) {       //This method connects two trees with the same degree.
    largeElement->parent = smallElement;                //We make necessary links in those two tree.
    largeElement->sibling = smallElement->child;
    smallElement->child = largeElement;
    smallElement->degree += 1;                      //Since we merge those two trees we increment the degree by one.
}

Element *createNodeFirst(char *fileName) {      //I use this method to create main elements.
    Element *file = malloc(sizeof(Element));    //Since I dont know the score of the element at the beginning, by default it is 0.
    strcpy(file->fileName, fileName);
    file->parent = NULL;
    file->sibling = NULL;
    file->child = NULL;
    file->degree = 0;
    file->score = 0;
    return file;
}

Element *createNode(char *fileName, int score) {    //I use this method to create temporary elements.
    Element *file = malloc(sizeof(Element));        //Since I know the score of the elements now i can create elements with this score.
    strcpy(file->fileName, fileName);
    file->parent = NULL;
    file->sibling = NULL;
    file->child = NULL;
    file->degree = 0;
    file->score = score;
    return file;
}

Element *mergeRootList(Element *header_1, Element *header_2) {  //This method merges root lists of two trees.
    Element *tempMainHeader = NULL;
    Element *tempHeader_1;
    Element *tempHeader_2;
    Element *tempTree;
    tempHeader_1 = header_1;    //We create temporary elements two traverse trees and keep the original header values.
    tempHeader_2 = header_2;
    if (tempHeader_1 != NULL) {     //This if - else checks the base case of merging
        //This if - else checks the degrees and assigns main header to make the tree in ascending order.
        if (tempHeader_2 == NULL || (tempHeader_2 != NULL && tempHeader_1->degree <= tempHeader_2->degree))
            tempMainHeader = tempHeader_1;
        else
            tempMainHeader = tempHeader_2;
    } else
        tempMainHeader = tempHeader_2;
    while (tempHeader_1 != NULL && tempHeader_2 != NULL) {  //This loop choses the correct place for the tree to be merged.
        if (tempHeader_1->degree < tempHeader_2->degree) {  //If this case occurs we skip to the next element.
            tempHeader_1 = tempHeader_1->sibling;
        } else if (tempHeader_1->degree == tempHeader_2->degree) {  //If this case occurs we put the second header to the sibling of first header.
            tempTree = tempHeader_1->sibling;
            tempHeader_1->sibling = tempHeader_2;
            tempHeader_1 = tempTree;
        } else {    //If this case occurs we put the first header to the sibling of second header.
            tempTree = tempHeader_2->sibling;
            tempHeader_2->sibling = tempHeader_1;
            tempHeader_2 = tempTree;
        }
    }
    return tempMainHeader;  //We return tempMainHeader to actually change the main header.
}

Element *merge(Element *header_1, Element *header_2) {  //This function merges whole tree and arranges it.
    Element *currentElement;
    Element *nextElement;
    Element *previousElement;
    Element *tempHeader = NULL;
    tempHeader = mergeRootList(header_1, header_2); //First we obtain one binomial heap from two trees.
    if (tempHeader == NULL)     //If no such tree exists we return the tempHeader which is NULL.
        return tempHeader;
    previousElement = NULL;     //We insert previous, current and next elements to check the same degree condition
    currentElement = tempHeader;
    nextElement = currentElement->sibling;
    while (nextElement != NULL) {   //We traverse the tree until we get to the end.
        //This statement checks if we have two elements that have same degree next to each other or with some other tree with different degree in between.
        if ((currentElement->degree != nextElement->degree) || ((nextElement->sibling != NULL) && (nextElement->sibling)->degree == currentElement->degree)) {
            previousElement = currentElement; //If we don't have the trees with same degree we skip to the next pair.
            currentElement = nextElement;
        } else {        //If we have two trees with same degree we connect those trees.
            if (currentElement->score <= nextElement->score) {  //Check the scores to decide which tree will be the parent tree.
                currentElement->sibling = nextElement->sibling; //Since it's a min heap, tree with a smaller score will be the parent.
                connect(nextElement, currentElement);       //Method takes larger parameter first.
            } else {
                if (previousElement == NULL)        //If current is the first element of the tree , update the header.
                    tempHeader = nextElement;
                else
                    previousElement->sibling = nextElement; //We put a sibling link.
                connect(currentElement, nextElement);   //We connect trees with the same degree.
                currentElement = nextElement;       //We update the current element since it is another tree's child.
            }
        }
        nextElement = currentElement->sibling;     //Go to next element.
    }
    return tempHeader;      //Return tempHeader to change the original Header value.
}

Element *insert(Element *header, Element *element) {    //This method inserts one element to a binomial tree.
    Element *tempHeader = NULL;
    element->degree = 0;
    element->parent = NULL;
    element->sibling = NULL;
    element->child = NULL;
    tempHeader = element;   //We create a temp element then send it to merge function and assign the return value to header.
    header = merge(header, tempHeader);
    return header;  //We return updated header pointer.
}

void getChildLL(Element *childPtr) {    //This method arranges linked list of the extracted element.
    if (childPtr->sibling != NULL) {    //After this function linked list is on ascending order by degree.
        getChildLL(childPtr->sibling);
        (childPtr->sibling)->sibling = childPtr;
    } else {
        childHeader = childPtr;
    }
}

Element *getMin(Element *header) {  //This method extracts minimum element from the tree.
    int min;
    Element *previous = NULL;
    Element *current = header;
    Element *minTraverser;
    childHeader = NULL;
    if (current == NULL) {      //If tree is empty we return NULL
        printf("\nNo elements in heap.");
        return current;
    }
    min=current->score; //We assign relevancy score of the first element of tree to min.
    minTraverser = current; //We assign a temp pointer to traverse the tree.
    while (minTraverser->sibling != NULL) {     //We traverse the tree until the end.
        if ((minTraverser->sibling)->score < min) { //We only check the roots because it is a minHeap.
            min = (minTraverser->sibling)->score;   //Update the min value.
            previous = minTraverser;
            current = minTraverser->sibling;
        }
        minTraverser = minTraverser->sibling;   //Jump to next element.
    }
    if (previous == NULL && current->sibling == NULL)   //If tree has only one root we assign NULL to header.
        header = NULL;
    else if (previous == NULL)      //If min element is the first root, we update the header with the next element.
        header = current->sibling;
    else                            //Make the links so after extraction the links are proper.
        previous->sibling = current->sibling;
    if (current->child != NULL) {   //Get a linked list of the childs in ascending order by degree.
        getChildLL(current->child);
        (current->child)->sibling = NULL;   //This blocks the possible loop in linked list.
    }
    Element *temp = NULL;
    temp = header;
    double totalHeader = 0;
    double totalChildHeader = 0;
    while(temp != NULL) {       //In this while loops we count the number of elements in the trees.
        totalHeader += pow(2,temp->degree);
        temp = temp->sibling;
    }
    temp = childHeader;
    while(temp != NULL) {
        totalChildHeader += pow(2,temp->degree);
        temp = temp->sibling;
    }
    if(totalChildHeader >= totalHeader)     //We send the most crowded tree as main header.
        mainHeader = merge(childHeader, header);
    else
        mainHeader = merge(header, childHeader);
    return current; //We return the extracted element.
}

int main() {
    struct dirent *de;
    char keyword[100];
    DIR *dr = opendir("files"); //We open the files directory.
    printf("Please enter a keyword: "); //We get the keyword input from the user.
    scanf("%s",&keyword);
    FILE *x = fopen("filenames.txt","w");   //Open a txt file to hold the names of the files.
    while ((de = readdir(dr)) != NULL) {    //We get the file names and write them to text file.
        fprintf(x, de->d_name);
        fprintf(x,"\n");
        count++;    //We count the elements to create an array with proper size.
    }
    fclose(x); //We close the file after we are done with it.

    if (dr == NULL) {   //If directory pointer is NULL then directory opening operation is failed.
        printf("Could not open current directory" );
        return 0;
    }

    Element elementArray[count];    //We create an array to hold the elements before insertion to the binomial tree.
    char str[80];
    x = fopen("filenames.txt","r"); //We read the file names from text file we created before.
    int i = 0;
    int numOfRelevantFiles = 0;

    while(fgets(str, 80, x) != NULL) {
        char fName[200];
        char addr[100] = "files/";
        char *token = strtok(str, "\n");    //In the text file file names have "\n" at the end. In this line we remove those.
        strcpy(fName, token);
        if(strcmp(fName,".") == 0){ //We skip the first two names because directory doesn't give us correct names in first 2 iterations.
        }
        else if(strcmp(fName,"..") == 0){
        }
        else {
            elementArray[i] = *createNodeFirst(fName);  //We create the elements and put them in the array.
            strcpy(fName,strcat(addr,fName));   //We concat the name string with an address to open the file via string.
            FILE *y = fopen(fName,"r");  //We open the file with the string we created before.
            char word[1024];
            char punc[10] = {".,;:?!()-"};  //To ignore the punctuation we define a punctuation array.
            int simScore = 0;
            while (fscanf(y, "%1023s", word) == 1) {
                /*In this following lines we check if we have a word that is seperated by punctuation.
                If so we seperate the word into two parts*/
                char *token = word;
                char *token2 = NULL;
                int loopCount = 0;
                do {
                    if(token2 == NULL && loopCount != 0)
                        break;
                    if(token2 != NULL)
                        token = token2;
                    token = strtok(token,punc);
                    token2 = strtok(NULL,punc);
                    if(stricmp(token,keyword) == 0) //We compare the word with keyword.
                        simScore++; //If they are equal we update the relevancy.
                    loopCount++;
                } while(token != NULL);
            }
            if(simScore > 0)      //We take the number of relevant files.
                numOfRelevantFiles++;
            /*Since we have a minHeap and we want the maximum element we update the relevancy score.We subtract it from
            a bigger number so maximum numbers become minimum so after extraction we actually get minimum elements.*/
            elementArray[i].score = 9999-simScore;
            i++;
        }
    }
    fclose(x);  //We close the file and directory since we are done with them.
    closedir(dr);
    int a;
    Element *tempElement;
    for(a = 0; a < count; a++) {    //We create a temp element and insert it to the tree.
        tempElement = createNode(elementArray[a].fileName, elementArray[a].score);
        mainHeader = insert(mainHeader, tempElement);
    }
    int loopSize = numOfRelevantFiles;
    if(loopSize > 5)
        loopSize = 5;
    if(loopSize != 0)
        printf("Relevance order is : \n");
    else
        printf("Keyword doesn't exist in files.");
    Element printElement[5];    //This array holds the top 5 relevant elements.
    for(a = 0; a < loopSize; a++) {
        Element *temp1 = NULL;
        temp1 = getMin(mainHeader); //We get the minimum element and put it in the array.
        printElement[a] = *createNode(temp1->fileName, temp1->score);
        //We subtract score from 9999 to get the actual value of score.
        printf("%s(%d)\n",temp1->fileName,9999 - temp1->score); //We print the results.
    }
    printf("\n");
    for(a = 0; a < loopSize; a++) {    //In this loop we open top 5 elements and print them on the screen.
        char addr[200] = "files/";
        strcpy(addr,strcat(addr,printElement[a].fileName));
        x = fopen(addr, "r");   //We open the file via address string.
        printf("%s(%d): ", printElement[a].fileName,9999 - printElement[a].score);
        while(fscanf(x, "%s", str) != EOF) {    //We read the file and print the output.
            printf("%s ", str);
        }
        fclose(x);
        printf("\n\n");
    }
    return 0;
}

