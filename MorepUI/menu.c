#include "menu.h"
#include "oled.h"

/* Page*/
xpMenu NowPage;
xMenu
    MainPage,                          // 一级Page
    Page1, Page2, Page3, Page4, Page5, // 二级Page
    No3Page1, No3Page2, No3Page3;      // 三级Page

/* item */
// 一级Page的item
xpItem Selectitm;
xItem Mainitem1, Mainitem2, Mainitem3, Mainitem4, Mainitem5, Mainitem6;
// 二级Page的item
xItem Page1item1, Page1item2, Page1item3;
xItem Page2item1, Page2item2, Page2item3;
xItem Page3item1, Page3item2, Page3item3;
xItem Page4item1, Page4item2, Page4item3;
xItem Page5item1, Page5item2, Page5item3;
// 三级Page的item
xItem No3Pageitem1, No3Pageitem2, No3Pageitem3;

void AddPage(uint8_t *name, xpMenu page)
{
    page->PageName = name;
    page->itemHead = NULL;
    page->itemTail = NULL;
}

void AddItem(uint8_t *Name, xpItem item, xpMenu LocalPage, xpMenu nextpage)
{
    item->itemName = Name;
    item->location = LocalPage;
    item->JumpPage = nextpage;
    /* 新建item的下一个肯定是null */
    item->nextiTem = NULL;
    /* 如果可以跳转，那么此item是跳转页面的父级 */
    if (nextpage != NULL)
        nextpage->ParentiTem = item;
    /* 链式结构创建item */
    if (LocalPage->itemHead == NULL) // 如果是第一个iTem
    {
        item->lastiTem = NULL;
        LocalPage->itemHead = item;
        LocalPage->itemTail = item;
        LocalPage->len = 1;
    }
    else // 不是第一个item
    {
        item->lastiTem = LocalPage->itemTail;                // 新item的last指向Local的tailitem
        LocalPage->itemTail->nextiTem = item;                // 让尾巴的next指向新的item，连接起来
        LocalPage->itemTail = LocalPage->itemTail->nextiTem; // 让尾巴指向新的item
        LocalPage->len++;
    }
}
void DrawPage(uint8_t pos, xpMenu Page, uint8_t LineSpacing)
{
    MP_DrawStr(pos, FirstLine, Page->PageName, 6);
    xpItem temp = Page->itemHead;
    for (int i = 1; i <= Page->len; i++)
    {
        MP_DrawStr(pos, FirstLine + i * LineSpacing, temp->itemName, 6);
        temp = temp->nextiTem;
    }
}

void Menu_Init(void)
{
    NowPage = &MainPage;
    Selectitm = &Mainitem1;
    MainPage.ParentiTem = NULL;

    AddPage("[MainPage]", &MainPage);
    AddItem(" -Application", &Mainitem1, &MainPage, &Page1);
    AddItem(" -Text", &Mainitem2, &MainPage, &Page2);
    AddItem(" -Image", &Mainitem3, &MainPage, &Page3);
    AddItem(" -Reset All", &Mainitem4, &MainPage, &Page4);
    AddItem(" -About", &Mainitem5, &MainPage, &Page5);

        AddPage("[Application]", &Page1);
        AddItem(" -VsCode", &Page1item1, &Page1, &No3Page1);
        AddItem(" -STM32CubeMX", &Page1item2, &Page1, &No3Page2);
        AddItem(" -Altium", &Page1item3, &Page1, &No3Page3);

            AddPage("[VsCode]", &No3Page1);
            AddItem(" -File1", &No3Pageitem1, &No3Page1, NULL);
            AddItem(" -File2", &No3Pageitem2, &No3Page1, NULL);
            AddItem(" -File3", &No3Pageitem3, &No3Page1, NULL);

        AddPage("[Text]", &Page2);
        AddItem(" -New Project", &Page2item1, &Page2, NULL);
        AddItem(" -New Project", &Page2item2, &Page2, NULL);
        AddItem(" -New Project", &Page2item3, &Page2, NULL);

        AddPage("[Image]", &Page3);
        AddItem(" -New Project", &Page3item1, &Page3, NULL);
        AddItem(" -New Project", &Page3item2, &Page3, NULL);
        AddItem(" -New Project", &Page3item3, &Page3, NULL);

        AddPage("[Reset All]", &Page4);
        AddItem(" -Reset Name", &Page4item1, &Page4, NULL);
        AddItem(" -Reset Time", &Page4item2, &Page4, NULL);
        AddItem(" -Reset Setting", &Page4item3, &Page4, NULL);

        AddPage("[About]", &Page5);
        AddItem(" -Github", &Page5item1, &Page5, NULL);
        AddItem(" -Bilibili", &Page5item2, &Page5, NULL);
        AddItem(" -ReadME", &Page5item3, &Page5, NULL);
}
