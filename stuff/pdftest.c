#include <hpdf.h>
#include <hpdf_consts.h>
#include <hpdf_objects.h>
#include <hpdf_types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>

jmp_buf env;


#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler (HPDF_STATUS   error_no,
               HPDF_STATUS   detail_no,
               void         *user_data)
{
    printf ("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
                (HPDF_UINT)detail_no);
    longjmp(env, 1);
}

int draw_Centered_OneLine_Text (HPDF_Doc pdf,
            HPDF_Page page,
            char * text, 
            HPDF_Rect borders, 
            HPDF_Font fontname, 
            HPDF_REAL fontsize,
            int lines)
{
    HPDF_PageAttr attr = (HPDF_PageAttr )page->attr;
    
    struct box {
        HPDF_REAL width;
        HPDF_REAL height;
    }   textbox;

    HPDF_Rect output;

    HPDF_Box box = HPDF_Font_GetBBox(fontname);
    HPDF_REAL font_cap = HPDF_Font_GetCapHeight(fontname) * fontsize / 1000;
    HPDF_REAL mult = (box.top - box.bottom) / 1000 * fontsize;

    HPDF_Page_SetTextLeading(page, font_cap);
    HPDF_Page_SetFontAndSize(page, fontname, fontsize);

    textbox.width = borders.right - borders.left;
    textbox.height = attr->gstate->text_leading * lines * mult;

    HPDF_REAL font_offset_top = box.top / 1000 * fontsize - font_cap;
    HPDF_REAL font_offset_bottom = box.bottom / 1000 * fontsize;


    
    output.left = borders.left;
    output.right = borders.right;   
    output.top = borders.top + font_offset_top;
    output.bottom = borders.bottom + font_offset_bottom - textbox.height;

    HPDF_Page_TextRect(page, output.left, output.top, output.right, output.bottom, text, HPDF_TALIGN_CENTER, NULL);
    
    return 0;

}

