#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
//#pragma GCC poison texture_image
#pragma GCC diagnostic ignored "-Wwrite-strings"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#define  BLOCK_SIZE 4
#include <stdio.h>
#include <iostream>
// Image Type
#include <image.h>

// GUI
#include <imgui/imgui.h>
#include <imgui/local.h>  // reset_image, texture_image,
#include <imgui/imgui_glfw.h>

// STL
#include <vector>
#include <string>

// Local functions
static ImGuiWindowFlags static_window();
static void error_callback(int error, const char* description);

/*
Based on: Transition program showed in class
Author: Pablo Adell 
Date: 01-24-2018
*/
ImVec2 resize(int x, int y)
{
    if (x > 200 && y > 200) {
        float d = (x > y ? x : y) / 200;
        return ImVec2( x/d, y/d );
    }
    return ImVec2(x, y);
}

//pops up the file list
std::string image_select(int s)
{
    static std::vector<std::string> selected = {"",""};
    ImGui::SameLine();
    ImGui::Text(selected[s] == "" ? "<None>" : selected[s].c_str());

    std::string current_dir;


    int sindex;
    if  (s == 0)
        sindex = fs(selected[s], current_dir, "select 0");
        

    if (sindex == -1 )
        return "";
    else
        return current_dir + "/" + selected[s];
}


extern "C" void image_creation(uint8* pixels, Image a, int jump)
{

    int len = a.width * a.height*4;
    int fib_1=0;
    int fib_2 = 1;
    int fib_3= fib_1+fib_2;

    for(int i = 0; i<len; i++) pixels[i]=a.pixels[i];
   
     for(int j = 0; j<len; j+=jump){ //Jumping through the array using user values
            
           fib_1= fib_2;
           fib_2= fib_3;
           fib_3 = fib_1+fib_2; //Computing new Fibonacci's number
    

   if(j%2 == 0){ //Checking divisibility
        if(j+1<len){ //Modifying component G if jump is 4, R/G if jump is 3, A/G if jump is 2 and R/G/B/A is jump is 1 
       pixels[j+1] = (255 * fib_3)/9.8; //Dividing values by gravity constant (meters/second squared)
        }
   }

   if(j%3==0){
      if(j+2<len){ //Modifying component B if jump is 4, G/B if jump is 3, R/B if jump is 2 and R/G/B/A is jump is 1
       pixels[j+2] = (255 * fib_3)/9.8;
      }
    }
    else {
   if(j<len){
    
       pixels[j]= (255 * fib_3)/9.8; //Modifying component R if jump is 4, A/R if jump is 3, A/G if jump is 2 and R/G/B/A is jump is 1
   }
         }
         
 
}
} 
   
   
 

int main()
{
    Image a;
    Image b;
    std::vector<std::string> fnames = {"",""};
    std::string bits_name = "1";

    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(600,450, "Project 1", NULL, NULL);
    // glfwSetWindowPos(window, -1500, 100);
    glfwMakeContextCurrent(window);

    glewInit();

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

    bool slider_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
      glfwPollEvents();
      ImGui_ImplGlfwGL3_NewFrame();

      ImGui::SetNextWindowSize(ImVec2(600,100), ImGuiSetCond_FirstUseEver);
      ImGui::Begin("My project");
      {
        // Image A
        ImGui::BeginGroup();
        {
            if (a.texture_loaded)
                ImGui::Image((void*)a.texture, resize(a.width, a.height));
            if (ImGui::Button("Start"))
                ImGui::OpenPopup("select 0");
            std::string tmp = image_select(0); //opens the file list and assigns to tmp
            if (tmp != "") {
                fnames[0] = tmp;
                reset_image(&a);
                reset_image(&b);
          
                a.pixels = stbi_load(fnames[0].c_str(), &a.width, &a.height, &a.n, RGBA);
                texture_image(&a);
              }
            if(ImGui::Button("Back")){
             
                reset_image(&b);
                free_image(&b);
                free(b.pixels);
                bits_name = " ";
            }

            if(ImGui::Button("Reset")){
                reset_image(&a);
                reset_image(&b);
                free_image(&a);
                free(a.pixels);
                free_image(&b);
                free(b.pixels);
            }
        }
       
           ImGui::EndGroup();
           ImGui::SameLine();

           //Create Image when process is clicked 
        ImGui::BeginGroup();{
          
      
               if (a.texture_loaded && !b.texture_loaded) { //If a is loaded but b is not
                b.width = a.width;
                b.height = a.height;
                b.pixels = (uint8*)malloc(a.width * a.height * RGBA); //Allocate the pixels for b 
               }

            if(b.texture_loaded != 1){
                if (b.pixels && ImGui::Button("Process jump 1 bit!")) { //If the pixels have been allocated, create the image and point to its address for the texture
                image_creation(b.pixels, a, 1);
                texture_image(&b);
                bits_name = "1";
                                    }

                if (b.pixels && ImGui::Button("Process jump 2 bit!")) { //If the pixels have been allocated, create the image and point to its address for the texture
                image_creation(b.pixels, a, 2);
                texture_image(&b);
                bits_name = "2";
                                    }

                  if (b.pixels && ImGui::Button("Process jump 3 bit!")) { //If the pixels have been allocated, create the image and point to its address for the texture
                image_creation(b.pixels, a, 3);
                texture_image(&b);
                bits_name = "3";
                                    }

                
                  if (b.pixels && ImGui::Button("Process jump 4 bit!")) { //If the pixels have been allocated, create the image and point to its address for the texture
                image_creation(b.pixels, a, 4);
                texture_image(&b);
                bits_name = "4";
                                    }
           
             }

            if (b.texture_loaded) { //If b has been created, show it on the display
            ImGui::Image((void*)b.texture, resize(b.width, b.height), ImVec2(0,0), ImVec2(1,1), ImVec4(1,1,1,1), ImVec4(0,255,0,255));
            if (ImGui::Button("Save")) { //When clicked it saves the picture as prueba.png
              char sname[1000];
              bits_name.append("_m.png");
              sprintf(sname, bits_name.c_str());
              stbi_write_png(sname, a.width, a.height, 4, b.pixels, 0);
            }
          }
          
             }
     ImGui::EndGroup();

   
      
        }
      


          
      ImGui::End();

      // Gl Rendering
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT);
      ImGui::Render();
      glfwSwapBuffers(window);

    }  //end of while

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
    free_image(&a);
    free_image(&b);
    free(a.pixels);
    free(b.pixels);

    return 0;
} //end of main

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

