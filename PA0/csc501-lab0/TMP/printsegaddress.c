#include <stdio.h>
extern long etext, edata, end;

void printsegaddress()
{
	kprintf("\n\nSecond Task: print the address and contents of text, data, BSS registers\n\n");
    kprintf("\nThe address of text is 0x%08x", &etext);
    kprintf("\nThe address of data is 0x%08x", &edata);
    kprintf("\nThe address of BSS segment is 0x%08x", &end);
    kprintf("\nThe content preceding text is 0x%08x", *(&etext-1));
    kprintf("\nThe content after text is 0x%08x", *(&etext+1));
    kprintf("\nThe content preceding data is 0x%08x", *(&edata-1));
    kprintf("\nThe content after data is 0x%08x", *(&edata+1));
    kprintf("\nThe content preceding BSS is 0x%08x", *(&end-1));
    kprintf("\nThe content after BSS is 0x%08x", *(&end+1));    
}