int main ()
{
    HPDF_Doc  pdf;
    HPDF_Page page;
    HPDF_PageSizes diploma_size = HPDF_PAGE_SIZE_A4;
    HPDF_PageDirection diploma_direction = HPDF_PAGE_LANDSCAPE;
    
    HPDF_Image background;
    HPDF_Point background_size;

    HPDF_REAL page_height, page_width;
    
    const float dpi_conversion = 72.0 / 300;
    
    HPDF_Font bold_font, regular_font;
    const char *bold_font_name, *regular_font_name;

    HPDF_Rect rect1, rect2, rect3;
    HPDF_Box  bold_box, regular_box;

    HPDF_Rect border;

    HPDF_REAL bold_font_pt = 26, regular_font_pt = 8;

    HPDF_REAL bold_font_size = bold_font_pt / dpi_conversion, regular_font_size = regular_font_pt / dpi_conversion;


    HPDF_PageAttr attr;

    struct box {
        HPDF_REAL width; float height;
     } box1, box2, box3;

    char * NameSurname = "";
    char * PlaceName = "";
    char * TeacherFIO = "";



    char *lorem = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
    char *UPPERCASE = "THIS IS AN UPPERCASE TEST STRING";

    
   
    //  Create new document
    pdf = HPDF_New (error_handler, pdf);

    HPDF_UseUTFEncodings(pdf);
    HPDF_SetCurrentEncoder(pdf, "UTF-8");
    
    //  Set compression (image, text and all), 
    //  so that the 300dpi images don't weigth 26MB - analogue to zip-compression after
    HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
    
    //  Setting up page layout
    HPDF_SetPagesConfiguration (pdf, 1);
    HPDF_SetPageLayout (pdf, HPDF_PAGE_LAYOUT_SINGLE);
    
    //  Loading up the fonts
    bold_font_name = HPDF_LoadTTFontFromFile(pdf, "CenturyGothicBold.ttf", HPDF_TRUE);
    regular_font_name = HPDF_LoadTTFontFromFile(pdf, "CenturyGothic.ttf", HPDF_TRUE);

    //  Create new page in the document
    page = HPDF_AddPage(pdf);

    attr = (HPDF_PageAttr )page->attr;
            
    //  Set page size and direction to A4 Landscape, and convert from 72dpi to 300dpi for hi-res images
    HPDF_Page_SetSize (page, diploma_size, diploma_direction);
    HPDF_Page_Concat (page, dpi_conversion, 0, 0, dpi_conversion, 0, 0); // conversion happens here
    page_height = HPDF_Page_GetHeight(page) / dpi_conversion;
    page_width = HPDF_Page_GetWidth(page) / dpi_conversion;

    printf ("Page height is %f\n", page_height);
    
    //  Text vertical boundaries
    const float upperlimit = page_height - 1000.0;
    const float lowerlimit = page_height - 1288.0;
  
       
    //  Loading background image from external file and setting its size
    background = HPDF_LoadPngImageFromFile(pdf, "gramota.png");
    background_size = HPDF_Image_GetSize(background);

    //  In case we need to squeeze landscape-like picture into the page
    float coeff = background_size.y / background_size.x;
    
    //  Drawing the image!
    HPDF_Page_DrawImage(page, background, 0.0, 0.0, page_width, page_width * coeff);
    

    bold_font = HPDF_GetFont(pdf, bold_font_name, "UTF-8");
    regular_font = HPDF_GetFont(pdf, regular_font_name, "UTF-8");
        
    bold_box = HPDF_Font_GetBBox(bold_font);
    regular_box = HPDF_Font_GetBBox(regular_font);

    const float bold_cap = HPDF_Font_GetCapHeight(bold_font) * bold_font_size / 1000.0;
    const float regular_cap = HPDF_Font_GetCapHeight(regular_font) * regular_font_size / 1000.0;
    int result = 0;

    float inner_offset = 7;
    
    float bold_mult = 1, regular_mult = (regular_box.top - regular_box.bottom) / 1000 * regular_font_size ;// = 1.5;
    float offset = (upperlimit - lowerlimit - bold_cap - 3*regular_cap*1.2) / 4; //= 22;
 
    HPDF_Page_BeginText(page);
    
    //  RGB for bold font is (0.933, 0.11, 0.145)
    //  RGB for regular font is (0.302, 0.106, 0.729)

    
    
    HPDF_Page_SetTextLeading(page, bold_cap);
    HPDF_Page_SetFontAndSize(page, bold_font, bold_font_size);
    HPDF_Page_SetRGBFill(page, 0.933, 0.11, 0.145);
    
    //  box1 - Bold text rectangle definitions
    box1.width = 1880;
    box1.height = attr->gstate->text_leading * 1;
    
    HPDF_REAL bold_offset_top = bold_box.top / 1000 * bold_font_size - bold_cap;
    HPDF_REAL bold_offset_bottom = bold_box.bottom / 1000 * bold_font_size;
    
    HPDF_REAL regular_offset_top = regular_box.top / 1000 * regular_font_size - regular_cap;
    HPDF_REAL regular_offset_bottom = regular_box.bottom / 1000 * regular_font_size;
    
    float current_line = upperlimit - offset;
    
    rect1.left = (page_width - box1.width) / 2;
    rect1.right = rect1.left + box1.width;
    rect1.top = current_line + bold_offset_top;
    rect1.bottom = rect1.top + bold_offset_bottom - box1.height;

    current_line = current_line - box1.height - offset;

        
    //  Bold text rectangle rendering, centered text
    //  HPDF_Page_TextRect (page, rect1.left, rect1.top, rect1.right, rect1.bottom, UPPERCASE, HPDF_TALIGN_CENTER, NULL);
    
    border.top = upperlimit - offset; border.bottom = upperlimit - offset - bold_cap; border.left = (page_width - box1.width) / 2; border.right = border.left + box1.width;

    draw_Centered_OneLine_Text(pdf, page, UPPERCASE, border, bold_font, bold_font_size, 1);


    
    //  box2 - regular text rectangle definitions #1
        
    HPDF_Page_SetTextLeading(page, regular_cap * 1.2);
    HPDF_Page_SetFontAndSize(page, regular_font, regular_font_size);
    HPDF_Page_SetRGBFill(page, 0.302, 0.106, 0.729);

       
    box2.width = box1.width;
    box2.height = attr->gstate->text_leading  * regular_mult * 1;   // attr->gstate->text_leading * 1;
    
    rect2.left = (page_width - box2.width) / 2;
    rect2.right = rect2.left + box2.width;
    rect2.top = current_line + regular_offset_top;
    rect2.bottom = rect2.top + regular_offset_bottom - box2.height;

    current_line = current_line - box2.height / regular_mult - offset;

    
    //  HPDF_Page_TextRect (page, rect2.left, rect2.top, rect2.right, rect2.bottom, "i ain't THE sharpest tool in THE she-e-d she was looking kinda dumn with a finger and a thumb in the shape of an", HPDF_TALIGN_CENTER, NULL);


    //  box3 - regular text rectangle definitions #1
    
    box3.width = box2.width;
    box3.height = attr->gstate->text_leading * 2 * regular_mult; 
    
    rect3.left = (page_width - box3.width) / 2;
    rect3.right = rect3.left + box3.width;
    rect3.top = current_line + regular_offset_top;
    rect3.bottom = rect3.top + regular_offset_bottom - box3.height;

    current_line = current_line - box3.height / regular_mult;

   

    //  HPDF_Page_TextRect (page, rect3.left, rect3.top, rect3.right, rect3.bottom, "i ain't THE sharpest tool in THE she-e-d she was looking kinda dumn with a finger and a thumb in the shape of an 'L' at her forhead i ain't the sharpest tool in the she-e-d she was looking kinda dumn with a finger and a thumb", HPDF_TALIGN_CENTER, NULL);
        
    
    
    //  HPDF_Page_ShowText(page, "Привет, МИР!");

    for (int i = 0; i < 12; i++)
    {
        char printout[20];
        sprintf (printout, "Line number %d", i);
        HPDF_REAL v_offset = 15;
        int lines = 2;

        border.top = 1000 - (regular_cap*lines + v_offset)*i; border.bottom = 1000 - (i+1)*(regular_cap*lines + v_offset); border.left = 700; border.right = 1000;

        draw_Centered_OneLine_Text(pdf, page, printout, border, regular_font, regular_font_size, 1);
    }
    

    HPDF_Page_EndText(page);
    
    HPDF_Page_Rectangle(page, border.left, border.bottom + 15, border.right - border.left, border.top - border.bottom - 15);
    HPDF_Page_Stroke(page);
      
    //  Save to file
    HPDF_SaveToFile(pdf, "test.pdf");
    //  Free the document
    HPDF_Free(pdf);
    
    return 0;
    
}