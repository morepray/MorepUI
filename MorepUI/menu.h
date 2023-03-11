#ifndef __MENU_H__
#define __MENU_H__

#include "main.h"

#define BoxWidth 80
#define BoxHeight 10
#define FirstLine 9
#define LineSpace 10

typedef struct MenuPage *xpMenu;
typedef struct Item *xpItem;
typedef struct MenuPage
{
    uint8_t *PageName;
    uint8_t len;
    xpItem ParentiTem;
    xpItem itemHead;
    xpItem itemTail;
} xMenu;

typedef struct Item
{
    uint8_t *itemName;
    xpMenu location;
    xpMenu JumpPage;
    xpItem lastiTem;
    xpItem nextiTem;
} xItem;

// Page
extern xMenu MainPage, Page1, Page2, Page3, Page4, Page5, No3Page1, No3Page2;
extern xpMenu NowPage;
// item
extern xpItem Selectitm;
extern xItem Mainitem1, Mainitem2, Mainitem3, Mainitem4, Mainitem5, Mainitem6;
extern xItem Page1item1, Page1item2, Page1item3;
extern xItem Page2item1, Page2item2, Page2item3;
extern xItem No3Pageitem1, No3Pageitem2, No3Pageitem3;
void Menu_Init(void);
void AddPage(uint8_t *name, xpMenu page);
void AddItem(uint8_t *Name, xpItem item, xpMenu LocalPage, xpMenu NextPage);

void DrawPage(uint8_t pos, xpMenu Page, uint8_t LineSpacing);

#endif
