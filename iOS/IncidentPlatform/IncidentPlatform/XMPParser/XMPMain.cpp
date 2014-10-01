/*#include <stdio.h>

#include "dss_list.h"
//#include "XMPParser.h"
#include "XMPTree.h"

using namespace dss;

RESULT TestSampleInput()
{
    RESULT r = R_SUCCESS;
    
    printf("***********************************\r\n");
    printf("Test Sample XMP Input\r\n");
    printf("This will test XMP parsing\r\n");
    printf("***********************************\r\n");

    SmartBuffer *pTemp = NULL;

    XMPTree MyTree("sampleinput.xml");
    CRM(MyTree.PrintXMPTree(pTemp), "Failed to print out XMP Tree");

    printf("%s", pTemp->GetBuffer());

Error:
    return r;
}

RESULT CreateCustomTree()
{
    RESULT r = R_SUCCESS;

    printf("***********************************\r\n");
    printf("Create Custom XMP Tree Test\r\n");
    printf("This will test custom tree creation\r\n");
    printf("***********************************\r\n");

    XMPTree *pTree = new XMPTree();
    SmartBuffer *pTemp = NULL;

    //pTree->AppendContent("idan rules");
    pTree->AddChildByName("tempnode");
    pTree->NavigateToChildName("tempnode");
    pTree->AddChildByName("secondtempnode");
    pTree->AppendContent("Idanz!");
    

    pTree->NavigateToParent();
    pTree->AppendContent("hello!");
    
    
    CRM(pTree->PrintXMPTree(pTemp), "Failed to print out XMP Tree");    
    printf("%s", pTemp->GetBuffer());

Error:
    return r;
}


int main(int argv, char* argc[])
{
    RESULT r = R_SUCCESS;
    
    TestSampleInput();
    CreateCustomTree();


    //system("PAUSE");
    
    //return RESULT;
Error:
    return r;
}
*/