ImGuiWindowFlags static_window()
{
    static bool no_titlebar = false;
    static bool no_border = true;
    static bool no_resize = false;
    static bool no_move = false;
    static bool no_scrollbar = false;
    static bool no_collapse = false;
    static bool no_menu = true;

    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (!no_border)   window_flags |= ImGuiWindowFlags_ShowBorders;
    if (no_resize)    window_flags |= ImGuiWindowFlags_NoResize;
    if (no_move)      window_flags |= ImGuiWindowFlags_NoMove;
    if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (no_collapse)  window_flags |= ImGuiWindowFlags_NoCollapse;
    if (!no_menu)     window_flags |= ImGuiWindowFlags_MenuBar;
    return window_flags;
}

/*

int previous = (int) ((a.width * RGBA)*((ceil(a.height/2))-2))+(floor(a.width/2)*BLOCK_SIZE); //Previous index for filling the pixels
    int start = (int) ((a.width * RGBA)*((ceil(a.height/2))-1))+(floor(a.width/2)*BLOCK_SIZE); //Starting point, middle point of matrix
    int next = (int) ((a.width * RGBA)*ceil(a.height/2))+(floor(a.width/2)*BLOCK_SIZE); //Next row index
   







  for(int j = 0; j<len;j++) pixels[j] = (float)(255);
    for(int i=0; i < len ; i++){
      pixels[start] = (float) a.pixels[start+4]+3*fib_3;
       pixels[previous] = (float) a.pixels[previous+4]+3*fib_3;
       pixels[next] = (float) a.pixels[next+4]+3*fib_3;


        //Recreate Fibonacci series
     for(int k = 0; k<4; k++)  { 
         //Fill previous row
    if( previous<len && k==6){ //Accessing R component
        
        pixels[previous] = a.pixels[previous]*fib_3;
       if( previous+BLOCK_SIZE +k  < len   ) pixels[previous+BLOCK_SIZE] = (float)a.pixels[previous+BLOCK_SIZE]*(255);
       if( previous-BLOCK_SIZE + k  <len ) pixels[previous-BLOCK_SIZE] = (float)a.pixels[previous-BLOCK_SIZE]*(255);
       
            previous=start;
      }else {
            previous = (a.width/2 * RGBA)*2;
            continue;
        }

        //Fill same row both sides

        if( start<len && k==8){ //Accesing G component
    
        pixels[start] = a.pixels[start]*fib_3;
       if( start + BLOCK_SIZE +k  <len ) pixels[start + BLOCK_SIZE ] = (float) a.pixels[start + BLOCK_SIZE ]* (255);
      if( start - BLOCK_SIZE +k < len )  pixels[start - BLOCK_SIZE ] =(float) a.pixels[start - BLOCK_SIZE ]*(255);
          start = next;
       }else {
            start = (a.width/2 * RGBA);
            continue;
        }
      

        //Fill next row
       if(next<len && k==2){ //Accessing B component
       
        pixels[next] = a.pixels[next+2]*fib_3;
        if(next+BLOCK_SIZE < len && pixels[next+BLOCK_SIZE] == a.pixels[next+BLOCK_SIZE+2])pixels[next+BLOCK_SIZE] 
        =(float)(255); 

        if(next+BLOCK_SIZE < len  && pixels[next-BLOCK_SIZE] == a.pixels[next-BLOCK_SIZE+2]) pixels[next-BLOCK_SIZE] 
        = (float)(255); 

        next = next + (4*BLOCK_SIZE);
        }else {
            next = (a.width/2 * RGBA)*3;
            continue;
        }
    }
     
  }*/