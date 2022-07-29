
//  sql handling stuff
#include <mysql/mysql.h>
#include <stdlib.h>
#include <stdio.h>

//  pdf creation stuff
#include <hpdf.h>
#include <hpdf_consts.h>
#include <hpdf_objects.h>
#include <hpdf_types.h>

#include <string.h>
#include <setjmp.h>

//  dir creation suff
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


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

    HPDF_Box     box = HPDF_Font_GetBBox(fontname);
    HPDF_REAL    font_cap = HPDF_Font_GetCapHeight(fontname) * fontsize / 1000;
    HPDF_REAL    mult = (box.top - box.bottom) / 1000 * fontsize;

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



int makePDF (char *NameSurname, char *PlaceName, char *email, char *current_dir, int number)
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

    HPDF_Rect border;
    
    HPDF_REAL bold_font_pt = 26, regular_font_pt = 8; // font size in points

    HPDF_REAL bold_font_size = bold_font_pt / dpi_conversion, regular_font_size = regular_font_pt / dpi_conversion;

    HPDF_PageAttr attr;

    struct box {
        float width; float height;
     } box1, box2, box3;

    char filename[200];
     
    
    //  End of definitions
    
    sprintf(filename, "%s/%s.pdf", current_dir, NameSurname);
    //  printf ("%s \n", filename);

   
   
    //  Create new document
    pdf = HPDF_New (error_handler, NULL);
    
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

    //  Text vertical boundaries
    const float upperlimit = page_height - 1000.0;
    const float lowerlimit = page_height - 1227.0;
       
    //  Loading background image from external file and setting its size
    background = HPDF_LoadPngImageFromFile(pdf, "bpm.png");
    background_size = HPDF_Image_GetSize(background);
    
    //  In case we need to squeeze landscape-like picture into the page
    float coeff = background_size.y / background_size.x;
    
    //  Drawing the image!
    HPDF_Page_DrawImage(page, background, 0.0, 0.0, page_width, page_width * coeff);
    

    bold_font = HPDF_GetFont(pdf, bold_font_name, "UTF-8");
    regular_font = HPDF_GetFont(pdf, regular_font_name, "UTF-8");
           
    const float bold_cap = HPDF_Font_GetCapHeight(bold_font) * bold_font_size / 1000.0;
    const float regular_cap = HPDF_Font_GetCapHeight(regular_font) * regular_font_size / 1000.0;
       
    float offset = (upperlimit - lowerlimit - bold_cap - regular_cap) / 3;
    float current_line = upperlimit - offset;
    
       
    HPDF_Page_BeginText(page);
    
    //  RGB for bold font is (0.933, 0.11, 0.145)
    //  RGB for regular font is (0.302, 0.106, 0.729)
       
    HPDF_Page_SetRGBFill(page, 0.933, 0.11, 0.145);
    
    //  box1 - Bold text rectangle definitions
    
    box1.width = 2500;
    box1.height = bold_cap;
    
    //  Bold text rectangle rendering, centered text
       
    border.top = current_line; border.bottom = current_line - bold_cap; border.left = (page_width - box1.width) / 2; border.right = border.left + box1.width;

    draw_Centered_OneLine_Text(pdf, page, NameSurname, border, bold_font, bold_font_size, 1);
    
     
    
    //  box3 - regular text rectangle definitions #1
    HPDF_Page_SetRGBFill(page, 0.302, 0.106, 0.729);

    box3.width = box1.width;
    box3.height = regular_cap;

    current_line = current_line - box1.height - offset + 15;

    border.top = current_line; border.bottom = current_line - regular_cap; border.left = (page_width - box3.width) / 2; border.right = border.left + box3.width;

    draw_Centered_OneLine_Text(pdf, page, PlaceName, border, regular_font, regular_font_size, 1);

   
    HPDF_Page_EndText(page);
    
    //  Save to file

    printf ("Filename to write is %s ", filename);

    HPDF_SaveToFile(pdf, filename);
    //  Free the document
    HPDF_Free(pdf);
    //  if (output != NULL) {free (output);}
    //  if (nameUppercase != NULL) {free (nameUppercase);}
  
  return 0;
}



int main(int argc, char **argv)
{  
  
    MYSQL *con = mysql_init(NULL);
    char current_dir[100];


    if (con == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "shambler", "1", 
            NULL, 0, NULL, 0) == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }  

    //    We need the participants' FIO to be in uppercase, and FI only - hence, the following SQL query
    if (mysql_query(con, "SELECT UPPER(RTRIM(LTRIM(FIOt))), RTRIM(LTRIM(place)), LTRIM(RTRIM(email)) FROM mydb.males ORDER BY email asc")) 
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }
    
        MYSQL_RES *result = mysql_store_result (con);

        if ( result == NULL )
        {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
        }

        int num_fields = mysql_num_fields (result);
        int filenum = 0;
        MYSQL_ROW row;
        
        int j = 0;

        while ((row = mysql_fetch_row(result)))
        {
            
            /*
            for (int i = 0; i < num_fields; i++)
            {
                printf ("%s | ", row[i] ? row[i] : "NULL");
            }
            printf ("\n");
            */
            
            sprintf(current_dir, "/home/shambler/Documents/progs/mysql/%s", row[2]);
            //  printf ("Current dir is %s \n", current_dir);
            
            struct stat st = {0};
            if (stat(current_dir, &st) == -1)
            {
                mkdir (current_dir, 0700);
                filenum = 0;
            }
            
            
            makePDF (row[0], row[1], row[2], current_dir, filenum);
            filenum++;

            j++; printf (" - file no %d\n", j);
        
        }
        mysql_free_result (result);

    mysql_close(con);
    
    printf("\n SUCCESS!! \n");
    exit(0);
}

